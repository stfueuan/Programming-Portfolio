
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


// INCLUDES //
#include <Windows.h>
#include <fstream>
#include <gl/gl.h>
#include <gl/glu.h>
using namespace std;


// MY CLASS INCLUDES //
#include "TokenStream.h"
#include <vector>
#include "Vec3.h"
#include "SOIL.h"

class ModelClass
{
public:
	
	bool Initialise(char*, char*);
	void Render();
	int GetIndexCount();
	GLuint model_dlist;
	
private:
	
	void LoadTexture(char*);
	bool LoadModel(char*);
	int m_vertexCount, m_indexCount;
	GLint texture;

	std::vector<float> vertex, normals, texCoords;		// use these when parsing the loaded model data
};

#endif