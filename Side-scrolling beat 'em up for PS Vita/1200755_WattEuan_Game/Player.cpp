#include "Player.h"

void Player::init()
{
	invincible = false;
	player_dead = false;
	jump = false;
	dead = false;
	state = idle;
	timer = 0;
	jump_frame = 0;
	death_frame = 0;
	lives = 3;
	anim_step_length = 1.0f/12.0f;
}

void Player::apply_damage()
{
	lives -= 1;
	if (lives < 0) 
	{
		lives = 0;
		state = death;
	}
}

void Player::react()
{
	timer++;

	if (timer < 120)
	{
		if (timer%10)
		{
			set_colour(0xff0000ff);
		}
		else 
		{
			set_colour(0x00000000);
		}
	}
	else
	{
		set_colour(0xffffffff);
		invincible = false;
		timer = 0;
		state = idle;
	}
	//reaction to getting hurt - change alpha value to flash sprite and change colour to red, set hit box to null for certain time
}

void Player::update()
{
	if(state == idle)
	{
		Animate(12, 5, anim_step_length);
		set_uv_position(abfw::Vector2(texture_x, 0.0f));
		x = GetX() - (width()/2);
		y = GetY() + 75;
		w = 140;
		h = 180;
	}
	else if (state == run)
	{
		Animate(10, 8, anim_step_length);
		set_uv_position(abfw::Vector2(texture_x, 1.0f/7.0f));
		x = GetX() - (width()/2);
		y = GetY() + 75;
		w = 140;
		h = 180;
	}
	else if(state == attack)
	{
		Animate(8, 6, anim_step_length);
		set_uv_position(abfw::Vector2(texture_x, 2.0f/7.0f));
		x = GetX() - (width()/2);
		y = GetY() + 60;
		w = 300;
		h = 240;
	}
	else if(state == attack2)
	{
		Animate(8, 6, anim_step_length);
		set_uv_position(abfw::Vector2(texture_x, 3.0f/7.0f));
		x = GetX() - (width()/2);
		y = GetY() + 50;
		w = 300;
		h = 200;
	}
	else if(state == attack3)
	{
		jump_frame += (anim_step_length/6);

		Animate(11, 6, anim_step_length);
		set_uv_position(abfw::Vector2(texture_x, 4.0f/7.0f));
		x = GetX() - (width()/2);
		y = GetY() - (height()/2 + 15);
		w = 280;
		h = 400;

		//when the player is visually off the 'floor', make 'jump' true and thus translate everything around the player
		if((jump_frame > anim_step_length) && (jump_frame < 5.0f/7.0f))
		{
			if (jump_frame > anim_step_length*6)
			{
				jump_frame = 0;
				jump = false;
			}
		}
	}
	else if(state == block)
	{
		Animate(8, 6, anim_step_length);
		set_uv_position(abfw::Vector2(texture_x, 5.0f/7.0f));
		x = GetX() - (width()/2);
		y = GetY() - (height()/2 + 15);
		w = 260;
		h = 400;
	}
	else if(state == hurt)
	{
		if (invincible)
		{
			react();
			Animate(12, 5, anim_step_length);
			set_uv_position(abfw::Vector2(texture_x, 0.0f));
		}
	}
	else if(state == death)
	{
		death_frame += (anim_step_length/6);
		Animate(9, 5, anim_step_length);
		set_uv_position(abfw::Vector2(texture_x, 6.0f/7.0f));

		if (death_frame > anim_step_length*6)
		{
			death_frame = 0;
			dead = true;
		}
	}
	else
	{
		state = idle;
	}
}