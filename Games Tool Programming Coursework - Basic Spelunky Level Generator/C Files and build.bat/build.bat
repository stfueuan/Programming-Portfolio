bison -d slg.y
flex -oslg.yy.c slg.l
cl /Feslg.exe slg.yy.c slg.tab.c symboltable.c parsetreebuilder.c slginterpreter.c slgmain.c
