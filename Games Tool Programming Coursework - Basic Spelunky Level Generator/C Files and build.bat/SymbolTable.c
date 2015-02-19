/*
	The implementation of a symbol table for the GenVal language.

	Notes on this version: 
	- uses standard C rather than object orientation or the STL.
	The - symbol table is implemented as a singly linked list. 
- no optimisations for table searching.
	- all function calls result in a new table search for the required entry.

	(c) Allan c. Milne, October 2010.
*/

#include <stdlib.h>

/* include the interface function prototypes. */
#include "SymbolTable.h"

/* A single entry in the symbol table. */
struct symbolNode {
	char *name;	/* the identifier name. */
	int value;	/* value assigned to it. */
	struct symbolNode *next;	/* pointer to next entry in the list. */
};

typedef struct symbolNode symbolEntry;

/* the Symbol table. */
symbolEntry *symbolTable = NULL;

/* local function to find an entry for the specified id name; returns null if not found. */
symbolEntry *findEntry (char *id) {
	symbolEntry *found = symbolTable;
	while (found!=NULL && strcmp(id,found->name)) {
		found = found->next;
	}
	return found;
}

/* returns true/false if there is an entry in the table for the specified id name. */
int isDeclared (char *id) {
	return (findEntry(id) != NULL);
}

/* adds a new entry for the specified id name; returns false if already in the table. */
int addId (char *id) {
	symbolEntry *newEntry;
	if (isDeclared (id)) return 0;
	newEntry = (symbolEntry*) malloc (sizeof(symbolEntry));
	newEntry->name = id;
	newEntry->value = 0;
	newEntry->next = symbolTable;
	symbolTable = newEntry;
	return 1;
}

/* sets the v parameter to the value associated in the table with the specified id name; returns false if no table entry exists. */
int getValue (char *id, int *v) {
	symbolEntry *entry = findEntry (id);
	if (entry == NULL) return 0;
	*v = entry->value;
	return 1;
}

/* sets the value associated with the specified id name to v; returns false if no table entry exists. */
int setValue (char *id, int v) {
	symbolEntry *entry = findEntry (id);
	if (entry == NULL) return 0;
	entry->value = v;
	return 1;
}

/* end of symbol table implementation. */