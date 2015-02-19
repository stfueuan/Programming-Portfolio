/*
	The top-level ("public") interpretation/generation function for the GenVal compiler.
	(c) Allan C. Milne, 2010, 4 October 2012.
*/

#ifndef SLGINTERPRETER_H
#define SLGINTERPRETER_H

/* The node struct of the parse tree. */
#include "ParseTree.h"

void interpret (parseNode* declaration, parseNode* sequence);

#endif

/* end of file */