#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "structures.h"

// UTILITIES
void appendEnv(VarDeclP thisEnv, VarDeclP superEnv);


// LINKERS
ClassP getClass(ClassP* class);

VarDeclP getVarDecl(VarDeclP* var, VarDeclP env);

MethodP getMethod(MethodP* method, ClassP class);


// CHECKERS
bool checkVarDecl(VarDeclP var, VarDeclP env);

bool checkBlock(TreeP block, VarDeclP env);

bool checkClassConstructorHeader(ClassP class);

bool checkClass(ClassP* class);


// TYPE CHECKERS
ClassP getType(TreeP expr);

bool derivesType(ClassP thisType, ClassP targetType);

#endif
