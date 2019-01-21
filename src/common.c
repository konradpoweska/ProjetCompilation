#include "common.h"
#include <stdio.h>
#include <stdarg.h>

extern int yylineno;

/* yyerror:  fonction importee par Bison et a fournir explicitement.
 * Elle est appelee quand Bison detecte une erreur syntaxique.
 * Ici on se contente d'un message minimal.
 */
void yyerror(char *ignore) {
    if(ignore){}
    
    fprintf(stderr, "Syntax error on line: %d\n", yylineno);
}

void printError(const char* format, ...) {
  va_list args;
  va_start(args, format);

  fprintf(stderr, "%sError: %s", ERROR_COLOR, DEFAULT_COLOR);
  vfprintf(stderr, format, args);

  va_end(args);
}

void printWarning(const char* format, ...) {
  va_list args;
  va_start(args, format);

  fprintf(stderr, "%sWarning: %s", WARNING_COLOR, DEFAULT_COLOR);
  vfprintf(stderr, format, args);

  va_end(args);
}
