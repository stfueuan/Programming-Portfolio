#include "Channel.h"

semaphore channel_semaphore(0);

void Channel::write(set<string> pins)
{
	boost::unique_lock<mutex> mutex_lock(list_mutex);

	string_list.clear(); //clear all previous pins as an entire new list is being added

	for(set<string>::iterator pin = pins.begin(); pin != pins.end(); pin++)
	{
		string_list.push_back(*pin); //push new pin into set
	}

	channel_semaphore.signal(); //signal to say that a new string is in the list
}

string Channel::read()
{
	string passed_pin = ""; //set string to NULL

	while(passed_pin == "")
	{
		channel_semaphore.wait(); //block function from running until list is populated, then block again
		boost::unique_lock<mutex> mutex_lock(list_mutex);
		passed_pin = string_list.front(); //set string to first value in list
		string_list.pop_front(); //delete first element in list
	}

	return passed_pin;
}

int Channel::string_list_size()
{
	return string_list.size();
}