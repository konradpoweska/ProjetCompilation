#include "common.h"
#include <stdio.h>

extern int yylineno;

/* yyerror:  fonction importee par Bison et a fournir explicitement.
 * Elle est appelee quand Bison detecte une erreur syntaxique.
 * Ici on se contente d'un message minimal.
 */
void yyerror(char *ignore) {
    if(ignore){}
    
    fprintf(stderr, "Syntax error on line: %d\n", yylineno);
}
