#ifndef PINSET_H
#define PINSET_H

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <set>

class pin_set
{
public:
	pin_set();
	~pin_set();
	
	std::set<std::string> pins;
	bool file_loaded;

	void load_file(std::string);
};

#endif