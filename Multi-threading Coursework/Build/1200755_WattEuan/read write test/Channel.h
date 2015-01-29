#ifndef CHANNEL_H
#define CHANNEL_H

#include <Windows.h>
#include "semaphore.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <boost\thread.hpp>
#include <set>

using namespace std;
using boost::mutex;

class Channel
{
private:
	list<string> string_list;
	mutex list_mutex;

public:
	void write(set<string>);
	string read();
	int string_list_size();
};

#endif