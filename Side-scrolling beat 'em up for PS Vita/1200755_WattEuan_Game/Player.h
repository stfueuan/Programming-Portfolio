#ifndef PLAYER_H
#define PLAYER_H

#include "game_object.h"
#include <iostream>

class Player : public GameObject
{
public:
	enum Anim
	{
		idle,
		run,
		attack,
		attack2,
		attack3,
		block,
		hurt,
		death
	};

	void init();
	void update();
	void apply_damage();
	bool invincible; //am I invincible?
	bool player_dead; //check if the player is dead
	Anim state;
	bool jump; //am I jumping?
	bool dead; //check if the death animation has finished
	float w; //bottom right corner
	float h; //bottom left corner
	float x; //top left corner
	float y; //top right corner
	float lives;

private:
	void react();
	int timer; //counts how long the player has been invincible
	float jump_frame; //counts how many frames through the current attack animation the player is through
	float death_frame; //counts how many frames through the death animation the player is through
	float anim_step_length; //step_length being the width of each sprite in terms of UV
};

#endif