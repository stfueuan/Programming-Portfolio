#include "pin_set.h"

pin_set::pin_set()
{
	file_loaded = false;
}

pin_set::~pin_set()
{

}

void pin_set::load_file(std::string file)
{
	std::ifstream pin_file;

	pin_file.open(file); //read file in, create one of the same name if it does not already exist in root directory

	if(!pin_file.bad())
	{
		std::string line;

		//while(getline(pin_file, line))
		//{
		//	pins.insert(line); //populate set with contents of file
		//}

		pin_file.close();
	}
	else
	{
		std::cout << "Bad ptr to file!" << std::endl;
	}
}