%x IN_COMMENT
%{
#include "common.h"
#include "structures.h"
#include "grammar.y.h"
extern YYSTYPE yylval;

char* parseString(char* s) {
  char *res = calloc(strlen(s), sizeof(char)),
       *r = res; // copy

  if(*s == '"') s++; // skip the beginning quote mark

  while(*s) {
    if(*s == '"') { // end of string
      break;
    }
    else if(*s == '\\') { // escape character
      s++;
      switch(*s) {
        case 'a': *(r++) = '\a'; break;
        case 'b': *(r++) = '\b'; break;
        case 'n': *(r++) = '\n'; break;
        case 'r': *(r++) = '\r'; break;
        case 't': *(r++) = '\t'; break;
        case 'f': *(r++) = '\f'; break;
        case 'v': *(r++) = '\v'; break;
        case '0': *(r++) = '\0'; break;
        case '\\': *(r++) = '\\'; break;
        case '\'': *(r++) = '\''; break;
        case '"': *(r++) = '"'; break;
        default: printWarning("Unknown escaped character \"\\%c\" ignored\n", *s);
      }
      s++;
      continue;
    }
    else *(r++) = *(s++);
  }

  *r = 0;
  return res;
}

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
[^*]+     // eat comment in chunks
"*"       // eat the lone star
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
"&" {return T_CONCAT;}
"<>" {yylval.C = L_NOTEQ; return T_COMP;}
"<=" {yylval.C = L_INFEQ; return T_COMP;}
">=" {yylval.C = L_SUPEQ; return T_COMP;}
":=" {return T_AFFECT;}
"<" {yylval.C = L_INF; return T_COMP;}
">" {yylval.C = L_SUP; return T_COMP;}
"=" {yylval.C = L_EQ; return T_COMP;}

\"(\\\"|[^\"])*\" {
  yylval.S = parseString(yytext);
  return T_STRING;
}

[A-Z]{LtrInt}* {yylval.S = strdup(yytext); return T_IDENTCLASS;}
{Letter}{LtrInt}* {yylval.S = strdup(yytext); return T_IDENT;}
{Integer} {yylval.I = atoi(yytext); return T_CONST;}
{Symbol} {return yytext[0];}
. { printError("Unrecognized symbol: %s\n", yytext); exit(LEXICAL_ERROR); }
%%
