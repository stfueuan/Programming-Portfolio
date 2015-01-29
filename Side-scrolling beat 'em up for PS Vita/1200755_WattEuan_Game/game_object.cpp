#include "game_object.h"

GameObject::GameObject()
{
	velocity = abfw::Vector2(0, 0);
	texture_x = 0.0f;
	frame_update = 0;
	anim = false;
	active = false;
	frame_number = 0;
}

GameObject::~GameObject()
{
}

void GameObject::move(float x, float y)
{
	velocity.x = x;
	velocity.y = y;
	set_position(abfw::Vector3(position() + abfw::Vector3(velocity.x, velocity.y, 0)));
}

void GameObject::move_to(float x, float y)
{
	//move to specified x and y position
	set_position(abfw::Vector3(x, y, position().z));
}

void GameObject::MoveDeltaX(float x)
{
	//move to specified x position
	set_position(abfw::Vector3(x, position().y, position().z));
}

void GameObject::MoveDeltaY(float y)
{
	//move to specified y position
	set_position(abfw::Vector3(position().x, y, position().z));
}

int GameObject::GetX()
{
	return position().x;
}

int GameObject::GetY()
{
	return position().y;
}

int GameObject::GetX_vel()
{
	return velocity.x;
}

void GameObject::Animate(int limit, int frames, float step_length)
{
	frame_update++;

	if (frame_update >= frames)
	{
		frame_update = 0;
		frame_number++;
		
		if(frame_number >= limit)
		{
			frame_number = 0;
			anim = true;
		}

		texture_x = step_length*frame_number;
	}
}
