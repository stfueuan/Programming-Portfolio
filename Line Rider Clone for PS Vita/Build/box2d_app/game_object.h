#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H

#include <graphics/sprite.h>

class GameObject : public abfw::Sprite
{
	public:
		GameObject();
		virtual ~GameObject();
		void move(float, float);
		void rebound();
		abfw::Vector2 velocity;
		void move_to(float, float);
		void MoveDeltaX(float);
		void MoveDeltaY(float);
		int GetX();
		int GetY();
		int GetX_vel();
		int GetY_vel();
		float Animate(int limit, int frames, float step_length, float ticks); //how many frames of animation, how many frames per frame of animation, how far to move through the image, ticks
		float texture_x; //how far through the animation stream the current animation is
		float frame_update; //counts up to the number of frames needed for each animation update
		int frame_number; //counts up to the number of tiles in each animation stream
		bool anim; //is the current animation still playing?
		bool active; //which background object is active 

		enum state
		{
			PLAYER,
			ENEMY,
			WATER
		};

		GameObject::state object_state;
		GameObject::state get_state();
};

#endif // _GAME_OBJECT_H