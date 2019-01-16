lex.c: lex.l grammar.y.h common.h
	flex --yylineno -o lex.c lex.l

grammar.y.c grammar.y.h : grammar.y
	bison -d -b grammar.y grammar.y
