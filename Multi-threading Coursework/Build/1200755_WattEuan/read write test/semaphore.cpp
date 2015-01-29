#include "semaphore.h"

using boost::unique_lock;
using boost::mutex;
using boost::condition_variable; 

typedef unique_lock<mutex> mutex_lock;

semaphore::semaphore(int initial_count)
	: counter(initial_count)
{
}

void semaphore::signal()
{
	//signal to other threads that the operation is available
	mutex_lock lock(counter_lock);
	counter++;
	wake_up.notify_one();
}

void semaphore::wait()
{
	//block operation from running until there is space available
	mutex_lock lock(counter_lock);
	while (true)
	{
		if (counter > 0)
		{
			counter--;
			return;
		}

		wake_up.wait(lock);
	}
}