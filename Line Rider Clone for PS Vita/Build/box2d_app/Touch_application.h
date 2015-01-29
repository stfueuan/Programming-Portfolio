#ifndef TOUCH_H
#define TOUCH_H

#include <system/application.h>
#include "game_object.h"
#include <graphics/font.h>
#include <input/vita/controller_input_manager_vita.h>
#include <input/touch_input_manager.h>
#include <system/vita/platform_vita.h>

namespace abfw
{
	class Platform;
	class TouchInputManager;
}

class TouchApplication
{
public:
	TouchApplication();
	~TouchApplication();
	void Init(abfw::Platform*);
	void CleanUp();
	bool Update(); 

	abfw::TouchInputManager* touch_manager_;
	//abfw::ControllerInputManagerVita controller_manager_;
	float frame_rate_;
};

#endif