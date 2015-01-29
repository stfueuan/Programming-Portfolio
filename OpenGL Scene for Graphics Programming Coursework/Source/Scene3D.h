#ifndef SCENE3D_H
#define SCENE3D_H

#include <windows.h>
#include <stdio.h>
#include <mmsystem.h>
#include <math.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "Input.h"
#include "Camera.h"
#include "SOIL.h"
#include "cube.h"
#include "ModelClass.h"
#include "TokenStream.h"
#include "Vec3.h"
#include "Lighting.h"
#include "Disc.h"
#include "Cylinder.h"
#include "Sphere.h"
#include "ShadowsandMirrors.h"
#include <time.h>

class Scene3D
{
	struct fire_object
	{
		GLuint list; //fire display list
		bool is_active; 
		GLfloat fire_x; //fire position
		GLfloat fire_y;
		GLfloat fire_z;
		GLfloat colour[3]; //fire colour
		GLfloat light_number;
	};

	public:
		void Init(HWND&); //initialise function
		void DrawScene(Input&); // render scene
		void Resize(HWND&);
		void wire();
		bool wired;
		void skybox();
		void display_list1();
		void floor();
		void plane();

		enum Vector
		{
			x, 
			y,
			z
		};

	private:
		void ResizeGLWindow(int, int); //width and height
		void InitializeOpenGL(int, int); // width and height
		GLuint Dlist; //cube
		GLuint floor_dlist; 
		GLuint tree_dlist;
		GLuint tree_dlist2;
		GLuint log_dlist;
		GLuint wall_dlist;
		void camera_func(Input&);
		void wall();
		void room();
		void textures();
		void fire(int fire_amount, int fire_height, float fire_pos[2]);
		void fire_construction(int fire_number, int fire_height);
		void fire_input(Input&);
		void tree();
		void tree2();
		void log();
		void shadows();
		void objects();
	//vars
	HWND hwnd;
	RECT screenRect;
	HDC hdc;
	HGLRC hrc; //hardware RENDERING CONTEXT
	int s_wdith, s_height;
	Camera camera;
	GLuint myTexture;
	GLuint large_texture;
	POINT point;
	ModelClass tree_model;
	ModelClass caveman_model;
	ModelClass chicken_model;
	ModelClass wolf_model;
	ModelClass bird_model;
	Lighting lighting;
	ShadowsandMirrors SaM;
	ShadowsandMirrors tree_shadow;
	ShadowsandMirrors tree_shadow2;
	ShadowsandMirrors caveman_shadow;
	ShadowsandMirrors wolf_shadow;
	ShadowsandMirrors chicken_shadow;

	//texture variables
	GLuint sky_front;
	GLuint sky_back;
	GLuint sky_top;
	GLuint sky_bottom;
	GLuint sky_left;
	GLuint sky_right;
	GLuint tree_texture;
	GLuint bark_texture;

	//fire variables
	std::vector<fire_object> fire_cubes;
	std::vector<fire_object> fire_outliers;
	fire_object fired;
	int fire_height;
	int fire_outliers_height;
	float shadow_alpha;
	float point_light_attenuation;

	//for moving light
	float angle;
	float radius;
	float speed;
	float moving_light_x;
	float moving_light_z;

	//for bird
	float bird_angle;
	float bird_angle2;
	float bird_rotation;
	float bird_x;
	float bird_z;
};

#endif