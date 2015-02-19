/*
	SLG
	Language interpreter for the Spelunky Level Generator language.
	Written by Euan Watt 2014 for Module AG0900A, Games Tool Programming.
	
	Loosely based on Interpreter.c by (c) Allan C. Milne, 2010, 5 October 2012.
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <malloc.h>
#include "ParseTree.h"
#include "SymbolTable.h"
#include "SLGInterpreter.h"

extern FILE *SLFile;
extern int errorCount;

void interpretGenerate(parseNode*);
void interpretLevel(parseNode*);
void interpretSeededLevel(parseNode*);
int* interpretSize(parseNode*);
int* interpretPosition(parseNode*);
void interpretSeed(parseNode*);
void interpretSetList(parseNode*);
void interpretSetValue(parseNode*);
int evaluate(parseNode*);
void interpretError(int[2], int[2], int[2]);

void GenerateLevel(int[2], int[2], int[2]);
void GenerateSeededLevel(int);
void GenerateRandomLevel(int[2]);
void CreateLevel(int);
int CheckEdge(int**);
void ChooseDirection(int**);
void CreateLevelArray(int, int);
void DestroyLevelArray();

int LEVELSIZE;
int LEVELSIZEY;
int LEVELSIZEX;
int** level;

enum TakeDirection
{
	left = -1,
	right = 1,
	down = 1
};

struct CurrentPos
{
	int x;
	int y;
};

enum RoomType
{
	noExits,
	leftRightExits,
	leftRightDownExits,
	leftRightUpExits,
	startRoom,
	endRoom
};

struct CurrentPos currentPos;
struct CurrentPos* pCurrentPos = &currentPos;

int haveWeMoved = 0;

/* declared in header so it can be called elsewhere */
void interpret(parseNode* declaration, parseNode* generate)
{
	parseNode* declarations;
	int i;
	srand(time(NULL)); /* seed the randomiser to give unique seed */
	
	if(declaration != NULL)
	{
		/* interpret any declared variables */
		declarations = declaration;
		interpretSetList(declarations);
		
		if(generate != NULL)
		{
			/* interpret generate rule and output artefact */
			interpretGenerate(generate);
		}
	}
	else if(generate != NULL)
	{
		interpretGenerate(generate);
	}
} 

void interpretGenerate(parseNode* generate)
{
	parseNode* node = generate->structure;

	if(node->type == NTLevel)
	{
		interpretLevel(node);
	}
	else if(node->type == NTLevelSeed)
	{
		interpretSeededLevel(node);
	}
} 

void interpretLevel(parseNode* level)
{
	parseNode* node;
	int* size;
	int* start; 
	int* exit;
	node = level;
	size = interpretSize(node);
	
	node = level->structure;
	
	if(node->next != NULL)
	{
		/* get user defined start position */
		start = interpretPosition(node);
		node = level->structure->next;
		
		if(node->next != NULL)
		{
			/* get user defined exit position */
			exit = interpretPosition(node);
		}
		else
		{
			/* stop tool as exit position does not exist */
			errorCount++;
		}
		
		/* check user variables are correct */
		interpretError(size, start, exit);
		
		if(errorCount == 0)
		{
			/* generate artefact if no errors have been found */
			GenerateLevel(size, start, exit);
		}
	}
	else
	{
		GenerateRandomLevel(size);
	}
} 

void interpretSeededLevel(parseNode* seededLevel)
{
	parseNode* node = seededLevel->structure;

	interpretSeed(node);
} 

int* interpretSize(parseNode* size)
{
	parseNode* node = size->structure;
	static int dimensions[2];
	
	node = node->structure;
	dimensions[0] = evaluate(node);
	dimensions[1] = evaluate(node->next);
	
	return dimensions;
} 

int* interpretPosition(parseNode* position)
{
	parseNode* node = position;
	int* pos = (int*)malloc (sizeof(int)*2);

	node = node->next;
	node = node->structure;
	pos[0] = evaluate(node);
	pos[1] = evaluate(node->next);
	
	return pos;
} 

void interpretSeed(parseNode* seed)
{	
	parseNode* node = seed->structure;
	int seedValue;
	
	seedValue = evaluate(node);
	
	/* user defined seed must be six digits */
	if(seedValue < 100000 || seedValue > 999999)
	{
		errorCount++;
	}

	if(errorCount == 0)
	{
		GenerateSeededLevel(seedValue);
	}
} 

void interpretSetList(parseNode* list)
{
	parseNode* node = list->structure;
	int i;

	if(node->type == NTSet)
	{
		interpretSetValue(node);
	}
	else if(node->type == NTSetList)
	{
		interpretSetList(node);
		interpretSetValue(node->next);
	}
} 

void interpretSetValue(parseNode* set)
{
	parseNode* node;
	int value;
	node = set;
	value = evaluate(node->structure->next);
	node = node->structure;
	
	if(node->value.strValue != NULL)
	{
		setValue(node->value.strValue, value);
	}
} 

