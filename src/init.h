#ifndef INIT_H
#define INIT_H

/* ICI ON DECLARE LES TRUCS POUR INIT LE COMPILO (type predef)*/
#include "structures.h"


/* Predefined class */
ClassP Integer;
ClassP String;

/* A list that stores all the classes of the programm in order to use them for context verif */
ClassDeclP classList;

void _Init();

#endif /* end of include guard: INIT_H */
