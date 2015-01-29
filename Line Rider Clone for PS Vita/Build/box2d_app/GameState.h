#ifndef GAMESTATE_H
#define GAMESTATE_H

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
#include <math.h>
#include "Draw_LINES.h"
#include "OptionsState.h" 

class GameState : public State
{
public:
	GameState(abfw::Platform* platform);
	~GameState();

	State* Update(abfw::TouchInputManager*, abfw::ControllerInputManagerVita&, abfw::AudioManagerVita*, float ticks);
	void Draw();
	void Exit(abfw::AudioManagerVita*);
	void Enter(abfw::AudioManagerVita*);
	Draw_LINES get_draw_lines();
	Draw_LINES get_draw_curves();
	bool get_player_placed();

private:
	abfw::Platform* platform_;
	b2World* world_;

	void Controller_input(abfw::ControllerInputManagerVita);
	void Touch_input(abfw::TouchInputManager*);
	void Collision_response(abfw::AudioManagerVita* audio_manager_);
	bool touch_updated;
	bool player_placed;
	bool eraser_placed;
	bool engine_running;
	void create_player(const abfw::Touch&);
	void delete_physics(b2Body*&);
	bool friction_sound_timer();
	bool friction_bool;
	
	Draw_LINES draw_lines;
	Draw_LINES draw_curves;

	enum Player
	{
		Circle_player,
		Car_player
	};

	Player player;

	b2Body* Circle_body; //hamster ball
	b2Body* Left_wheel;
	b2Body* Right_wheel;
	b2RevoluteJoint* Left_wheel_joint;
	b2RevoluteJoint* Right_wheel_joint;
	b2Body* Car_body;
	b2Body* Ground;
	b2Body* Eraser;
	b2MouseJoint* Mouse; //mouse joint for eraser

	enum draw_mode
	{
		draw_enter,
		draw_line,
		draw_curve,
		draw_erase,
		place_character
	};

	draw_mode mode;

	Int32 engine_sound;
	Int32 game_music;
	Int32 friction_sound;
	abfw::VolumeInfo volume_info;
	float friction_timer;

public:
	draw_mode get_draw_mode()
	{
		return mode;
	}

	GameObject line_button;
	GameObject curve_button;
	GameObject eraser_button;
	GameObject clear_button;
	GameObject place_player_button;
	GameObject game_back_button;
	GameObject player_vita;
	GameObject player_vita_penguin;
	GameObject left_wheel_sprite;
	GameObject right_wheel_sprite;
	GameObject car_body_sprite;
	GameObject player_eraser;
	GameObject background;
	
	int player_selection;
	int character_selection;
	bool newly_entered_draw_curve;

	std::vector<b2Body*> player_body; //mainly used to keep all the parts of the car together

	Player get_player()
	{
		return player;
	}
};

#endif