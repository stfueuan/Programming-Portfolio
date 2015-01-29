#include "ShadowsandMirrors.h"

bool ShadowsandMirrors::CreatePixelFormat(HDC hdc) 
{ 
	PIXELFORMATDESCRIPTOR pfd = {0}; 
	int pixelformat; 

	// Set the size of the structure
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);

	// Always set this to 1
	pfd.nVersion = 1; 

	// Pass in the appropriate OpenGL flags
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | 
	PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;

	// standard mask (this is ignored anyway)
	pfd.dwLayerMask = PFD_MAIN_PLANE;

	// RGB and Alpha pixel type
	pfd.iPixelType = PFD_TYPE_RGBA;

	// Here we use our #define for the color bits
	pfd.cColorBits = COLOUR_DEPTH;

	// Ignored for RBA
	pfd.cDepthBits = COLOUR_DEPTH;
	pfd.cAccumBits = 0; // nothing for accumulation
	pfd.cStencilBits = COLOUR_DEPTH;
	//Gets a best match on the pixel format as passed in from device

	if ( (pixelformat = ChoosePixelFormat(hdc, &pfd)) == false ) 
	{ 
		MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK); 
		return false; 
	} 

	//sets the pixel format if its ok. 
	if (SetPixelFormat(hdc, pixelformat, &pfd) == false) 
	{ 
		MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK); 
		return false; 
	} 

	return true;
}

void ShadowsandMirrors::mirror(ModelClass model, GLuint dlist, Vec3 translation, Vec3 scale, Vec3 rotation, float angle)
{
	glEnable(GL_STENCIL_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glStencilFunc(GL_NEVER, 1, 0xFF);
	glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);

	// draw stencil pattern
	glStencilMask(0xFF);
	glClear(GL_STENCIL_BUFFER_BIT);  // needs mask=0xFF

	quad();

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glStencilMask(0x00);
	// draw where stencil's value is 0
	glStencilFunc(GL_EQUAL, 0, 0xFF);
	// draw only where stencil's value is 1
	glStencilFunc(GL_EQUAL, 1, 0xFF);

	glPushMatrix();
		glScalef(scale[0], scale[1], -scale[2]);
		glTranslatef(translation[0], translation[1], translation[2]);
		glRotatef(angle, rotation[0], rotation[1], rotation[2]);
			glCallList(model.model_dlist);
	glPopMatrix();

	glDisable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glColor4f(0.8f, 0.8f, 1.0f, 0.8f);

	quad();

	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
}

void ShadowsandMirrors::findPlane(GLfloat plane[], GLfloat v0[], GLfloat v1[], GLfloat v2[])
{
    GLfloat vec0[3], vec1[3];

    // Need 2 vectors to find cross product
    vec0[0] = v1[0] - v0[0];
    vec0[1] = v1[1] - v0[1];
    vec0[2] = v1[2] - v0[2];

    vec1[0] = v2[0] - v0[0];
    vec1[1] = v2[1] - v0[1];
    vec1[2] = v2[2] - v0[2];

    // Find cross product to get A, B, and C of plane equation
    plane[0] =   vec0[1] * vec1[2] - vec0[2] * vec1[1];
    plane[1] = -(vec0[0] * vec1[2] - vec0[2] * vec1[0]);
    plane[2] =   vec0[0] * vec1[1] - vec0[1] * vec1[0];
    plane[3] = (plane[0] * v0[0] + plane[1] * v0[1] + plane[2] * v0[2]);
}

