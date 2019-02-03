#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "structures.h"

// UTILITIES
void appendEnv(VarDeclP thisEnv, const VarDeclP superEnv, VarDeclP* const undoBuff);

void undoAppendEnv(VarDeclP* const undoBuff);

// LINKERS
ClassP getClass(ClassP* class);

VarDeclP getVarDecl(VarDeclP* var, VarDeclP env);

MethodP getMethod(MethodP* method, ClassP class);


// CHECKERS
bool checkVarDecl(VarDeclP var, VarDeclP env);

bool checkBlock(TreeP block, VarDeclP env);

bool sameArgList(VarDeclP l1, VarDeclP l2);

bool checkClassConstructorHeader(ClassP class);

bool checkClass(ClassP class);


// TYPE CHECKERS
ClassP getType(TreeP expr, VarDeclP env);

bool derivesType(ClassP type, ClassP superType);

#endif
