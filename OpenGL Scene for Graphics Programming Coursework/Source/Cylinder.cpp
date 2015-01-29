#include "Cylinder.h"

#define sides 100.0f //must be an even number to give correct number of triangle strips
#define PI 3.14f

struct c_normals
{
	float x;
	float y;
	float z;
} cylinder_normals;

void Draw_CYLINDER(float height, float radius)
{
	float theta = 2.0f*PI/sides;

	// cylinder is end up, y = 0
	float x1 = radius;
	float y1 = 0;
	float z1 = 0;

	glBegin(GL_TRIANGLE_STRIP);

		for(int i = 0; i < sides + 1; i++) 
		{
			if (i == sides)
			{
				float x1 = radius;
				float y1 = 0;
				float z1 = 0;
			}

			// texture coord
			float texture_coords = i/sides;

			// normals
			float normal = 1.0f/sqrtf((x1*x1) + (z1*z1));
			
			cylinder_normals.x = x1*normal; 
			cylinder_normals.z = z1*normal;

			//draw 2 points of the triangle strip each loop
			glNormal3f(cylinder_normals.x, 0, cylinder_normals.z);
			glTexCoord2f(texture_coords, 0);
				glVertex3f(x1, 0, z1);
			glNormal3f(cylinder_normals.x, 0, cylinder_normals.z);
			glTexCoord2f(texture_coords, 1);
				glVertex3f(x1, height, z1);

			// next position
			float x2 = x1;
			x1 = (cos(theta) * x1) - (sin(theta) * z1);
			z1 = (sin(theta) * x2) + (cos(theta) * z1);
		}

	glEnd();
}

void Draw_CONES(float height, float radius)
{
	float theta = 2.0f*PI/sides;

	// cone is end up, y = 0
	float x1 = radius;
	float y1 = 0;
	float z1 = 0;

	glBegin(GL_TRIANGLE_STRIP);

		for(int i = 0; i < sides + 1; i++) 
		{
			if (i == sides)
			{
				float x1 = radius;
				float y1 = 0;
				float z1 = 0;
			}

			// texture coord
			float texture_coords = i/sides;

			// normals
			float normal = 1.0f/sqrtf((x1*x1) + (z1*z1));
			
			//using cylinder stuff to reduce code
			cylinder_normals.x = x1*normal; 
			cylinder_normals.z = z1*normal;

			//draw 2 points of the triangle strip each loop
			glNormal3f(cylinder_normals.x, 0, cylinder_normals.z);
			glTexCoord2f(texture_coords, 0);
				glVertex3f(x1, 0, z1);
			glNormal3f(cylinder_normals.x, 0, cylinder_normals.z);
			glTexCoord2f(texture_coords, 1);
				glVertex3f(0, height, 0);

			// next position
			float x2 = x1;
			x1 = (cos(theta) * x1) - (sin(theta) * z1);
			z1 = (sin(theta) * x2) + (cos(theta) * z1);
		}

	glEnd();
}