%x IN_COMMENT
%{
#include "../src/common.h"
#include "grammar.tab.h"
extern YYSTYPE yylval;
extern int yylineno;
%}

Integer [0-9]+
Letter [a-zA-Z]
LtrInt {Letter}|{Integer}
Whitespace [ \n\t\r]
Symbol [;,.:(){}]

%%
"//".*$ { /* inline comment */ }

"/*" { BEGIN(IN_COMMENT); /* multiline comment */ }
<IN_COMMENT>{
"*/"      BEGIN(INITIAL);
[^*\n]+   // eat comment in chunks
"*"       // eat the lone star
\n        yylineno++;
}

{Whitespace}+ {/* ignore */}
"if" {return T_IF;}
"then" {return T_THEN;}
"else" {return T_ELSE;}
"class" {return T_CLASS;}
"extends" {return T_EXTENDS;}
"var" {return T_VAR;}
"is" {return T_IS;}
"def" {return T_DEF;}
"object" {return T_OBJECT;}
"return" {return T_RETURN;}
"override" {return T_OVERRIDE;}
"new" {return T_NEW;}
"+" {return T_ADD;}
"-" {return T_SUB;}
"*" {return T_MULT;}
"/" {return T_DIV;}
"<>" {yylval.C = L_NOTEQ; return T_COMP;}
"<=" {yylval.C = L_INFEQ; return T_COMP;}
">=" {yylval.C = L_SUPEQ; return T_COMP;}
":=" {return T_AFFECT;}
"<" {yylval.C = L_INF; return T_COMP;}
">" {yylval.C = L_SUP; return T_COMP;}
"=" {yylval.C = L_EQ; return T_COMP;}
\"[^"]*\" {
  { char* s = yytext; // remove quotation marks
  do s++; while(*s != '"');
  *s = 0; }
  yylval.S = strdup(yytext + 1);
  return T_STRING;
}
{Letter}{LtrInt}* {yylval.S = strdup(yytext); return T_IDENT;}
{Integer} {yylval.I = atoi(yytext); return T_CONST;}
{Symbol} {return yytext[0];}
. { fprintf(stderr, "Unrecognized symbol: %s\n", yytext); exit(1); }
%%
