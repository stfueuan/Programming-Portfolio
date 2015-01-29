#ifndef HOWTOPLAY_H
#define HOWTOPLAY_H

#include "State.h"
#include <input/vita/controller_input_manager_vita.h>
#include <Box2D\Box2D.h>
#include "game_object.h"
#include <input/touch_input_manager.h>
#include <audio/vita/audio_manager_vita.h>
#include <system/platform.h>
#include "box2d_helpers.h"
#include "Box2D_Bodies.h"
#include <vector>
#include <iostream>
#include <cstdlib>

class HowToPlayState : public State
{
public:
	HowToPlayState(abfw::Platform* platform);
	~HowToPlayState();
	State* Update(abfw::TouchInputManager*, abfw::ControllerInputManagerVita&, abfw::AudioManagerVita*, float ticks);
	void Draw();
	void Enter(abfw::AudioManagerVita*);
	void Exit(abfw::AudioManagerVita*);
	void Controller_input(abfw::ControllerInputManagerVita);
	void Touch_input(abfw::TouchInputManager*);

	GameObject howtoplay_background;
	GameObject main_menu_button;

private:
	abfw::Platform* platform_;
};

#endif //HOWTOPLAY_H