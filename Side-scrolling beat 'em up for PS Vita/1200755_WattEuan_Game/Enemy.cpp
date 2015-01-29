#include "Enemy.h"
#include <iostream>

Enemy::Enemy()
{

}
Enemy::~Enemy()
{
}

void Enemy::init()
{
	state = run;
	w = 125;
	h = 150;
	x = (GetX() - width()/2);
	y = (GetY() - (height()/2 + 25)); //25 is the enemy hit box offset
	death_frame =- 1.0;
	anim_step_length = (1.0f/7.0f); 
	set_uv_width(anim_step_length);
	set_uv_height(1.0f/6.0f);
	range = false;
	attack_frame = 0;
	attacks = false;
}

void Enemy::AI()
{
	if (state == run)
	{
		move(-2.5f, 0);
		x = (GetX() - width()/2 + 150);
		y = (GetY() - (height()/2 + 25)); //25 is the enemy hit box offset
		Animate(5, 5, anim_step_length);
		set_uv_position(abfw::Vector2(texture_x, 0.0f));
	}
	else if (state == walk)
	{
		move(2.5f, 0);
		x = (GetX() - width()/2 + 150);
		y = (GetY() - (height()/2 + 25)); //25 is the enemy hit box offset
		Animate(5, 5, anim_step_length);
		set_uv_position(abfw::Vector2(texture_x, 0.0f));
	}

	if (state == attack)
	{
		attack_frame += (anim_step_length/6);

		Animate(5, 6, anim_step_length);
		set_uv_position(abfw::Vector2(texture_x, 1.0f/6.0f));
		x = (GetX() - width()/2 + 150);
		y = (GetY() - (height()/2 + 25)); //25 is the enemy hit box offset

		//if the enemy is still doing their attack animation then do reset
		if(attack_frame > anim_step_length*3)
		{
			attacks = true;

			if (attack_frame > anim_step_length*5)
			{
				attack_frame = 0;
				attacks = false;
			}
		}	
	}
	else if (state == attack2)
	{
		attack_frame += (anim_step_length/6);

		Animate(7, 6, anim_step_length);
		set_uv_position(abfw::Vector2(texture_x, 1.0f/3.0f));
		x = (GetX() - width()/2 + 150);
		y = (GetY() - (height()/2 + 25)); //25 is the enemy hit box offset

		if(attack_frame > anim_step_length*5)
		{
			attacks = true;

			if (attack_frame > anim_step_length*7)
			{
				attack_frame = 0;
				attacks = false;
			}
		}		
	}
	else if (state == attack3)
	{
		attack_frame += (anim_step_length/6);

		Animate(5, 6, anim_step_length);
		set_uv_position(abfw::Vector2(texture_x, 0.5f));
		x = (GetX() - width()/2 + 150);
		y = (GetY() - (height()/2 + 25)); //25 is the enemy hit box offset

		if(attack_frame > anim_step_length*3)
		{
			attacks = true;

			if (attack_frame > anim_step_length*5)
			{
				attack_frame = 0;
				attacks = false;
			}
		}		
	}
	else if (state == block)
	{
		Animate(3, 20, anim_step_length);
		set_uv_position(abfw::Vector2(texture_x, 5.0f/6.0f));
		state = run;
	}
	else if (state == death)
	{
		death_frame += anim_step_length;

		Animate (5, 10, anim_step_length);
		set_uv_position(abfw::Vector2(texture_x, 2.0f/3.0f));

		//ensure full death animation is played before reset
		if(death_frame > anim_step_length*4)
		{
			anim = true;

			if (death_frame > anim_step_length*6)
			{
				death_frame = 0;
				anim = false;
			}
		}
	}

	//is enemy is within range of player then change to attack states
	if (range == true)
	{
		if (state != attack && state != attack2 && state != attack3)
		{
			int temp = rand()%90;

			if (temp < 30)
			{
				state = attack;
			}
			else if ((temp > 30) && (temp < 60))
			{
				state = attack2;
			}
			else
			{
				state = attack3;
			}
		}
	}
}

void Enemy::react()
{
	//give a 50/50 chance for the enemy to block
	if(rand()%100 < 50)
	{
		state = block;
	}
}

void Enemy::update()
{
	AI();
}