#include "Lighting.h"

Lighting::Lighting()
{
}

Lighting::~Lighting()
{
}

void Lighting::init()
{
	glShadeModel(GL_SMOOTH); // Enable Smooth Shading
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); //Stop light being applied to both sides of each poly
	
	//material values
	no_mat[0] = no_mat[1] = no_mat[2] = no_mat[3] = 0.0f;
	mat_ambient[0] = mat_ambient[1] = mat_ambient[2] = 0.7f;
	mat_ambient[3] = 1.0f;
	//mat_emission[4] = (0.3f, 0.2f, 0.2f, 0.0f);

	//shininess
	no_shininess[0] = 0.0f;
	low_shininess[0] = 50.0f;
	high_shininess[0] = 100.0f;

	//specular
	no_spec[0] = no_spec[1] = no_spec[2] = 0.0f;
	no_spec[3] = 1.0f;

	low_spec[0] = low_spec[1] = low_spec[2] = low_spec[3] = 1.0f;
	high_spec[0] = high_spec[1] = high_spec[2] = 10.0f;
	high_spec[3] = 1.0f;

//	GLfloat no_mat[] = {0.0, 0.0, 0.0, 0.0};
//	GLfloat mat_ambient[] = {0.7, 0.7, 0.7, 1.0};
//	GLfloat mat_ambient_colour[] = {0.8, 0.8, 0.2, 1.0};
//	GLfloat mat_diffuse[] = {0.1, 0.5, 0.8, 1.0};
//	GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
//	GLfloat no_shininess[] = {0.0};
//	GLfloat low_shininess[] = {50};
//	GLfloat high_shininess[] = {100};
//	GLfloat mat_emission[] = {0.3, 0.2, 0.2, 0.0};
//	GLfloat highSpec[] = {10.0, 10.0, 10.0, 1.0};
//	GLfloat Spec[] = {1.0, 1.0, 1.0, 1.0};
//	GLfloat shininess[] = {100.0};
}

const GLfloat* Lighting::set_light_ambient()
{
	//set how much the scene is lit regardless of included lights
	GLfloat Light_Ambient[] = {0.1f, 0.1f, 0.1f, 1.0f};

	const GLfloat* lightAmbient[] = {&Light_Ambient[0], &Light_Ambient[1], &Light_Ambient[2], &Light_Ambient[3]};

	return *lightAmbient;
}

void Lighting::directional_light(int light_num, GLfloat light_pos[], GLfloat diff_colour[], GLfloat specular[])
{
	//Use this function to set up an entirely new directional light from scratch
	glLightfv(light_num, GL_AMBIENT, set_light_ambient());
	glLightfv(light_num, GL_DIFFUSE, diff_colour);
	glLightfv(light_num, GL_SPECULAR, specular);
	glLightfv(light_num, GL_POSITION, light_pos);
	glEnable(light_num);

	for (int i = 0; i < 4; i++)
	{
		mat_diffuse[i] = diff_colour[i];
	}
}

void Lighting::point_light(int light_num, GLfloat light_pos[], GLfloat diff_colour[], GLfloat specular[], float attenuation)
{
	//Use this function to set up an entirely new point light from scratch
	glLightfv(light_num, GL_AMBIENT, set_light_ambient());
	glLightfv(light_num, GL_DIFFUSE, diff_colour);
	glLightfv(light_num, GL_SPECULAR, specular);
	glLightfv(light_num, GL_POSITION, light_pos);
	glLightf(light_num, GL_CONSTANT_ATTENUATION, attenuation);
	glLightf(light_num, GL_LINEAR_ATTENUATION, 0.2f); 
	glLightf(light_num, GL_QUADRATIC_ATTENUATION, 0.0f);
	glEnable(light_num);
}

