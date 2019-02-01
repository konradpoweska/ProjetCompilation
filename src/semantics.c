#include <stdio.h>
#include "semantics.h"
#include "common.h"
#include "init.h" // access to globals
#include <string.h>


////// UTILITIES //////

void appendEnv(VarDeclP thisEnv, VarDeclP superEnv) {
  // append superEnv to the end of thisEnv
  while(thisEnv->next != NIL(VarDecl)){
      thisEnv = thisEnv->next;
  }
  thisEnv->next = superEnv;
}



////// LINKERS //////

ClassP getClass(ClassP* class) {
  if(!(*class)->tmp){
    return *class;
  }
  ClassP realClass = getClassInList(classList, (*class)->name);
  if(realClass == NIL(Class)){
    printError("Class %s has not been declared\n", (*class)->name);
    exit(CONTEXT_ERROR);
  }
  *class = realClass;
  return realClass;
}


VarDeclP getVarDecl(VarDeclP* var, VarDeclP env) {
  /*if(!(*var)->tmp){
    return *var;
  }*/
  VarDeclP realVar = getVarInList(env, (*var)->name);
  if(realVar == NIL(VarDecl)){
    printError("Var %s has not been declared\n", (*var)->name);
    exit(CONTEXT_ERROR);
  }
  *var = realVar;
  return realVar;
}


MethodP getMethod(MethodP* method, ClassP class) {
  if(!(*method)->tmp){
    return *method;
  }
  MethodP realMethod = getMethodInList(class->methods, (*method)->name);
  if(realMethod == NIL(Method)){
    printError("Method %s does not exist in class %s\n", (*method)->name, (class)->name);
    exit(CONTEXT_ERROR);
  }
  *method = realMethod;
  return realMethod;
}


////// CHECKERS //////

bool checkVarDecl(VarDeclP var, VarDeclP env) {
  //assuming var is not temp

  getClass(&(var->type));//Checks if the type exists, and link it in the VarDecl if it does

  if(var->initialValue != NIL(Tree)){
    checkBlock(var->initialValue, env);//Checks if affected expression is valid
    ClassP initialValType = getType(var->initialValue);
    if(!derivesType(initialValType, var->type)){
      printError("Expected %s, got %s\n", var->type->name, initialValType->name);
      exit(CONTEXT_ERROR);
    }
  }

  return TRUE;
}


bool checkBlock(TreeP block, VarDeclP env) {
  /* TODO
   * - check each instruction...
   */
  return FALSE;
}


bool checkClassConstructorHeader(ClassP class) {
  // assuming class & its contructor are not temp
  MethodP constr;

  if(class == NIL(Class) || (constr = class->constructor) == NIL(Method)) {
    printError("null pointer in %d", __func__);
    exit(UNEXPECTED);
  }

  if(strcmp(constr->name, class->name) != 0) {
    printError(
      "in %d, constructor has different name than its class: %s != %s\n",
      __func__, constr->name, class->name);
    exit(UNEXPECTED);
  }

  // check arguments
  VarDeclP classArgs  = class->header;
  VarDeclP constrArgs = constr->parameters;

  while(classArgs != constrArgs) {

    if(classArgs != NULL || constrArgs != NULL)
      return FALSE;
      // they are different because only one of them is NULL (see while condition)

    if(
      getClass(&classArgs->type) != getClass(&constrArgs->type) ||
      strcmp(classArgs->name, constrArgs->name) != 0
    ) {
      printError("%s constructor header is different from class header\n",
        class->name);
      exit(CONTEXT_ERROR);
      return FALSE;
    }

    classArgs = classArgs->next;
    constrArgs = constrArgs->next;
  }

  return TRUE;
}



bool checkClass(ClassP* class) {
  /* - check header & constructor
   * - check type of every attribute
   * - check each method body
   * - forbid method overloading
   * - set each method owner
   */
  return FALSE;
}


////// TYPE CHECKING //////

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
    default: {
      printError("In %s, unrecognised type of expression (label=%d)",
        __func__, expr->opLabel);
      exit(UNEXPECTED);
    }
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
