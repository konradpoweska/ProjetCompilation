#include <stdio.h>
#include "semantics.h"
#include "common.h"
#include "init.h" // access to globals


ClassP getTypeITE(TreeP expr) {
  if(expr->opLabel != L_IFTHENELSE) exit(UNEXPECTED);

  const ClassP thenType = getType(expr->u.children[1]);
  const ClassP elseType = getType(expr->u.children[2]);

  if(thenType == elseType) return thenType;
  else {
    printError("different types in then & else\n");
    exit(CONTEXT_ERROR);
  }
}

/* Returns type of an expression */
ClassP getType(TreeP expr) {
  switch(expr->opLabel) {
    case L_IFTHENELSE: return getTypeITE(expr);
    case L_CONSTINT: return &Integer;
    case L_CONSTSTR: return &String;
    default: return NIL(Class);
  }
}

/* Returns TRUE if targetType is supertype of thisType */
bool derivesType(ClassP thisType, ClassP targetType) {
  if(targetType == NIL(Class)) exit(UNEXPECTED);

  if(thisType == NIL(Class)) return FALSE;
  // reached top of hierarchy -> no match

  if(thisType == targetType) return TRUE;

  return derivesType(thisType->superClass, targetType);
  // launch recursive with upper level
}
