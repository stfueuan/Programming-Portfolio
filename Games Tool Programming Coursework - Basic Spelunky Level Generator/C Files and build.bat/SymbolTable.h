/*
	Definition of the symbol table interface functions for GenVal.
	This version is in standard C rather than being object-oriented or using the STL.

	(c) Allan C. Milne, October 2010.
*/

/* returns true/false if there is an entry in the table for the specified id name. */
int isDeclared (char *id);
/* adds a new entry for the specified id name; returns false if already in the table. */
int addId (char *id);
/* sets the v parameter to the value associated in the table with the specified id name; returns false if no table entry exists. */
int getValue (char *id, int *v);
/* sets the value associated with the specified id name to v; returns false if no table entry exists. */
int setValue (char *id, int v);

/* end of symbol table header file. */