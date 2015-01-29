#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <boost/thread.hpp>

class semaphore
{
public:
	semaphore(int initial_count);
	void signal();
	void wait();

private:
	int counter;

	boost::mutex counter_lock;
	boost::mutex wakeup_lock;
	boost::condition_variable wake_up;
};

#endif SEMAPHORE_H