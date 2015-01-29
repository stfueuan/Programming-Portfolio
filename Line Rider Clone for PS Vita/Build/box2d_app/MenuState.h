#ifndef MENUSTATE_H
#define MENUSTATE_H

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

class MenuState : public State
{
public:
	MenuState(abfw::Platform* platform);
	~MenuState();
	State* Update(abfw::TouchInputManager*, abfw::ControllerInputManagerVita&, abfw::AudioManagerVita*, float ticks);
	void Draw();
	void Enter(abfw::AudioManagerVita*);
	void Exit(abfw::AudioManagerVita*);
	void Controller_input(abfw::ControllerInputManagerVita);
	void Touch_input(abfw::TouchInputManager*);

	GameObject start_game;
	GameObject options;
	GameObject credits;
	GameObject title_screen;
	Int32 menu_music;
	Int32 bing_sound;

private:
	abfw::Platform* platform_;
};

#endif