int evaluate(parseNode* expression)
{
	parseNode* node = expression;
	int result;

	switch(node->type)
	{
	case NTNum:
		result = node->value.number;
		break;
	
	case NTIdentifier:
		getValue(node->value.strValue, &result);
		break;
	}
	
	return result;
} 

void GenerateLevel(int size[2], int start[2], int exit[2])
{
	struct CurrentPos* pCurrentPos = &currentPos;
	int i, seed;
	seed = rand() % 8999 + 1000;

	LEVELSIZEX = size[0];
	LEVELSIZEY = size[1];

	for(i = 0; i < 2; i++)
	{
		seed = (seed * 10) + size[i]; /* add the size of the level to the end of the seed, this allows the code to have accurate information when the seed is reused */
	}

	srand(seed); /* allow for the user to repeat a seed using the outputted number */

	/* create 2D array of desired size */
	CreateLevelArray(LEVELSIZEX, LEVELSIZEY);

	/* select a start point, must be on top row */
	currentPos.x = start[0];

	currentPos.y = 0;
	level[pCurrentPos->y][pCurrentPos->x] = startRoom; /* set specified start position as startRoom */

	level[exit[1]][exit[0]] = endRoom; /* set specified end position as endRoom */

	CreateLevel(seed);
} 

void GenerateSeededLevel(int seed)
{
	struct CurrentPos* pCurrentPos = &currentPos;

	srand(seed); /* allow for the user to repeat a seed using the outputted number */

	LEVELSIZE = seed % 1000;
	LEVELSIZEX = (LEVELSIZE % 100) / 10; /* get the second last digit of the seed for the width of the level */
	LEVELSIZEY = (LEVELSIZE % 100) % 10; /* get the digit of the seed for the height of the level */

	/* create 2D array of desired size */
	CreateLevelArray(LEVELSIZEX, LEVELSIZEY);

	/* select a start point, must be on top row */
	currentPos.x = rand() % LEVELSIZEX; 
	currentPos.y = 0;
	level[pCurrentPos->y][pCurrentPos->x] = startRoom;

	CreateLevel(seed);
} 

void GenerateRandomLevel(int size[2])
{
	struct CurrentPos* pCurrentPos = &currentPos;
	int i, seed;
	
	seed = rand() % 8999 + 1000;
	
	for(i = 0; i < 2; i++)
	{
		seed = (seed * 10) + size[i]; /* add the size of the level to the end of the seed, this allows the code to have accurate information when the seed is reused */
	}

	srand(seed); /* allow for the user to repeat a seed using the outputted number */

	LEVELSIZEX = size[0];
	LEVELSIZEY = size[1];

	CreateLevelArray(LEVELSIZEX, LEVELSIZEY);

	/* select a start point, must be on top row */
	currentPos.x = rand() % LEVELSIZEX; 
	currentPos.y = 0;
	level[pCurrentPos->y][pCurrentPos->x] = startRoom;

	CreateLevel(seed);
} 

void CreateLevelArray(int sizeX, int sizeY)
{
	/* create 2D array for specified size of level */
	int i;
	level = (int**)calloc(sizeY, sizeof(int*)); /* C is not C++, must allocate pointer for first dimension */

	for (i = 0; i < sizeY; i++)
	{
		level[i] = (int*)calloc(sizeX, sizeof(int)); /* now make each element a new array, you now have a 2D array */
	}
} 

void CreateLevel(int seed)
{
	int i, j;
	
	ChooseDirection(level);

	level[pCurrentPos->y][pCurrentPos->x] = leftRightExits; //first room after exit must be of type 1

	haveWeMoved = 1; 

	while (level[pCurrentPos->y][pCurrentPos->x] != endRoom)
	{
		ChooseDirection(level);
	}

	fprintf(SLFile, "Level Seed: %i\n", seed);
	
	for ( i = 0; i < LEVELSIZEY; i++)
	{
		for ( j = 0; j < LEVELSIZEX; j++)
		{
			fprintf (SLFile, "%i\t", level[i][j]); 
		}

		fprintf(SLFile, "\n"); 
	}
	
	DestroyLevelArray();
} 

void DestroyLevelArray()
{
	free(level);
} 

