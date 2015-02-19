/* 
	SLG
	ParseTreeBuilder for the Spelunky Level Generator language.
	Written by Euan Watt 2014 for Module AG0900A, Games Tool Programming.
	
	Based on ParseTreeBuilder.c by (c) Allan C. Milne, 2010, 5 October 2012.
*/

#include <stdlib.h>
#include <stdio.h>
#include "ParseTree.h"
#include "ParseTreeBuilder.h"

/* Allocate space for a parse tree node and initialise fields. */
/* This is used at the beginning of every function following this in order to properly allocate memory */
/* Function written by Allan Milne (c) */
parseNode* newNode (int nodeType) 
{
	parseNode* node = (parseNode*)malloc (sizeof(parseNode));
	memset ((void*)node, 0, sizeof (parseNode));
	node->type = nodeType;
	return node;
}

parseNode* newGenerate (parseNode* level, parseNode* seededLevel)
{
	parseNode* node = newNode(NTGenerate);

	if (seededLevel != NULL)
	{
		node->structure = seededLevel;
	}
	else
	{
		node->structure = level;
	}
	return node;
} 

/* if seed is used (i.e. not NULL), then size and start and exit are required to build the level */
parseNode* newLevel (parseNode* size, parseNode* start, parseNode* exit)
{
	parseNode* node = newNode(NTLevel);
	node->structure = size;
	
	if(start != NULL)
	{
		size->next = start;
		start->next = exit;
	}
	
	return node;
} 

parseNode* newSeededLevel (parseNode* seed)
{
	parseNode* node = newNode(NTLevelSeed);
	node->structure = seed;
	return node;
} 

parseNode* newSize (parseNode* sizeX, parseNode* sizeY)
{
	parseNode* node = newNode(NTSize);
	node->structure = sizeX;
	sizeX->next = sizeY;
	return node;
} 

parseNode* newPosition (parseNode* positionX, parseNode* positionY)
{
	parseNode* node = newNode(NTPosition);
	node->structure = positionX;
	positionX->next = positionY;
	return node;
} 

parseNode* newSeed (parseNode* seed)
{
	parseNode* node = newNode(NTSeed);
	node->structure = seed;

	return node;
}

parseNode* newNum (int num)
{
	parseNode* node = newNode(NTNum);
	node->value.number = num;

	return node;
} 

parseNode* newIdentifier(char* ID)
{
	parseNode* node = newNode(NTIdentifier);
	node->value.strValue = ID;

	return node;
} 

parseNode* newSetList(parseNode* list, parseNode* expression)
{
	parseNode* node = newNode(NTSetList);
	if(list != NULL)
	{
		node->structure = list;
		list->next = expression;
	}
	else
	{
		node->structure = expression;
	}

	return node;	
} 

parseNode* newSetValue(char* ID, parseNode* value)
{
	parseNode* node = newNode(NTSet);
	parseNode* idNode = newIdentifier(ID);
	node->structure = idNode;
	idNode->next = value;

	return node;
} 

/* end of file */