#include <stdio.h>
#include "semantics.h"
#include "common.h"
#include "init.h" // access to globals
#include <string.h>


////// UTILITIES //////

void appendEnv(VarDeclP thisEnv, const VarDeclP superEnv, VarDeclP* const undoBuff) {
  while(thisEnv->next != NIL(VarDecl)){
      thisEnv = thisEnv->next;
  }

  if(undoBuff == NIL(VarDeclP))
    printWarning("internal: no undo buffer provided in %s\n", __func__);
  else
    *undoBuff = thisEnv;

  thisEnv->next = superEnv;
}

void undoAppendEnv(VarDeclP* const undoBuff) {
  if(undoBuff == NIL(VarDeclP) || *undoBuff == NIL(VarDecl)) {
      printError("null pointer passed to %s\n", __func__);
      exit(UNEXPECTED);
  }

  (*undoBuff)->next = NIL(VarDecl);
  *undoBuff = NIL(VarDecl);
}



////// LINKERS //////

ClassP getClass(ClassP* class) {
  if(class == NIL(ClassP) || *class == NIL(Class)) {
    printError("null pointer passed to %s\n", __func__);
    exit(UNEXPECTED);
  }

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
  if(var == NIL(VarDeclP) || *var == NIL(VarDecl)) {
    printError("null pointer passed to %s\n", __func__);
    exit(UNEXPECTED);
  }

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
  if(method == NIL(MethodP) || *method == NIL(Method)) {
    printError("null pointer passed to %s\n", __func__);
    exit(UNEXPECTED);
  }

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
    ClassP initialValType = getType(var->initialValue, env);
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


bool sameArgList(VarDeclP l1, VarDeclP l2) {
  if(l1 == NIL(VarDecl) || l2 == NIL(VarDecl)) {
    printError("null argument passed to %s\n", __func__);
    exit(UNEXPECTED);
  }

  while(l1 != l2) { // if pointers are equal, lists are equal

    if(l1 == NIL(VarDecl) || l2 == NIL(VarDecl))
      return FALSE;
      // they are different because only one of them is NULL (see while condition)

    if(
      getClass(&l1->type) != getClass(&l2->type) ||
      strcmp(l1->name, l2->name) != 0
    )
      return FALSE;

    l1 = l1->next;
    l2 = l2->next;
  }

  return TRUE;
}

bool hasCopyInList(MethDeclP list, MethodP method, MethodP *copy){
  while(list != NIL(MethDecl)){

    if(strcmp(list->method->name, method->name) == 0){//same name

      if(list->method != method){//different address
        *copy = list->method;
        return TRUE;
      }
    }
    list = list->next;
  }
  return FALSE;
}

bool isSurcharge(ClassP class, MethodP method){
  if(hasCopyInList(class->methods, method, NIL(MethodP))){//another method exists in the class with the same name
    printError("Redefinition of method %s in class %s", method->name, class->name);
    exit(CONTEXT_ERROR);
  }

  while(class->superClass != NIL(Class)){
    class = class->superClass;
    MethodP eventualCopy = NIL(Method);

    if(hasCopyInList(class->methods, method, &eventualCopy)){//same name in super class
      if(!sameArgList(eventualCopy->parameters, method->parameters)){//different parameters means its a surcharge
        printError("Surcharge of method %s from class %s", eventualCopy->name, class->name);
        exit(CONTEXT_ERROR);
      }
    }
  }
  return FALSE;
}

bool checkClassConstructorHeader(ClassP class) {
  // assuming class & its contructor are not temp
  MethodP constr;

  if(class == NIL(Class) || (constr = class->constructor) == NIL(Method)) {
    printError("null argument in %s\n", __func__);
    exit(UNEXPECTED);
  }

  if(strcmp(constr->name, class->name) != 0) {
    printError(
      "in %s, constructor has different name than its class: %s != %s\n",
      __func__, constr->name, class->name);
    exit(UNEXPECTED);
  }

  // check arguments
  VarDeclP classArgs  = class->header;
  VarDeclP constrArgs = constr->parameters;

  if(!sameArgList(constrArgs, classArgs)) {
    printError("%s constructor header is different from class header\n",
      class->name);
    exit(CONTEXT_ERROR);
  }

  return TRUE;
}



bool checkClass(ClassP class) {
  /* - check circular inheritance
   * - check header & constructor
   * - add var-arguments to attributes
   * - check type of every attribute
   * - forbid method overloading
   * - check each method body + set owner
   */

  getClass(&class);

  checkClassConstructorHeader(class);

  return FALSE;
}


////// TYPE CHECKING //////

ClassP getTypeITE(TreeP expr, VarDeclP env) {
  if(expr == NIL(Tree) || expr->opLabel != L_IFTHENELSE) {
    printError("invalid argument in %s\n", __func__);
    exit(UNEXPECTED);
  }

  const ClassP thenType = getType(expr->u.children[1], env);
  const ClassP elseType = getType(expr->u.children[2], env);

  if(thenType == elseType) return thenType;
  else {
    printError("different types in then & else\n");
    exit(CONTEXT_ERROR);
  }
}

ClassP getTypeNEW(TreeP expr, VarDeclP env) {
  if(expr == NIL(Tree) || expr->opLabel != L_NEW) {
    printError("invalid argument in %s\n", __func__);
    exit(UNEXPECTED);
  }

  TreeP classNode = expr->u.children[0];
  if(classNode == NIL(Tree) || classNode->opLabel != L_CLASS) {
    printError("first child of L_NEW node is not a L_CLASS, in %s\n", __func__);
  }

  return getClass(&classNode->u.class);
}

ClassP getTypeSELEC(TreeP expr, VarDeclP env) {
  if(expr == NIL(Tree) || expr->opLabel != L_SELECTION) {
    printError("invalid argument in %s\n", __func__);
    exit(UNEXPECTED);
  }
  return NULL; //A FINIR

}

ClassP getTypeMSG(TreeP expr, VarDeclP env) {
  if(expr->opLabel != L_MESSAGE) exit(UNEXPECTED);

  return NULL; //A FINIR
}

ClassP getTypeCAST(TreeP expr, VarDeclP env) {
  if(expr == NULL || expr->opLabel != L_CAST) exit(UNEXPECTED);

  return expr->u.children[0]->u.class;
}

/* Returns type of an expression */
ClassP getType(TreeP expr, VarDeclP env) {
  switch(expr->opLabel) {
    case L_IFTHENELSE: return getTypeITE(expr, env);
    case L_NEW: return getTypeNEW(expr, env);
    case L_CONSTINT: return &Integer;
    case L_CONSTSTR: return &String;
    case L_SELECTION: return getTypeSELEC(expr, env);
    case L_MESSAGE: return getTypeMSG(expr, env);
    case L_CAST: return getTypeCAST(expr, env);
    default: {
      printError("In %s, unrecognised type of expression (label=%d)\n",
        __func__, expr->opLabel);
      exit(UNEXPECTED);
    }
  }
}

/* Returns TRUE if "type" is equal to or subtype of "superType" */
bool derivesType(ClassP type, ClassP superType) {
  if(superType == NIL(Class)) exit(UNEXPECTED);

  if(type == NIL(Class)) return FALSE;
  // reached top of hierarchy -> no match

  if(type == superType) return TRUE;

  return derivesType(type->superClass, superType);
  // launch recursive with upper level
}
