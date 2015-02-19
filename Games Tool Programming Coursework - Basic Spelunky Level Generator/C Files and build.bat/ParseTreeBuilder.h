/*
	SLG
	ParseTreeBuilder header for the Spelunky Level Generator language. 
	Written by Euan Watt 2014 for Module AG0900A, Games Tool Programming.
	
	Based on ParseTreeBuilder.h by (c) Allan C. Milne, 2010, 5 October 2012.
*/

#ifndef PARSETREEBUILDER_H
#define PARSETREEBUILDER_H

#include "ParseTree.h"

/* Allocate space for a parse tree node and initialise fields */
parseNode* newNode (int nodeType);

/* build a branch for the generate statement from level size, start position and exit position */
parseNode* newGenerate (parseNode* level, parseNode* seededLevel);

/* build a branch for the level detailed by the user */
parseNode* newLevel (parseNode* size, parseNode* start, parseNode* exit);

/* build a branch for the given level seed */
parseNode* newSeededLevel (parseNode* seed);

/* build a branch for the supplied size of the level */
parseNode* newSize (parseNode* sizeX, parseNode* sizeY);

/* build a branch for a new position */
parseNode* newPosition (parseNode* positionX, parseNode* positionY);

/* build a branch for the supplied seed */
parseNode* newSeed (parseNode* seed);

/* create a node for an integer detailed by the user */
parseNode* newNum (int num);

/* create a node for a new identifier */
parseNode* newIdentifier (char* ID);

/* create a node for a new declaration list */
parseNode* newSetList(parseNode* list, parseNode* value);

/* create a node for a new value setter */
parseNode* newSetValue(char* ID, parseNode* value);

#endif

/* end of file */