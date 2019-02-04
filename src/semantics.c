#include <stdio.h>
#include "semantics.h"
#include "common.h"
#include "init.h" // access to globals
#include "structures.h"
#include <string.h>


////// UTILITIES //////

void appendEnv(VarDeclP* localEnv, const VarDeclP superEnv,
                VarDeclP** const undoBuff) {

  if(localEnv == NIL(VarDeclP) || undoBuff == NIL(VarDeclP*)) {
    printError("null argument passed to %s\n", __func__);
    exit(UNEXPECTED);
  }

  VarDeclP l = *localEnv;

  if(l == NIL(VarDecl)) { // if empty localEnv, set it to superEnv
    *localEnv = superEnv;
    *undoBuff = localEnv;
    return;
  }

  while(l->next != NIL(VarDecl))
    l = l->next;

  l->next = superEnv;
  *undoBuff = &l->next;
}

void undoAppendEnv(VarDeclP** const undoBuff) {
  if(undoBuff == NIL(VarDeclP*) || *undoBuff == NIL(VarDeclP)) {
      printError("null pointer passed to %s\n", __func__);
      exit(UNEXPECTED);
  }

  **undoBuff = NIL(VarDecl);
  *undoBuff = NIL(VarDeclP);
}



////// LINKERS //////
/*These methods return the actual object.
 * For example, for a method it will return the method as it has been declared, so we have access to parameters types.
 * This allows to have all the necessary information everywhere the method is called.
 * This is why it takes a potentially temporary struct : if it's just a call, we will only have the name,
 * and we go through the environment to link that name to the complete declaration.
 */

ClassP getClass(ClassP* class) {
  if(class == NIL(ClassP) || *class == NIL(Class)) {
    printError("null pointer passed to %s\n", __func__);
    exit(UNEXPECTED);
  }

    //class is not temporary
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

    //var is not temporary
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

  //method is not temporary
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
    //another method exists in the class with the same name
  if(hasCopyInList(class->methods, method, NIL(MethodP))){
    printError("Redefinition of method %s in class %s", method->name, class->name);
    exit(CONTEXT_ERROR);
  }

  while(class->superClass != NIL(Class)){
    class = class->superClass;
    MethodP eventualCopy = NIL(Method);

      //same name in super class
    if(hasCopyInList(class->methods, method, &eventualCopy)){
        //different parameters means its a surcharge
      if(!sameArgList(eventualCopy->parameters, method->parameters)){
        printError("Surcharge of method %s from class %s", eventualCopy->name, class->name);
        exit(CONTEXT_ERROR);
      }
    }
  }
  return FALSE;
}

bool checkCircularInheritance(ClassP class) {

  if(class == NIL(Class)){
    printError("null argument in %s\n", __func__);
    exit(UNEXPECTED);
  }

  while(class->superClass != NIL(Class)){

    class -> alreadyMet = TRUE;
    class = class->superClass;
    if(class -> alreadyMet == TRUE)
      return FALSE;  //this class has a circular inheritance
    }
    return TRUE; //this class hasn't a circular inheritance

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

  checkCircularInheritance(class);

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

/* Returns true if the instanciated class is declared and in scope
 */
bool checkNEW(TreeP expr, VarDeclP env){
    if(expr == NIL(Tree) || expr->opLabel != L_NEW) {
        printError("invalid argument in %s\n", __func__);
        exit(UNEXPECTED);
    }

    ClassP class = expr->u.children[0]->u.class;

    if(class == NIL(Class)) exit(UNEXPECTED);


    if(getClassInList(classList, class->name) == NIL(Class)){
        printError("Trying to instantiate non declared class %s", class->name);
        exit(CONTEXT_ERROR);
    }

    return TRUE;
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
/*
bool checkMSG(TreeP expr, VarDeclP env){
    if(expr->opLabel != L_MESSAGE) exit(UNEXPECTED);

    MethodP calledMethod = expr->u.children[1]->u.method;

    //malformed tree
    if(calledMethod == NIL(Method)) exit(UNEXPECTED);

    if(calledMethod->tmp){
        //this checks if the method exists in the class
        getMethod(&calledMethod, getType(expr->u.children[0], env));
    }

    return TRUE;
}
 */

ClassP getTypeMSG(TreeP expr, VarDeclP env) {
  if(expr->opLabel != L_MESSAGE) exit(UNEXPECTED);

    MethodP calledMethod = expr->u.children[1]->u.method;

    //malformed tree
    if(calledMethod == NIL(Method)) exit(UNEXPECTED);

    //checkMSG(expr, env);

    if(calledMethod->tmp){
        //this checks if the method exists in the class
        getMethod(&calledMethod, getType(expr->u.children[0], env));
    }

  return calledMethod->returnType;
}

/* Returns true if all different types are correctly declared and in the scope;
 * and the class we are trying to cast to is a superclass of the casted expression.
 */
bool checkCAST(TreeP expr, VarDeclP env){
    if(expr == NULL || expr->opLabel != L_CAST) exit(UNEXPECTED);

    ClassP class = expr->u.children[0]->u.class;

    //malformed tree
    if(class == NIL(Class)) exit(UNEXPECTED);

    ClassP realClass = getClassInList(classList, class->name);

    if(realClass == NIL(Class)){
        printError("cast into unknown type : %s", class->name);
        exit(CONTEXT_ERROR);
    }

    //checks are performed by getType, so we do not need to check that originalClass does exist
    ClassP originalClass = getType(expr->u.children[1], env);

    if(!derivesType(originalClass, realClass)){
        printError("unauthorised cast : %s is not a subclass of %s", originalClass->name, realClass->name);
        exit(CONTEXT_ERROR);
    }

    return TRUE;
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
