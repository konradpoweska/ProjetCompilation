#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "structures.h"


ClassP getType(TreeP expr);

bool derivesType(ClassP thisType, ClassP targetType);

#endif
