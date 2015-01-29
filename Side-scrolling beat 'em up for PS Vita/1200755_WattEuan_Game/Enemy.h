#ifndef ENEMY_H
#define ENEMY_H

#include "game_object.h"

class Enemy : public GameObject
{
public:
	Enemy();
	~Enemy();

	enum Anim
	{
		run,
		walk,
		attack,
		attack2,
		attack3,
		block,
		death
	};

	void init();
	void update();
	void react(); //put knock back here
	bool range; //determines whether the enemy is within attack range of the player
	Anim state;
	float w; //bottom right corner
	float h; //bottom left corner
	float x; //top left corner
	float y; //top right corner
	bool attacks; //is the enemy attacking
private:
	void AI();
	float attack_frame; //counts how many frames through the current attack animation the enemy is through
	float death_frame; //counts how many frames through the death animation the enemy is through
	float anim_step_length; //step_length being the width of each sprite in terms of UV
};

#endif