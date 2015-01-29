#include "Disc.h"

#define NO_OF_SEGMENTS 100

GLuint display_list;

GLuint Draw_DISC()
{
	//initialise variables
	float angle = 0;
	float theta = (2*3.14)/NO_OF_SEGMENTS;
	float x, y, u, v;

	std::vector<float> vertex;
	std::vector<float> normals;
	std::vector<float> texture_coords;

	//start display list
	display_list = glGenLists(1);
	glNewList(display_list, GL_COMPILE);

		for (int disc_vertex = 0; disc_vertex < NO_OF_SEGMENTS + 1; disc_vertex++)
		{
			//to complete the disc, force last segment to end at start position
			if (disc_vertex == (NO_OF_SEGMENTS))
			{
				angle = 0;
			}

			x = cos(angle);
			y = sin(angle);
			u = x/2 + 0.5;
			v = y/2 + 0.5;

			angle += theta;

			//push x, y, 0 to vector
			vertex.push_back(x);
			vertex.push_back(y);

			//push 0, 0, 1 to normals vector
			normals.push_back(0);
			normals.push_back(1);

			//push texture coords to texture vector
			texture_coords.push_back(u);
			texture_coords.push_back(v);
		}

		glBegin(GL_TRIANGLE_FAN);
			glTexCoord2f(0.5, 0.5);
			glNormal3f(0, 0, 1);
			glVertex3f(0, 0, 0);

			for (int render = 0; render < (vertex.size() - 1); render+=2)
			{
				glTexCoord2f(texture_coords[render], texture_coords[render + 1]);
				glNormal3f(0, 0, normals[render + 1]);
				glVertex3f(vertex[render], vertex[render + 1], 0);
			}
		glEnd();

	//end display list
	glEndList();

	return display_list;
}