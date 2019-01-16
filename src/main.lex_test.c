/*
* Un petit programme de demonstration qui n'utilise que l'analyse lexicale.
* Permet principalement de tester la correction de l'analyseur lexical et de
* l'interface entre celui-ci et le programme qui l'appelle.
* Appel: test_lex < programme.txt
*/
#include <stdio.h>
#include "structures.h"
#include "grammar.y.h"




/* Fonction appelee par le programme principal pour obtenir l'unite lexicale
* suivante. Elle est produite par Flex (fichier tp_l.c)
*/
extern int yylex (void);

/* Le texte associe au token courant: defini et mis a jour dans tp_l.c */
extern char *yytext;

/* Le numero de ligne courant : defini et mis a jour dans tp_l.c */
extern int yylineno;

/* Variable pour interfacer flex avec le programme qui l'appelle, notamment
* pour transmettre de l'information, en plus du type d'unite reconnue.
* Le type YYSTYPE est defini dans tp.h.
*/
YYSTYPE yylval;

int main(void) {
  int token;

  while (1) {
    token = yylex();

    switch (token) {
      case 0: /* EOF */
        printf("Fin de l'analyse lexicale\n");
        return 0;
      case T_IDENTCLASS:
        printf("Identificateur de classe:\t\t%s\n", yylval.S);
        break;
      case T_IDENT:
        printf("Identificateur:\t\t%s\n", yylval.S);
        break;
      case T_CONST:
        printf("Constante:\t\t%d\n", yylval.I);
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
        printf("Mot-clef:\t\t%s\n",  yytext);
        break;
      case ';': case ',': case '.': case ':': case '(': case ')': case '{': case '}':
        printf("Symbole:\t\t%s\n",  yytext);
        break;
      case T_ADD:
        printf("Op. arithmetique\t+\n"); break;
      case T_SUB:
        printf("Op. arithmetique\t-\n"); break;
      case T_MULT:
        printf("Op. arithmetique\t*\n"); break;
      case T_DIV:
        printf("Op. arithmetique\t/\n"); break;
      case T_COMP:
        printf("Op. de comparaison\t");
        switch(yylval.C) {
          case L_NOTEQ: printf("<>"); break;
          case L_EQ: printf("="); break;
          case L_INF: printf("<"); break;
          case L_INFEQ: printf("<="); break;
          case L_SUP: printf(">"); break;
          case L_SUPEQ: printf(">="); break;
          default: printf("inconnu de code: %d", yylval.C);
        }
        printf("\n");
        break;
      case T_STRING:
        printf("Chaîne de caractères:\t%s\n", yylval.S);
        break;
      case T_AFFECT: printf("Symbole d'affectation\t:=\n");
        break;
      default:
        printf("Token non reconnu:\t\"%d\"\n", token);
    }
  }

  return 1;
}
