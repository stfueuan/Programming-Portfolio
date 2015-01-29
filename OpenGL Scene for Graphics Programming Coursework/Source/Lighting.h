#ifndef LIGHTING_H
#define LIGHTING_H

#include <windows.h>
#include <stdio.h>
#include <string>
#include <mmsystem.h>
#include <math.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "TokenStream.h"
#include "Vec3.h"

//for a directional light - set w component to 0
//for a point light - set w component to 1
//for spot light - set w component to 1

class Lighting
{
public:
	Lighting();
	~Lighting();
	void init();

	void directional_light(int light_num, GLfloat light_pos[], GLfloat diff_colour[], GLfloat specular[]); //point down a specified direction
	void spot_light(int light_num, GLfloat light_pos[], GLfloat diff_colour[], GLfloat specular[], GLfloat direction[]);
	void point_light(int light_num, GLfloat light_pos[], GLfloat diff_colour[], GLfloat specular[], float attenuation); //have no specified direction (like the sun)
	const GLfloat* set_light_material();
	const GLfloat* set_light_colour();

	void red_light(int light_num, GLfloat light_pos[]);
	void green_light(int light_num, GLfloat light_pos[]);
	void blue_light(int light_num, GLfloat light_pos[]);
	void white_light(int light_num, GLfloat light_pos[]);

	void rubber(); //no spec, no shiny
	void metal(); //high spec, high shiny
	void plastic(); //spec, shiny

private:
	//initialise material values
	const GLfloat* set_light_ambient();
	GLfloat no_mat[4];
	GLfloat mat_ambient[4];
	GLfloat mat_diffuse[4];

	GLfloat mat_emission[4];
	
	GLfloat no_shininess[1];
	GLfloat low_shininess[1];
	GLfloat high_shininess[1];

	GLfloat no_spec[4];
	GLfloat low_spec[4];
	GLfloat high_spec[4];

	float attenuation;
};

#endif

