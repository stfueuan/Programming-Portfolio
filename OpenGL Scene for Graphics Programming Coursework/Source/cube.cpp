#include "cube.h"

GLubyte CUBE_INDICES[]={5,   0,   9,	//	//front
						10,   1,   13, //
						18,   22,   32,	//	//back
						33,   23,   27,//
						2,   19,   14,	//	//left
						15,   20,   34,	//	
						24,   6,   28,	//	//right
						29,   7,   11, //
						35,   30,   16,	//	//up
						17,   31,   12, //
						8,   3,   25,	//	//down
						26,   4,   21,};//

float CUBE_VERTS[] = {	-0.5, -0.5, -0.5,			// Vertex #0 FRONT V0 //
						-0.5, -0.5, -0.5,			// Vertex #1 FRONT //
						-0.5, -0.5, -0.5,			// Vertex #2 LEFT //
						-0.5, -0.5, -0.5,			// Vertex #3 DOWN //
						-0.5, -0.5, -0.5,			// Vertex #4 DOWN NEW //
						 0.5, -0.5, -0.5,			// Vertex #5 FRONT V1 //
						 0.5, -0.5, -0.5,			// Vertex #6 RIGHT //
						 0.5, -0.5, -0.5,			// Vertex #7 RIGHT //
						 0.5, -0.5, -0.5,			// Vertex #8 DOWN //
						 0.5,  0.5, -0.5,			// Vertex #9 FRONT V2 //
						 0.5,  0.5, -0.5,			// Vertex #10 FRONT //
						 0.5,  0.5, -0.5,			// Vertex #11 RIGHT //
						 0.5,  0.5, -0.5,			// Vertex #12 UP //
						-0.5,  0.5, -0.5,			// Vertex #13 FRONT V3 //
						-0.5,  0.5, -0.5,			// Vertex #14 LEFT //
						-0.5,  0.5, -0.5,			// Vertex #15 LEFT //
						-0.5,  0.5, -0.5,			// Vertex #16 UP //
						-0.5,  0.5, -0.5,			// Vertex #17 UP NEW //
						-0.5, -0.5,  0.5,			// Vertex #18 BACK V4 //
						-0.5, -0.5,  0.5,			// Vertex #19 LEFT //
						-0.5, -0.5,  0.5,			// Vertex #20 LEFT //
						-0.5, -0.5,  0.5,			// Vertex #21 DOWN //
						 0.5, -0.5,  0.5,			// Vertex #22 BACK V5 //
						 0.5, -0.5,  0.5,			// Vertex #23 BACK //
						 0.5, -0.5,  0.5,			// Vertex #24 RIGHT //
						 0.5, -0.5,  0.5,			// Vertex #25 DOWN //
						 0.5, -0.5,  0.5,			// Vertex #26 DOWN NEW //
						 0.5,  0.5,  0.5,			// Vertex #27 BACK V6 //
						 0.5,  0.5,  0.5,			// Vertex #28 RIGHT //
						 0.5,  0.5,  0.5,			// Vertex #29 UP //
						 0.5,  0.5,  0.5,			// Vertex #30 UP //
						 0.5,  0.5,  0.5,			// Vertex #31 UP NEW //
						-0.5,  0.5,  0.5,			// Vertex #32 BACK V7 //
						-0.5,  0.5,  0.5,			// Vertex #33 BACK //
						-0.5,  0.5,  0.5,			// Vertex #34 LEFT //
						-0.5,  0.5,  0.5};			// Vertex #35 UP //

