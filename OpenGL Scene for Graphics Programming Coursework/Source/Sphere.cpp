#include "Sphere.h"

#define NO_OF_RINGS 100
#define PI 3.14

void Draw_SPHERE()
{
	glPushMatrix();
		//behind you
		glPushMatrix();
			glRotatef(180, 0, 0, 1);
				Draw_cube_to_SPHERE();
		glPopMatrix();

		//above you
		glPushMatrix();
			glRotatef(-90, 1, 0, 0);
				Draw_cube_to_SPHERE();
		glPopMatrix();

		//below you
		glPushMatrix();
			glRotatef(90, 1, 0, 0);
				Draw_cube_to_SPHERE();
		glPopMatrix();

		//left of you
		glPushMatrix();
			glRotatef(-90, 0, 1, 0);
				glRotatef(180, 0, 0, 1);
					Draw_cube_to_SPHERE();
		glPopMatrix();

		//right of you
		glPushMatrix();
			glRotatef(90, 0, 1, 0);
				glRotatef(180, 0, 0, 1);
					Draw_cube_to_SPHERE();
		glPopMatrix();

		//in front of you
		glPushMatrix();
			glRotatef(180, 0, 1, 0);
				glRotatef(180, 0, 0, 1);
					Draw_cube_to_SPHERE();
		glPopMatrix();

	glPopMatrix();
}

void Draw_cube_to_SPHERE()
{
	for(int i = -10; i < 10; i++)
	{
		for(int j = -10; j < 10; j++)
		{
			for(int k = 10; k <= 10; k++)
			{
				float a = i/10.0f;
				float b = j/10.0f;
				float c = k/10.0f;
	
				//Bend each point of the plane to match the curve of the sphere
				float dx = a * sqrtf(1.0f - ((b*b)/2.0f) - ((c*c)/2.0f) + (((b*b)*(c*c))/3.0f));
				float dy = b * sqrtf(1.0f - ((c*c)/2.0f) - ((a*a)/2.0f) + (((c*c)*(a*a))/3.0f));
				float dz = c * sqrtf(1.0f - ((a*a)/2.0f) - ((b*b)/2.0f) + (((a*a)*(b*b))/3.0f));
	
				float dx2 = (a + 0.1f) * sqrtf(1.0f - ((b*b)/2.0f) - ((c*c)/2.0f) + (((b*b)*(c*c))/3.0f));
				float dy2 = b * sqrtf(1.0f - (c*c/2.0f) - ((a + 0.1f)*(a + 0.1f)/2.0f) + (((c*c)*((a + 0.1f)*(a + 0.1f)))/3.0f));
				float dz2 = c * sqrtf(1.0f - (((a + 0.1f)*(a + 0.1f))/2.0f) - ((b*b)/2.0f) + ((((a + 0.1f)*(a + 0.1f))*(b*b))/3.0f));
	
				float dx3 = (a + 0.1f) * sqrtf(1.0f - (((b + 0.1f)*(b + 0.1f))/2.0f) - ((c*c)/2.0f) + ((((b + 0.1f)*(b + 0.1f))*(c*c))/3.0f));
				float dy3 = (b + 0.1f) * sqrtf(1.0f - ((c*c)/2.0f) - (((a + 0.1f)*(a + 0.1f))/2.0f) + (((c*c)*((a + 0.1f)*(a + 0.1f)))/3.0f));
				float dz3 = c * sqrtf(1.0f - (((a + 0.1f)*(a + 0.1f))/2.0f) - (((b + 0.1f)*(b + 0.1f))/2.0f) + ((((a + 0.1f)*(a + 0.1f))*((b + 0.1f)*(b + 0.1f)))/3.0f));
	
				float dx4 = a * sqrtf(1.0f - (((b + 0.1f)*(b + 0.1f))/2.0f) - ((c*c)/2.0f) + ((((b + 0.1f)*(b + 0.1f))*(c*c))/3.0f));
				float dy4 = (b + 0.1f) * sqrtf(1.0f - ((c*c)/2.0f) - ((a*a)/2.0f) + (((c*c)*(a*a))/3.0f));
				float dz4 = c * sqrtf(1.0f - ((a*a)/2.0f) - (((b + 0.1f)*(b + 0.1f))/2.0f) + (((a*a)*((b + 0.1f)*(b + 0.1f)))/3.0f));
	
				glBegin(GL_QUADS);
					glTexCoord2f((a + 1)/2, (b + 1)/2);
					glNormal3f(dx, dy, dz);
						glVertex3f(dx, dy, dz); //top left
	
					glTexCoord2f((a + 1)/2 + 0.05f, (b + 1)/2);
					glNormal3f(dx2, dy2, dz2);
						glVertex3f(dx2, dy2, dz2); //top right
	
					glTexCoord2f((a + 1)/2 + 0.05f, (b + 1)/2 + 0.05f);
					glNormal3f(dx3, dy3, dz3);
						glVertex3f(dx3, dy3, dz3); //bottom right
	
					glTexCoord2f((a + 1)/2, (b + 1)/2 + 0.05f);
					glNormal3f(dx4, dy4, dz4);
						glVertex3f(dx4, dy4, dz4); //bottom left
				glEnd();
			}
		}
	}
}