void ChooseDirection(int** grid)
{
	struct CurrentPos* pCurrentPos = &currentPos;
	int direction, i, j;
	int endRoomExists = 0;
	
	if (haveWeMoved == 0) /* have you moved yet? */
	{
		if (!CheckEdge(level))
		{
			 direction = rand() % 2;

			if (direction == 0)
			{
				pCurrentPos->x = pCurrentPos->x + left;
			}
			else if (direction == 1)
			{
				pCurrentPos->x = pCurrentPos->x + right;
			}
		}
	}
	else
	{
		if (!CheckEdge(level))
		{
			 direction = rand() % 3;

			if (direction == 0)
			{
				currentPos.x = currentPos.x + left;

				if (grid[pCurrentPos->y][pCurrentPos->x] != startRoom)
				{
					grid[pCurrentPos->y][pCurrentPos->x] = 1 + rand() % 3; /* choose room type */
				}
			}
			else if (direction == 1)
			{
				currentPos.x = currentPos.x + right;

				if (grid[pCurrentPos->y][pCurrentPos->x] != startRoom)
				{
					grid[pCurrentPos->y][pCurrentPos->x] = 1 + rand() % 3; /* choose room type */
				}
			}
			else if (direction == 2)
			{
				if (grid[pCurrentPos->y][pCurrentPos->x] != startRoom)
				{
					grid[pCurrentPos->y][pCurrentPos->x] = leftRightDownExits; /* if you are moving down to the next level, room must have guaranteed down exit */
				} 

				if (currentPos.y != LEVELSIZEY - 1)
				{
					currentPos.y = currentPos.y + down;

					if (grid[pCurrentPos->y][pCurrentPos->x] != startRoom)
					{
						grid[pCurrentPos->y][pCurrentPos->x] = 1 + rand() % 3; /* choose room type */
					}
				}
			}
		}
		else
		{
			if (grid[pCurrentPos->y][pCurrentPos->x] != startRoom)
			{
				grid[pCurrentPos->y][pCurrentPos->x] = 1 + rand() % 3; /* choose room type */
			}
		}
		
		if (currentPos.y == LEVELSIZEY - 1)
		{
			for(i = 0; i < LEVELSIZEX; i++)
			{
				if(grid[pCurrentPos->y][i] == endRoom)
				{
					endRoomExists = 1;
					break;
				}
			}
			
			if(endRoomExists == 1)
			{
				if(grid[pCurrentPos->y][i] != grid[pCurrentPos->y][pCurrentPos->x])
				{
					if((pCurrentPos->x - i) > 0)
					{
						for(j = i; j < pCurrentPos->x; j++)
						{
							grid[pCurrentPos->y][j] = leftRightUpExits;
						}
					}
					else
					{
						for(j = pCurrentPos->x + 1; j < i; j++)
						{
							grid[pCurrentPos->y][j] = leftRightUpExits;
						}
					}
					
					pCurrentPos->x = i;
					grid[pCurrentPos->y][pCurrentPos->x] = endRoom;
				}
				else
				{
					grid[pCurrentPos->y][pCurrentPos->x] = endRoom;
				}
			}
			else
			{
				grid[pCurrentPos->y][pCurrentPos->x] = endRoom; /* if you have reached the bottom floor, you have reached the end of the level */
			}
		}
	}
} 

int CheckEdge(int** grid)
{
	struct CurrentPos* pCurrentPos = &currentPos;
	
	if (haveWeMoved == 0)
	{
		if (currentPos.x == 0) /* check if you're at the edge of the level */
		{
			currentPos.x = currentPos.x + right;

			return 1;
		}
		else if (currentPos.x == LEVELSIZEX - 1)
		{
			currentPos.x = currentPos.x + left;

			return 1;
		}
	}
	else
	{
		if (currentPos.x == 0 || currentPos.x == LEVELSIZEX - 1)
		{
			if (currentPos.y != LEVELSIZEY - 1)
			{
				if (grid[pCurrentPos->y][pCurrentPos->x] != startRoom)
				{
					grid[pCurrentPos->y][pCurrentPos->x] = leftRightDownExits; /* set current room to have a guaranteed down exit before moving to new position */
				}

				currentPos.y = currentPos.y + down;
			}
			else
			{
				grid[pCurrentPos->y][pCurrentPos->x] = endRoom;
			}

			return 1;
		}
	}

	return 0;
} 

void interpretError(int size[2], int start[2], int exit[2])
{
	/* checks input size */
	if(size[0] <= 0 || size[1] <= 0)
	{
		yyerror("\nLevel size cannot be zero or negative.\n\n");
		errorCount++;
	}
	
	/* checks start x position is within width of level */
	if(start[0] < 0 || start[0] > (size[0] - 1))
	{
		yyerror("\nStart position must be within level size.\n\n");
		errorCount++;
	}
	
	/* checks start y position is on top row */
	if(start[1] != 0)
	{
		yyerror("\nStart position must be on top row.\n\n");
		errorCount++;
	}
	
	/* checks exit x position is within width of level */
	if(exit[0] < 0 || exit[0] > (size[0] - 1))
	{
		yyerror("\nExit position must be within level size.\n\n");
		errorCount++;
	}
	
	/* checks exit y position is on bottom row */
	if(exit[1] != (size[1] - 1))
	{
		yyerror("\nExit position must be on bottom row.\n\n");
		errorCount++;
	}
} 

/* end of file */