float CUBE_NORMALS[] = { 0, 0, 1,			//0
						 0, 0, 1,			//1
						 -1, 0, 0,			//2
						 0, -1, 0,			//3
						 0, -1, 0,			//4
						  0, 0, 1,			//5
						  1, 0, 0,			//6
						  1, 0, 0,			//7
						  0, -1, 0,			//8
 						  0,  0, 1,			//9
						  0,  0, 1,			//10
						  1,  0, 0,			//11
						  0,  1, 0,			//12
						  0,  0, 1,			//13
						 -1,  0, 0,			//14
						 -1,  0, 0,			//15
						 0,  1, 0,			//16
						 0,  1, 0,			//17
						 0, 0, -1,			//18
						 -1, 0, 0,			//19
						 -1, 0, 0,			//20
						 0, -1, 0,			//21
						  0, 0, -1,			//22
						  0, 0, -1,			//23
						  1, 0, 0,			//24
						  0, -1, 0,			//25
						  0, -1, 0,			//26
						  0,  0, -1,		//27
						  1,  0, 0,			//28
						  0,  1, 0,			//29
						  0,  1, 0,			//30
						  0,  1, 0,			//31
						  0,  0, -1,		//32	
						  0,  0, -1,   		//33
						 -1,  0, 0,			//34
						  0,  1, 0};		//35


float CUBE_T_COORDS[]= {1.0, 1.0, 			//0 front BOTTOM RIGHT
						1.0, 1.0, 			//1 front BOTTOM RIGHT
						0.0, 1.0, 			//2 left BOTTOM LEFT //
						1.0, 1.0, 			//3 down BOTTOM RIGHT //
						1.0, 1.0, 			//4 down BOTTOM RIGHT // 
						0.0, 1.0, 			//5 front BOTTOM LEFT
						1.0, 1.0, 			//6 right BOTTOM RIGHT //
						1.0, 1.0, 			//7 right BOTTOM RIGHT //
						0.0, 1.0, 			//8 down BOTTOM LEFT //
 						0.0, 0.0, 			//9 front TOP LEFT
						0.0, 0.0, 			//10 front TOP LEFT 
						1.0, 0.0, 			//11 right TOP RIGHT //
						1.0, 0.0, 			//12 up TOP RIGHT //
						1.0, 0.0, 			//13 front TOP RIGHT
						0.0, 0.0, 			//14 left TOP LEFT //
						0.0, 0.0, 			//15 left TOP LEFT //
						0.0, 0.0, 			//16 up TOP LEFT //
						0.0, 0.0, 			//17 up TOP LEFT //
						0.0, 1.0, 			//18 back BOTTOM LEFT //
						1.0, 1.0, 			//19 left BOTTOM RIGHT //
						1.0, 1.0, 			//20 left BOTTOM RIGHT //
						1.0, 0.0, 			//21 down TOP RIGHT //
						1.0, 1.0,			//22 back BOTTOM RIGHT //
						1.0, 1.0,			//23 back BOTTOM RIGHT //
						0.0, 1.0,			//24 right BOTTOM LEFT //
						0.0, 0.0,			//25 down TOP LEFT //
						0.0, 0.0,			//26 down TOP LEFT //
						1.0, 0.0,			//27 back TOP RIGHT //
						0.0, 0.0,			//28 right TOP LEFT // 
						0.0, 0.0,			//29 right TOP LEFT //
						1.0, 1.0,			//30 up BOTTOM RIGHT  //
						1.0, 1.0,			//31 up BOTTOM RIGHT  //
						0.0, 0.0,			//32 back TOP LEFT //
						0.0, 0.0,			//33 back TOP LEFT //
						1.0, 0.0,			//34 left TOP RIGHT //
						0.0, 1.0};			//35 up BOTTOM LEFT //



void Draw_CUBE(void)
{	
	//Enable arrays
	glEnableClientState(GL_INDEX_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//Initialise arrays
	glIndexPointer	 (GL_INT,	0, CUBE_INDICES);
	glVertexPointer	 (3, GL_FLOAT, 0, CUBE_VERTS);
	glNormalPointer	 (GL_FLOAT, 0, CUBE_NORMALS);
	glTexCoordPointer(2, GL_FLOAT, 0, CUBE_T_COORDS);

	//Draw cube with arrays
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, CUBE_INDICES);

	//Disable arrays
	glDisableClientState(GL_INDEX_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