void Lighting::spot_light(int light_num, GLfloat light_pos[], GLfloat diff_colour[], GLfloat specular[], GLfloat direction[])
{
	//Use this function to set up an entirely new spot light from scratch
	glLightfv(light_num, GL_AMBIENT, set_light_ambient());
	glLightfv(light_num, GL_DIFFUSE, diff_colour);
	glLightfv(light_num, GL_SPECULAR, specular);
	glLightfv(light_num, GL_POSITION, light_pos);
	glLightf(light_num, GL_CONSTANT_ATTENUATION, 1.0f);
	glLightf(light_num, GL_LINEAR_ATTENUATION, 0.2f);
	glLightf(light_num, GL_QUADRATIC_ATTENUATION, 0.0f);
	glLightf(light_num, GL_SPOT_CUTOFF, 45.0f); //consider passing in cut-off angle
	glLightfv(light_num, GL_SPOT_DIRECTION, direction);
	glLightf(light_num, GL_SPOT_EXPONENT, 50.0f);
	glEnable(light_num);
}

const GLfloat* Lighting::set_light_material()
{
	return 0;
}

const GLfloat* Lighting::set_light_colour()
{
	return 0;
}

void Lighting::red_light(int light_num, GLfloat light_pos[])
{
	GLfloat diffuseLight[] = {1.0f, 0.0f, 0.0f, 1.0f};

	glLightfv(light_num, GL_AMBIENT, set_light_ambient());
	glLightfv(light_num, GL_DIFFUSE, diffuseLight);
	glLightfv(light_num, GL_SPECULAR, low_spec);
	glLightfv(light_num, GL_POSITION, light_pos);
	glLightf(light_num, GL_CONSTANT_ATTENUATION, 1.0f);
	glLightf(light_num, GL_LINEAR_ATTENUATION, 0.2f);
	glLightf(light_num, GL_QUADRATIC_ATTENUATION, 0.0f);
	glEnable(light_num);
}

void Lighting::green_light(int light_num, GLfloat light_pos[])
{
	GLfloat diffuseLight[] = {0.0f, 1.0f, 0.0f, 1.0f};

	glLightfv(light_num, GL_AMBIENT, set_light_ambient());
	glLightfv(light_num, GL_DIFFUSE, diffuseLight);
	glLightfv(light_num, GL_SPECULAR,low_spec);
	glLightfv(light_num, GL_POSITION, light_pos);
	glLightf(light_num, GL_CONSTANT_ATTENUATION, 1.0f);
	glLightf(light_num, GL_LINEAR_ATTENUATION, 0.2f);
	glLightf(light_num, GL_QUADRATIC_ATTENUATION, 0.0f);
	glEnable(light_num);
}

void Lighting::blue_light(int light_num, GLfloat light_pos[])
{
	GLfloat diffuseLight[] = {0.0f, 0.0f, 1.0f, 1.0f};

	glLightfv(light_num, GL_AMBIENT, set_light_ambient());
	glLightfv(light_num, GL_DIFFUSE, diffuseLight);
	glLightfv(light_num, GL_SPECULAR, low_spec);
	glLightfv(light_num, GL_POSITION, light_pos);
	glLightf(light_num, GL_CONSTANT_ATTENUATION, 1.0f);
	glLightf(light_num, GL_LINEAR_ATTENUATION, 0.2f);
	glLightf(light_num, GL_QUADRATIC_ATTENUATION, 0.0f);
	glEnable(light_num);
}

void Lighting::white_light(int light_num, GLfloat light_pos[])
{
	GLfloat diffuseLight[] = {1.0f, 1.0f, 1.0f, 1.0f};

	glLightfv(light_num, GL_AMBIENT, set_light_ambient());
	glLightfv(light_num, GL_DIFFUSE, diffuseLight);
	glLightfv(light_num, GL_SPECULAR, low_spec);
	glLightfv(light_num, GL_POSITION, light_pos);
	glLightf(light_num, GL_CONSTANT_ATTENUATION, 1.0f);
	glLightf(light_num, GL_LINEAR_ATTENUATION, 0.2f);
	glLightf(light_num, GL_QUADRATIC_ATTENUATION, 0.0f);
	glEnable(light_num);
}

void Lighting::rubber()
{
	//set up rubber material
	glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, no_spec); //no_spec always set to 100, 0, 0, 0 for some reason
	glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
}

void Lighting::plastic()
{
	//set up plastic material
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, low_spec);
	glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
}

void Lighting::metal()
{
	//set up metal material
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, high_spec);
	glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
}