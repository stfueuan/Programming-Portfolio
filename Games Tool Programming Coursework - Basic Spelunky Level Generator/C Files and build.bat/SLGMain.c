/*
	Heavily based on Alan Milne's MainApp.c from the GenVal compiler.
	
	Minor edits by Euan Watt for the Speluky Level Generator.
	
	(c) Allan C. Milne, 2010, 9 October 2012.
	
	Following comments taken from same file with small edits. Any files in the list below with the SLG prefix have at least been edited with the exception of the ParseTree files, otherwise are as is.
	
	Usage: SLG <script-file> {<dataset-file>}
	if <dataset-file> is not supplied then stdout is used.
	
	This compiler is architectured into separate files reflecting the functional components of a compiler.
	Compiler components:
	SLGMain.c : command-line application entry point; handles command-line arguments; sets up input/output files and calls yyparse().
	SLG.y : Yacc parser; maintains symbol table; creates a parse tree representing the input script; interprets parse tree.
	SLG.l : Lex scanner; identifies input tokens; returns token types with any associated semantic values.
	SymbolTable.c/.h : exposes functions for adding and retrieving symbol table entries.
	ParseTree.h : defines data structure for  a node of the parse tree.
	ParseTreeBuilder.c/.h : exposes functions for building different parse tree branches.
	SLGInterpreter.c/.h : exposes interpret() function and internal functions to interpret different parse tree branches.
*/

#include <stdlib.h>
#include <stdio.h>

/* number of errors detected - defined in SLG.y */
extern int errorCount;

/* source of input script - defined in Lex. */
extern FILE *yyin;

 /* output file for generated dataset. */
FILE *SLFile;

/* internal function prototypes. */
int prologue (int, char**);		/* return 0 if invalid usage. */
void epilogue ();
int openFiles (int, char**);		/* return 0 if error opening files. */
void closeFiles (int);

/* the Yacc parser prototype. */
int yyparse ();

/*--- the SLG compiler entry point. ---*/
int main (int argc, char *argv[]) {
	if (!prologue (argc, argv)) { exit (-1); }
	if (!openFiles (argc, argv)) { exit (-2); }

	fprintf(stderr, "Parsing file..\n");
	yyparse(); 	/* call Yacc parser and subsequent interpreter. */
	fprintf(stderr, "File parsed correctly.\n");
	
	closeFiles (argc);
	epilogue();
	return 0; 
}

/* Display prologue information and check correct usage. */
int prologue (int argc, char *argv[]) {
	fprintf (stderr, "Spelunky Level Generator (SLG). \n");
	fprintf (stderr, "Written by Euan Watt for Module AG0900A, Games Tool Programming. \n\n");
	if (argc<2 || argc>3) {
		fprintf (stderr, "invalid usage: SLG <script-file> {<dataset-file>}\n");
		return 0;
	} else {
		fprintf (stderr, "Compiling SLG script '%s'. \n", argv[1]);
		fprintf (stderr, "Dataset values output to ");
		if (argc == 3)
			fprintf (stderr, "file '%s'. \n\n", argv[2]);
		else	fprintf (stderr, "standard output. \n\n");
		return 1;
	}
}

/* output closing information. */
void epilogue () {
	if (errorCount == 0)
		fprintf (stderr, "... dataset successfully created. \n");
	else	fprintf (stderr, "\n%d errors found; no dataset created. \n", errorCount);
}

/* open and close files depending on command-line arguments. */
/* Return 0 (false) if any error found opening the files. */
int openFiles (int argc, char *argv[]) {
	fprintf(stderr, "Opening files.. \n");
	if (fopen_s (&yyin, argv[1], "r")) {
		fprintf (stderr, "Error opening SLG script '%s'.", argv[1]);
		return 0;
	}
	if (argc > 2) {
		if (fopen_s (&SLFile, argv[2], "w")) {
			fprintf (stderr, "Error opening file '%s' for generating the desired level.", argv[2]);
			return 0;
		}
	} else	{
		SLFile = stdout;
	}
	fprintf(stderr, "Files opened successfully.\n");
	return 1;
}

void closeFiles (int argc) {
	fclose (yyin);
	if (argc > 2) fclose (SLFile);
}

/* end of main program code file. */
