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

ClassP getTypeNEW(TreeP expr) {
  if(expr->opLabel != L_NEW) exit(UNEXPECTED);

  return expr->u.children[0]->u.class;
}

ClassP getTypeSELEC(TreeP expr) {
  if(expr->opLabel != L_NEW) exit(UNEXPECTED);

  return NULL; //A FINIR

}

ClassP getTypeMSG(TreeP expr) {
  if(expr->opLabel != L_MESSAGE) exit(UNEXPECTED);
  
  return NULL; //A FINIR
}

ClassP getTypeCAST(TreeP expr) {
  if(expr == NULL || expr->opLabel != L_CAST) exit(UNEXPECTED);

  return expr->u.children[0]->u.class;
}

/* Returns type of an expression */
ClassP getType(TreeP expr) {
  switch(expr->opLabel) {
    case L_IFTHENELSE: return getTypeITE(expr);
    case L_NEW: return getTypeNEW(expr);
    case L_CONSTINT: return &Integer;
    case L_CONSTSTR: return &String;
    case L_SELECTION: return getTypeSELEC(expr);
    case L_MESSAGE: return getTypeMSG(expr);
    case L_CAST: return getTypeCAST(expr);
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
