#ifndef DRAW_LINES_H
#define DRAW_LINES_H

#include "Box2D_Bodies.h"
#include "box2d_helpers.h"
#include <iostream>
#include <math.h>
#include <vector>
#include <Box2D\Box2D.h>
#include <graphics/sprite.h>
#include <system/platform.h>
#include <iostream>
#include <cstdlib>

class Draw_LINES
{
public:
	Draw_LINES();
	~Draw_LINES();

	void calculate_bezier_curve(std::vector<b2Vec2>, b2World*);
	void draw_lines(b2Vec2, b2Vec2, int, b2World*);
	void clear_world();

	std::vector<abfw::Sprite> line;
	std::vector<b2Body*> edge;
	abfw::Sprite sprite_edge;
	std::vector<b2Vec2> old_curved_vec;
	std::vector<b2Vec2> new_curved_vec;
	b2Body* b2_edge;

	b2Vec2 v1;
	b2Vec2 v2;

	std::vector<b2Vec2> point;
};

#endif // DRAW_LINES_H