void ShadowsandMirrors::buildShadowMatrix(float fMatrix[], float fLightPos[], float fPlane[])
{
    float dotp;

	dotp = fPlane[0] * fLightPos[0] +
		   fPlane[1] * fLightPos[1] +
		   fPlane[2] * fLightPos[2] +
		   fPlane[3] * fLightPos[3];

	fMatrix[0]  = dotp - fLightPos[0] * fPlane[0];
	fMatrix[4]  = -fLightPos[0] * fPlane[1];
	fMatrix[8]  = -fLightPos[0] * fPlane[2];
	fMatrix[12] = -fLightPos[0] * fPlane[3];

	fMatrix[1]  = -fLightPos[1] * fPlane[0];
	fMatrix[5]  = dotp - fLightPos[1] * fPlane[1];
	fMatrix[9]  = -fLightPos[1] * fPlane[2];
	fMatrix[13] = -fLightPos[1] * fPlane[3];

	fMatrix[2]  = -fLightPos[2] * fPlane[0];
	fMatrix[6]  = -fLightPos[2] * fPlane[1];
	fMatrix[10] = dotp - fLightPos[2] * fPlane[2];
	fMatrix[14] = -fLightPos[2] * fPlane[3];

	fMatrix[3]  = -fLightPos[3] * fPlane[0];
	fMatrix[7]  = -fLightPos[3] * fPlane[1];
	fMatrix[11] = -fLightPos[3] * fPlane[2];
	fMatrix[15] = dotp - fLightPos[3] * fPlane[3];
}

void ShadowsandMirrors::Model_Shadow(GLfloat LightPos[], ModelClass model, Vec3 translation, Vec3 scale, Vec3 rotation, float angle, float alpha)
{
	shadow_dlist = glGenLists(1);

	glNewList(shadow_dlist, GL_COMPILE);

		//set up three points on a plane for flattening model
		GLfloat v0[3], v1[3], v2[3];

		v0[0] = -5.0f;
		v0[1] = -0.0f;
		v0[2] = -5.0f;

		v1[0] = -5.0f;
		v1[1] = -0.0f;
		v1[2] = 5.0f;

		v2[0] = 5.0f;
		v2[1] = -0.0f;
		v2[2] = 5.0f;

		findPlane(shadowPlane, v0, v1, v2);
		buildShadowMatrix(shadowMatrix, LightPos, shadowPlane);

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//render shadow here
		glPushMatrix();
			glMultMatrixf((GLfloat*) shadowMatrix);		
			glTranslatef(translation[0], translation[1], translation[2]);
			glScalef(scale[0], scale[1], scale[2]);
			glRotatef(angle, rotation[0], rotation[1], rotation[2]);
			glColor4f(0.0f, 0.0f, 0.0f, alpha);
				glCallList(model.model_dlist);
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glPopMatrix();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING); 
		glEnable(GL_TEXTURE);
		glDisable(GL_BLEND);

	glEndList();
}

void ShadowsandMirrors::Object_Shadow(GLfloat Light_Position1[], GLuint dlist, Vec3 translation, Vec3 scale, Vec3 rotation, float angle, float alpha)
{
	GLfloat v0[3], v1[3], v2[3];

	v0[0] = 0.0f;
	v0[1] = 0.0f;
	v0[2] = 0.0f;

	v1[0] = 0.0f;
	v1[1] = 0.0f;
	v1[2] = 1.0f;

	v2[0] = 1.0f;
	v2[1] = 0.0f;
	v2[2] = 0.0f;

	findPlane(shadowPlane, v0, v1, v2);
	buildShadowMatrix(shadowMatrix, Light_Position1, shadowPlane);
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//render shadow here
	glPushMatrix();
		glMultMatrixf((GLfloat*) shadowMatrix);
		glRotatef(angle, rotation[0], rotation[1], rotation[2]);
		glTranslatef(translation[0], translation[1], translation[2]);
		glScalef(scale[0], scale[1], scale[2]);
		glColor4f(0.0f, 0.0f, 0.0f, alpha);
			glCallList(dlist);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING); 
	glEnable(GL_TEXTURE);
	glDisable(GL_BLEND);
}

void ShadowsandMirrors::quad()
{
	glBegin(GL_QUADS);
		glNormal3f(0.0f, 1.0f, 0.0f);
			glVertex3f(-0.5f, 1.0f, 0.0f); //top left
		glNormal3f(0.0f, 1.0f, 0.0f);
			glVertex3f(0.5f, 1.0f, 0.0f); //top right
		glNormal3f(0.0f, 1.0f, 0.0f);
			glVertex3f(0.5f, 0.0f, 0.0f); //bottom right
		glNormal3f(0.0f, 1.0f, 0.0f);
			glVertex3f(-0.5f, 0.0f, 0.0f); //bottom left
	glEnd();
}