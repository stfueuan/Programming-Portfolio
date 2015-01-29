#ifndef SHADOWSMIRRORS_H
#define SHADOWSMIRRORS_H

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "ModelClass.h"

#define COLOUR_DEPTH 16 //Colour depth

class ShadowsandMirrors
{
public:
	void mirror(ModelClass model, GLuint dlist, Vec3 translation, Vec3 scale, Vec3 rotation, float angle);
	void Model_Shadow(GLfloat LightPos[], ModelClass model, Vec3 translation, Vec3 scale, Vec3 rotation, float angle, float alpha);
	void Object_Shadow(GLfloat LightPos[], GLuint dlist, Vec3 translation, Vec3 scale, Vec3 rotation, float angle, float alpha);
	bool CreatePixelFormat(HDC hdc);
	GLuint shadow_dlist;

private:
	void findPlane(GLfloat plane[], GLfloat v0[], GLfloat v1[], GLfloat v2[]);
	void buildShadowMatrix(float fMatrix[], float fLightPos[], float fPlane[]);
	void quad();
	
	GLfloat shadowPlane[4];
	GLfloat shadowMatrix[16];
};

#endif 