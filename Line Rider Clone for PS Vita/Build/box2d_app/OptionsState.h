#ifndef OPTIONS_H
#define OPTIONS_H

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

class OptionsState : public State
{
public:
	OptionsState(abfw::Platform* platform);
	~OptionsState();
	State* Update(abfw::TouchInputManager*, abfw::ControllerInputManagerVita&, abfw::AudioManagerVita*, float ticks);
	void Draw();
	void Enter(abfw::AudioManagerVita*);
	void Exit(abfw::AudioManagerVita*);
	void Controller_input(abfw::ControllerInputManagerVita);
	void Touch_input(abfw::TouchInputManager*);

	GameObject options_screen;
	GameObject choose_ball;
	GameObject choose_car;
	GameObject choose_penguin;
	GameObject choose_mouse;
	GameObject start_game_button;

private:
	enum player_select //select vehicle
	{
		circle_player,
		car_player
	};

	enum character_select //select sprite
	{
		penguin_character,
		mouse_character
	};

	player_select Select;
	character_select Character;

	abfw::Platform* platform_;

public:
	player_select get_selection()
	{
		return Select;
	}

	character_select get_character()
	{
		return Character;
	}
};


#endif