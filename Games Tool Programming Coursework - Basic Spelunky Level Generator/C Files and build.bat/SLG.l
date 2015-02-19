/*
	SLG
	Lex specification for the Spelunky Level Generator language. 
	Written by Euan Watt 2014 for Module AG0900A, Games Tool Programming.
	
	Based on the GenVal.l for the GenVal language by (c) Allan C. Milne; June 2014; 2012; 2010.
*/

%{
#include <stdlib.h> 
#include <stdio.h>
#include "SLG.tab.h"

void yyerror(char const*);
%}

%%

generate {return tGENERATE;}
level {return tLEVEL;}
of {return tOF;}
size {return tSIZE;}
with {return tWITH;}
start {return tSTART;}
at {return tAT;}
and {return tAND;}
exit {return tEXIT;}
using {return tUSING;}
seed {return tSEED;}
variable {return tVARIABLE;}

[(),x=+*-/^] { return *yytext; } /* single character punctuation */
[ \t] {} /* ignores white space */
[\r\n] {} /* ignores new lines */
"/*"((.|\n)*)"*/" {} /* ignore commented lines */
[0-9]+ {
	sscanf(yytext, "%i", &(yylval.number));
	return tNUM;
	} /* integers */
	/* An identifier. Taken from Allan Milne's GenVal.l. All rights reserved. */
[a-zA-Z][a-zA-Z0-9]*	{ 
	yylval.string = (char*)malloc (yyleng+1);
	strcpy (yylval.string, yytext);
	return tIDENTIFIER; 
	}
. { yyerror("invalid character"); }

%%

int yywrap (void) { return 1; }

/* end of file */
