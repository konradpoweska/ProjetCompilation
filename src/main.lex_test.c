// Usage: test_lex < program.txt
#include <stdio.h>
#include "structures.h"
#include "grammar.y.h"

// Given by Flex, in generated lex.c
extern int yylex (void);
extern char *yytext;
extern int yylineno;

// Variable (union) to store token data, used in lex.l
YYSTYPE yylval;

int main(void) {
  int token;

  while (1) {
    token = yylex();
    printf("%3d| ", yylineno);

    switch (token) {
      case 0: /* EOF */
        printf("End of file\n");
        return 0;

      case T_IDENTCLASS:
        printf("Class identifier:\t\t%s\n", yylval.S);
        break;

      case T_IDENT:
        printf("Identifier:\t\t%s\n", yylval.S);
        break;

      case T_CONST:
        printf("Constant:\t\t\t%d\n", yylval.I);
        break;

      case T_IF:
      case T_THEN:
      case T_ELSE:
      case T_CLASS:
      case T_EXTENDS:
      case T_VAR:
      case T_IS:
      case T_DEF:
      case T_OBJECT:
      case T_RETURN:
      case T_OVERRIDE:
      case T_NEW:
        printf("Keyword:\t\t\t%s\n",  yytext);
        break;

      case ';': case ',': case '.': case ':':
      case '(': case ')': case '{': case '}':
        printf("Symbol:\t\t\t%s\n",  yytext);
        break;

      case T_ADD: printf("Arithmetic operator:\t+\n"); break;
      case T_SUB: printf("Arithmetic operator:\t-\n"); break;
      case T_MULT: printf("Arithmetic operator:\t*\n"); break;
      case T_DIV: printf("Arithmetic operator:\t/\n"); break;

      case T_COMP:
        printf("Comparison operator:\t");
        switch(yylval.C) {
          case L_NOTEQ: printf("<>"); break;
          case L_EQ: printf("="); break;
          case L_INF: printf("<"); break;
          case L_INFEQ: printf("<="); break;
          case L_SUP: printf(">"); break;
          case L_SUPEQ: printf(">="); break;
          default: printf("unknown: %d", yylval.C);
        }
        printf("\n");
        break;

      case T_STRING:
        printf("String:\t\t\t\"%s\"\n", yylval.S);
        break;

      case T_CONCAT:
        printf("Concatenation operator:\t&\n");
        break;

      case T_AFFECT:
        printf("Assignment symbol:\t:=\n");
        break;

      default:
        printf("Unrecognized token:\t\"%d\"\n", token);
    }
  }

  return 1;
}
