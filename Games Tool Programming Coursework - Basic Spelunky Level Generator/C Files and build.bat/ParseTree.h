/* 
	SLG
	ParseTree header for the Spelunky Level Generator language.
	Written by Euan Watt 2014 for Module AG0900A, Games Tool Programming.
	
	Based on ParseTree.h by (c) Allan C. Milne, 2010, 5 October 2012.
*/

#ifndef PARSETREE_H
#define PARSETREE_H

enum NodeType 
{
	NTGenerate,		/* 0 */
	NTLevel,		/* 1 */
	NTSize,			/* 2 */
	NTStart,		/* 3 */
	NTExit,			/* 4 */
	NTPosition, 	/* 5 */
	NTSeed,			/* 6 */
	NTLevelSeed,	/* 7 */
	NTNum, 			/* 8 */
	NTIdentifier, 	/* 9 */
	NTSetList, 		/* 10 */
	NTSet 			/* 11 */
};

/* a node of the parse tree. */
struct treeNode {
	int type;	/* enum NodeType. */
	union data {
		int number;	/* only for numerical nodes. */
		char* strValue; /* for tIDENTIFIER VALUES */
	} value;
	struct treeNode* structure;	/* pointer to child node; start of sibling list representing the structure of this non-terminal. */
	struct treeNode* next;	/* pointer to next sibling element of the production applied for the parent non-terminal. */
};		

typedef struct treeNode parseNode;

#endif

/* end of file */