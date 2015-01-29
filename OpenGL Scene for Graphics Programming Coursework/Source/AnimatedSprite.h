#ifndef ANIMATEDSPRITE_H
#define ANIMATEDSPRITE_H

#include "Sprite.h"

class AnimatedSprite : public Sprite
{
	public:
	void LoadSprite(LPSTR, int, int, int, int, int);// filename, x, y, frame w, h, speed
	void Update(); // will update the sprite animation
	int offsetX, offsetY, frameWidth, frameHeight, frameCounter, speed;
};

#endif