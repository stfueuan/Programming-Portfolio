#include "Draw_LINES.h"

Draw_LINES::Draw_LINES()
{
}

Draw_LINES::~Draw_LINES()
{
}

void Draw_LINES::calculate_bezier_curve(std::vector<b2Vec2> p, b2World* world)
{
	//Quartic Bezier Curve Equation
	//T(0) = (1 - t)^4.P(0) + 4(1 - t)^3.t.P(1) + 6(1 - t)^2.t^2.P(2) + 4(1 - t).t^3.P(3) + t^4.P(4)

	float point_x, point_y;
	float old_x, old_y;

	point_x = p[0].x;
	point_y = p[0].y;

	int j = edge.size();

	for (float t = 0; t < 1.01; t += 0.05) //increment is usually 0.01 to provide the smoothest curve possible, taking this to 0.05 increases performance significantly and does not really sacrifice curve smoothness
	{
		line.push_back(sprite_edge);
		edge.push_back(b2_edge);

		//get the previous point calculated
		old_x = point_x;
		old_y = point_y;

		//get the new point
		point_x = ((1 - t)*(1 - t)*(1 - t)*(1 - t))*p[0].x + (4*((1 - t)*(1 - t)*(1 - t)))*t*p[1].x + (6*((1 - t)*(1 - t)))*(t*t)*p[2].x + (4*(1 - t))*(t*t*t)*p[3].x + (t*t*t*t)*p[4].x;
		point_y = ((1 - t)*(1 - t)*(1 - t)*(1 - t))*p[0].y + (4*((1 - t)*(1 - t)*(1 - t)))*t*p[1].y + (6*((1 - t)*(1 - t)))*(t*t)*p[2].y + (4*(1 - t))*(t*t*t)*p[3].y + (t*t*t*t)*p[4].y;

		//used to give the draw_lines function a start and end point
		old_curved_vec.push_back(b2Vec2(old_x, old_y));
		new_curved_vec.push_back(b2Vec2(point_x, point_y));
	}

	for (int i = j; i < edge.size(); i++)
	{
		draw_lines(old_curved_vec[i], new_curved_vec[i], i, world);
	}
}

void Draw_LINES::draw_lines(b2Vec2 v1, b2Vec2 v2, int f, b2World* world)
{
	//find line length
	float length_x = v2.x - v1.x;
	float length_y = v2.y - v1.y;

	//find line position
	float pos_x = v1.x + (length_x/2);
	float pos_y = v1.y + (length_y/2);
		
	//create edgeshape and associated sprite
	edge[f] = EdgeShape(v1, v2, world);
	line[f].set_width(BOX2D_GFX_SIZE(sqrt((length_x*length_x) + (length_y*length_y))));
	line[f].set_height(1.0f);
	line[f].set_position(abfw::Vector3(BOX2D_GFX_POS_X(pos_x), BOX2D_GFX_POS_Y(pos_y), 0));
	line[f].set_rotation(-atanf(length_y/length_x));
	line[f].set_colour(0xff000000);
	edge[f]->SetUserData(&line[f]);
}

void Draw_LINES::clear_world() //delete all lines and curves
{
	int size = edge.size();
	for (int i = 0; i < size; i++)
	{
		b2Body *thisEdge = edge[i];
		b2World* thisWorld_ = thisEdge->GetWorld();
		thisWorld_->DestroyBody(thisEdge);
		edge[i] = NULL;
	}

	line.clear();
	edge.clear();
	old_curved_vec.clear();
	new_curved_vec.clear();
	point.clear();
}