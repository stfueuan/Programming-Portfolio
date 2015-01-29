#ifndef STATE_H
#define STATE_H

#include <input/touch_input_manager.h>
#include <input/vita/controller_input_manager_vita.h>
#include <audio/vita/audio_manager_vita.h>


using namespace abfw;

class State
{
public:
	virtual ~State()
	{
	}

	virtual State* Update(abfw::TouchInputManager*, abfw::ControllerInputManagerVita&, abfw::AudioManagerVita*, float ticks) = 0; 
	virtual void Draw() = 0;
	virtual void Enter(abfw::AudioManagerVita*) = 0; //init
	virtual void Exit(abfw::AudioManagerVita*) = 0; //clean up
	virtual void Touch_input(abfw::TouchInputManager*) = 0;
	virtual void Controller_input(abfw::ControllerInputManagerVita) = 0;
};

#endif