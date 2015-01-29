#include "AnimatedSprite.h"

void AnimatedSprite::LoadSprite(LPSTR filename, int xpos, int ypos, int w, int h, int s)
{
	x = xpos;
	y = ypos;
	frameWidth = w;
	frameHeight = h;
	frameCounter = 0;
	speed = s;
	offsetX = 0;
	offsetY = 0;
	LoadABitmap(filename);
}

void AnimatedSprite::Update()
{
	frameCounter++;

	if(frameCounter >= speed)
	{
		// update offsets and reset counter
		offsetX += frameWidth;

		if(offsetX > width - frameWidth)
		{
			// offset is outside sprite size
			offsetX = 0;
		}

	frameCounter = 0;
	}
}