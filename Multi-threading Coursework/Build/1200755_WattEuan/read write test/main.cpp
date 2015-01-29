#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <boost\thread.hpp>
#include <boost\random.hpp>
#include "semaphore.h"
#include "pin_set.h"
#include <set>
#include "Channel.h"

//Timing stuff
#ifdef _WIN32
#include <sys/types.h>
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif

float timer = 0;
//

#define NO_OF_CORES 7

using namespace std;
using boost::thread;
using boost::mutex;

pin_set Pins;
Channel channel;
boost::mt19937 rng; //boost random number generator, mersenne twister
boost::uniform_int<> range(0, 9); //initialise range of generated numbers
boost::variate_generator<boost::mt19937&, boost::uniform_int<>> next_number(rng, range); //get the next 'random' number
mutex file_mutex;
mutex check_mutex;

//timer function
long long int get_time()
{
	#ifdef _WIN32
		struct _timeb timebuffer;
		_ftime64_s(&timebuffer);
		return (timebuffer.time * 1000LL) + timebuffer.millitm;
	#else
		struct timeval tv;
		gettimeofday(&tv, NULL);
		return (tv.tv_sec * 1000LL) + (tv.tv_usec / 1000LL);
	#endif
}

string get_pin()
{
	string pin = "";
	int pin_array[4];
	char pins[4];

	for(int i = 0; i < 4; i++)
	{
		pin_array[i] = next_number(); //produce a random 4 digit code using thread-safe rng

		pin += itoa(pin_array[i], pins, 10); //convert int array to string, base 10 for decimal output
	}	

	return pin;
}

void read()
{
	string read_pin = ""; //set read_pin to NULL

	read_pin = get_pin();

	check_mutex.lock();

	if(Pins.file_loaded)
	{
		for(set<string>::iterator pin_finder = Pins.pins.begin(); pin_finder != Pins.pins.end(); pin_finder++)
		{
			if(Pins.pins.find(read_pin) == pin_finder) //compare pin with set of pins
			{
				//cout << read_pin << endl; //write previously used pin to screen
				read_pin.clear(); //resets the string so new pin gets a clear run
				read_pin = get_pin(); //get a new pin if previous was a match
				break; //start loop again
			}
		}

		Pins.pins.insert(read_pin); //write pin to set
	}
	else
	{
		file_mutex.lock();

		Pins.load_file("pin-numbers.txt");
		Pins.file_loaded = true;

		file_mutex.unlock();
	}

	check_mutex.unlock();

	channel.write(Pins.pins); //write pin set to channel for write_thread to pick up later
}

void write()
{
	ofstream write_pin;

	file_mutex.lock();

	write_pin.open("pin-numbers.txt"); //writes to file
	
	if(!write_pin.bad())
	{
		while(channel.string_list_size() != 0)
		{
			string get_pins = channel.read();
			write_pin << get_pins << "\n"; //write string 'pin' to file
			get_pins.clear(); //resets the string for next loop
		}
	}
	else
	{
		cout << "Cannot write to file" << endl;
	}

	file_mutex.unlock();
}

void main()
{
	rng.seed(time(0));

	thread* read_thread[NO_OF_CORES];
	thread* write_thread[1]; //only have one thread to write the file, means only opening file once improving performance

	long long int start = get_time();
	for(int i = 0; i < 2000/NO_OF_CORES; i++)
	{	
		for(int threads = 0; threads < NO_OF_CORES; threads++)
		{
			read_thread[threads] = new thread(read);
		}

		for(int threads = 0; threads < NO_OF_CORES; threads++)
		{
			read_thread[threads]->join();
			delete read_thread[threads];
		}
	}

	write_thread[0] = new thread(write);
	write_thread[0]->join();
	delete write_thread[0];

	long long int end = get_time();
	timer = (end-start);

	cout << timer << endl;
	
	cin.get();
}