#ifndef COMMON_H
#define COMMON_H

/**
 * NEW : Macro used for dynamic memory allocation.
 * @param howmany, number of object to create
 * @param type, type of the structure to create (not the pointer but the struct)
 * @return the new pointer of the structure
 */
#define NEW(howmany, type) (type *) calloc((unsigned) howmany, sizeof(type))

/**
 * NIL : Macro used for dynamic memory allocation.
 * @param type, type of the structure to create (not the pointer but the struct)
 * @return the NIL pointer corresponding to the structure
 */
#define NIL(type) (type *) 0

/**
 * Defining a boolean type for C
 */
#define TRUE 1
#define FALSE 0
typedef int bool;


/* Error code */
#define NO_ERROR	0
#define USAGE_ERROR	1
#define LEXICAL_ERROR	2
#define SYNTAX_ERROR    3
#define CONTEXT_ERROR	4
#define EVAL_ERROR	5
#define UNEXPECTED	10

/* Display colors */
#define ERROR_COLOR "\x1B[91m"
#define WARNING_COLOR "\x1B[93m"
#define DEFAULT_COLOR "\x1B[0m"


void yyerror(char *ignore);

void printError(const char* format, ...);

void printWarning(const char* format, ...);

#endif /* end of include guard: COMMON_H */
