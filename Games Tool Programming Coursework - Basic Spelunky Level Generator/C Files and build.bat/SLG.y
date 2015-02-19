/*
	SLG
	Yacc definition for the Spelunky Level Generator language. 
	Written by Euan Watt 2014 for Module AG0900A, Games Tool Programming.
	
	Based on the GenVal.y for the GenVal language by (c) Allan C. Milne; June 2014; 2012; 2010.
*/ 

%{
	#include <stdio.h>
	#include <ctype.h> 
	#include <math.h>
	#include "SymbolTable.h"
	#include "ParseTree.h"
	#include "ParseTreeBuilder.h"
	#include "SLGInterpreter.h"

	int errorCount = 0; /* count the number of errors detected. */
	
	int yylex();
	void yyerror(char const*);
	
	void semanticError (int, char*); /* generate semantic error messages. Function written by Allan Milne */
%}

%union {
	int number;
	char* string;
	struct treeNode* parseTree;
	}

%type <parseTree> generateLevel
%type <parseTree> level
%type <parseTree> size
%type <parseTree> position
%type <parseTree> seed
%type <parseTree> seededLevel
%type <parseTree> setList
%type <parseTree> set

/* non-terminals with associated type of semantic value. */
%type <parseTree> expression

%token <number> tNUM
%token <string> tIDENTIFIER
%token tGENERATE tLEVEL tOF tSIZE tWITH tSTART tAT tAND tEXIT tUSING tSEED tVARIABLE

/* Following taken from GenVal.y, (c) Allan C. Milne; June 2014; 2012; 2010. */
/* The operators; explicitly setting their associativity. */
/* The order of these declarations also determines precedence; from lowest upwards in order. */
%left '-' '+'
%left '*' '/'

%%

levelGeneratorScript :
	declarations setList generateLevel { if (errorCount == 0) interpret($2, $3); }
	;
	
generateLevel :
	tGENERATE tLEVEL level { $$ = newGenerate($3, NULL); }
	| tGENERATE tLEVEL seededLevel { $$ = newGenerate(NULL, $3); }
	;

level :
	tOF tSIZE size {
		$$ = newLevel($3, NULL, NULL);
	}
	| tOF tSIZE size tWITH tSTART tAT position tAND tEXIT tAT position
	{
		$$ = newLevel($3, $7, $11);
	}
	;
	
size :
	expression 'x' expression {
	/* if $1 or $3 is negative, error */
		$$ = newSize($1, $3);
	}
	;
	
position :
	'(' expression ',' expression ')' {
	/* standard format of coordinate */
		$$ = newPosition($2, $4);
	}
	;

expression :
	tNUM {
		$$ = newNum($1);
	}
	| tIDENTIFIER {
		if(!isDeclared($1))
		{
			semanticError(2, $1);
		}
		$$ = newIdentifier($1);
	}
	| expression '+' expression {
		float temp = evaluate($1) + evaluate($3); 
		$$ = newNum(temp);
	}
	| expression '-' expression {
		float temp = evaluate($1) - evaluate($3);  
		$$ = newNum(temp);
	}
	| expression '*' expression {
		float temp = evaluate($1) * evaluate($3); 
		$$ = newNum(temp);
	}
	| expression '/' expression {
		float temp = evaluate($1) / evaluate($3); 
		$$ = newNum(temp);
	}
	| expression '^' expression { 
		/* first expression to the power of the second expression */
		float temp = pow (evaluate($1), evaluate($3));
		$$ = newNum(temp);
	}
	;
	
seededLevel :
	tUSING tSEED seed {
		$$ = newSeededLevel($3);
	}
	;

seed :
	expression { 
		$$ = newSeed($1);
	}
	;

declarations :
	declarations declaration
	| /* user can choose not to have pre-defined values */
	;

declaration :
	tVARIABLE variableList
	;

variableList :
	tIDENTIFIER {
	if (!addId ($1))
		semanticError (1, $1);
	}
	| variableList ',' tIDENTIFIER {
	if (!addId ($3))
		semanticError (1, $3);
	}
	;

setList :
	set { $$ = newSetList(NULL, $1); }
	| setList ',' set {
		$$ = newSetList($1, $3);
	}
	|
	;

set :
	tIDENTIFIER '=' expression {
		$$ = newSetValue($1, $3);
	}
	;

%%

void yyerror (char const *msg) { 
	/* this version prints on the standard error output stream. */
	fprintf (stderr, "%s\n", msg);
	errorCount++;
}

/* generate semantic error messages. Function written by Allan Milne (c). */
void semanticError (int errType, char *info) {
	char errmsg[200];
	switch (errType) {
	case 1: sprintf_s (errmsg, 200, "Semantic error 1: variable '%s' is already declared.", info); break;
	case 2: sprintf_s (errmsg, 200, "Semantic error 2: variable '%s' is not declared.", info); break;
	}
	yyerror (errmsg);
}
	
/* end of file */
