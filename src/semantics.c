#include <stdio.h>
#include "semantics.h"
#include "common.h"
#include "init.h" // access to globals
#include "structures.h"
#include <string.h>


///////////////////////////
//////// UTILITIES ////////
///////////////////////////

void concatEnv(VarDeclP* localEnv, const VarDeclP superEnv,
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


void undoConcatEnv(VarDeclP** const undoBuff) {
  if(undoBuff == NIL(VarDeclP*) || *undoBuff == NIL(VarDeclP)) {
      printError("null pointer passed to %s\n", __func__);
      exit(UNEXPECTED);
  }

  **undoBuff = NIL(VarDecl);
  *undoBuff = NIL(VarDeclP);
}




/////////////////////////
//////// LINKERS ////////
/////////////////////////

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

  // TODO : add tmp attribute to VarDecl
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




/////////////////////////////////////
//////// EXPRESSION CHECKING ////////
/////////////////////////////////////

/* Recursive function : adds every variable declaration to scope checking its
 * initialValue, and checksExpression(callback) with env = scope+env.
 */
bool checkScope(VarDeclP scope, TreeP callback, VarDeclP env) {
  if(scope == NIL(VarDecl))
    return checkExpression(callback, env);

  getClass(&scope->type); /* link type */

  if(scope->initialValue != NIL(Tree)) { /* check & link initial value */
    if(!checkExpression(scope->initialValue, env))
      return FALSE;

    ClassP rightType = getType(scope->initialValue);
    ClassP leftType = scope->type;

    if(!derivesType(rightType, leftType)) {
      printError("Assignment of incompatible type: %s to %s\n", rightType->name, leftType->name);
      exit(CONTEXT_ERROR);
    }
  }

  VarDeclP next = scope->next;
  scope->next = env; // scope is now first element + env

  bool val = checkScope(next, callback, scope);

  scope->next = next; // back to normal

  return val;
}


bool checkBlock(TreeP expr, VarDeclP env) {
  TreeP firstChild = getChild(expr, 0);

  if(firstChild == NIL(Tree)) {
    return TRUE;
  }
  else if(firstChild->opLabel == L_LISTVAR) {
    return checkScope(firstChild->u.ListDecl, getChild(expr, 1), env);
  }
  else
    return checkExpression(firstChild, env);
}


bool checkListInst(TreeP expr, VarDeclP env) {
  if(!checkExpression(getChild(expr, 0), env)) return FALSE;
  if(expr->nbChildren == 2)
    if(!checkExpression(getChild(expr, 1), env)) return FALSE;
  return TRUE;
}


/* Returns true if all different types are correctly declared and in the scope;
 * and the class we are trying to cast to is a superclass of the casted expression.
 */
bool checkCAST(TreeP expr, VarDeclP env){
    TreeP classNode = getChild(expr, 0);

    if(classNode == NIL(Tree) || classNode->opLabel != L_CLASS) {
      printError("internal: no L_CLASS child on L_CAST node\n");
      exit(UNEXPECTED);
    };

    ClassP destType = getClass(&classNode->u.class);

    if(destType == NIL(Class)){ // is never called : getClass exits() anyway
        printError("cast into unknown type : %s", classNode->u.class->name);
        exit(CONTEXT_ERROR);
    }

    checkExpression(getChild(expr, 1), env);
    ClassP exprType = getType(getChild(expr, 1));

    if(!derivesType(exprType, destType)){
        printError("unauthorised cast : %s is not a subclass of %s", exprType->name, destType->name);
        exit(CONTEXT_ERROR);
    }

    return TRUE;
}


bool checkAffect(TreeP expr, VarDeclP env) {
  if(expr->nbChildren == 1) return checkExpression(getChild(expr, 0), env);

  /* Todo : allow left to only be a var or selection */
  checkExpression(getChild(expr, 0), env);
  ClassP leftType  = getType(getChild(expr, 0));
  checkExpression(getChild(expr, 1), env);
  ClassP rightType = getType(getChild(expr, 1));

  if(!derivesType(rightType, leftType)) {
    printError("Assignment of incompatible type: %s to %s", rightType->name, leftType->name);
    exit(CONTEXT_ERROR);
  }

  return TRUE;
}


bool checkID(TreeP expr, VarDeclP env){
    VarDeclP var = getVarDecl(&expr->u.ListDecl, env);

    return var != NIL(VarDecl);
}


bool checkSELEC(TreeP expr, VarDeclP env){
    TreeP identifier = getChild(expr, 1);

    checkExpression(getChild(expr, 0), env);
    ClassP selectedOn = getType(getChild(expr, 0));

    //check that the selected value is an attribute of the selected on class
    if(getVarInList(selectedOn->attributes, identifier->u.ListDecl->name) == NIL(VarDecl)){
        printError("%s is not an attribute of %s", identifier->u.ListDecl->name, selectedOn->name);
        exit(CONTEXT_ERROR);
    }

    return checkID(identifier, env);
}


/* Checks if provided arguments at call of method/constructor are compatible
 * with method/constructor definition */
bool checkProvidedArgs(TreeP exprList, VarDeclP definition, VarDeclP env) {
  if(
    exprList == NIL(Tree) ||
    exprList->opLabel != L_PARAMLIST
  ) {
      printError("invalid argument in %s\n", __func__);
      exit(UNEXPECTED);
  }

  VarDeclP param = definition; /* We are on the first param */

  TreeP arg = exprList;

  while(arg!=NIL(Tree)){

    /* Too many arguments provided */
    if(param == NIL(VarDecl)){

        printError("Too many arguments used !");
        exit(CONTEXT_ERROR);

    }

    /*Last param of the list*/
    if(arg->nbChildren==1){

        if(!derivesType(getType(getChild(arg,0)), param->type)) {
            printError("Invalid param type !");
            exit(CONTEXT_ERROR);
        }

    }
    /* Param + continue the list of param */
    else if(arg->nbChildren==2){

        if(!derivesType(getType(getChild(arg,0)), param->type)) {
            printError("Invalid param type !");
            exit(CONTEXT_ERROR);
        }

        arg=getChild(arg,1);
        param=param->next;
    }
    else{
        printError("Invalid expression type in the expression list in %s!",__func__);
        exit(CONTEXT_ERROR);
    }

  }

  return TRUE;
}


bool checkNEW(TreeP expr, VarDeclP env){
  TreeP classNode = getChild(expr, 0);

  if(classNode == NIL(Tree) || classNode->opLabel != L_CLASS) {
    printError("internal: first child of L_NEW node is not a L_CLASS, in %s\n", __func__);
    exit(UNEXPECTED);
  }

  ClassP class = getClass(&classNode->u.class);
  if(class == NIL(Class)) {
    printError("Trying to instantiate non declared class %s\n", class->name);
    exit(CONTEXT_ERROR);
  }

  printf("Checking arguments of %s constructor\n", class->name);
  if(!checkProvidedArgs(getChild(expr, 1), class->header, env)) {
    printError("incompatible arguments given for %s constructor\n", class->name);
    exit(CONTEXT_ERROR);
  }

  return TRUE;
}


bool checkMSG(TreeP expr, VarDeclP env) {
  TreeP methodNode = getChild(expr, 1);

  checkExpression(getChild(expr, 0), env);
  ClassP selectedOn = getType(getChild(expr, 0));

  MethodP method = getMethod(&methodNode->u.method, selectedOn);

  //check that the selected value is an attribute of the selected on class
  if(method == NIL(Method)){
      printError("%s is not an method of %s", method->name, selectedOn->name);
      exit(CONTEXT_ERROR);
  }

  if(!checkProvidedArgs(getChild(expr, 2), method->parameters, env)) {
    printError("incompatible arguments given for %s method\n", method->name);
    exit(CONTEXT_ERROR);
  }

  return TRUE;
}


bool checkExpression(TreeP expr, VarDeclP env) {
  if(expr == NIL(Tree)) {
    printError("internal: null argument in %s\n", __func__);
    exit(UNEXPECTED);
  }

  switch(expr->opLabel) {
    case L_BLOC: return checkBlock(expr, env);
    case L_LISTINST: return checkListInst(expr, env);
    case L_ID: return checkID(expr, env);
    case L_MESSAGE: return checkMSG(expr, env);
    case L_NEW: return checkNEW(expr, env);
    case L_CAST: return checkCAST(expr, env);
    case L_SELECTION: return checkSELEC(expr, env);
    case L_AFFECT: return checkAffect(expr, env);

    case L_CONSTINT:
    case L_CONSTSTR:
      return TRUE;

    case L_ADD: case L_SUB: // Todo : recursive check
    case L_MULT: case L_DIV: // Todo : recursive check
    case L_UNARYPLUS: case L_UNARYMINUS: // Todo : recursive check
    case L_CONCAT: // Todo : recursive check
      return FALSE;

    default:
      printError("internal: in %s, unrecognized type of expression (label=%d)\n",
        __func__, expr->opLabel);
      exit(UNEXPECTED);
  }
  return FALSE;
}




////////////////////////////////
//////// CLASS CHECKING ////////
////////////////////////////////

bool sameArgList(VarDeclP l1, VarDeclP l2) {
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

  ClassP ccopy = class;

  while(class->superClass != NIL(Class)) {
    getClass(&class->superClass);
    class->alreadyMet = TRUE;
    class = class->superClass;

    if(class->alreadyMet == TRUE) {
      printError("circular inheritance from class %s\n", class->name);
      exit(CONTEXT_ERROR);
    }
  }

  /* reset all alreadyMet flags */
  class = ccopy;
  do { class->alreadyMet = FALSE; } while((class = class->superClass));

  return TRUE; //this class doesn't have a circular inheritance
}


bool checkClassConstructorHeader(ClassP class) {
  // assuming class & its contructor are not temp
  MethodP constr;

  if(class == NIL(Class)) {
    printError("null argument in %s\n", __func__);
    exit(UNEXPECTED);
  }
  if((constr = class->constructor) == NIL(Method)) {
    printError("in %s, class %s has no constructor\n", __func__, class->name);
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
   * - check method headers, body + set owner
   * - forbid method overloading
   */

  checkCircularInheritance(class);

  checkClassConstructorHeader(class);

  /* TODO : check attributes + add var-arguments */

  MethDeclP methdL = class->methods;
  while(methdL != NIL(MethDecl)) {
    // checkMethod(methdL->method, env);
    methdL->method->owner = class;
    isSurcharge(class, methdL->method);
    methdL = methdL->next;
  }

  return FALSE;
}


bool checkAllClasses() {
  ClassDeclP clist = classList; // keep global list untouched

  while(clist != NIL(ClassDecl)) {
    if(!clist->class->predef) // do not check predefined classes
      if(!checkClass(clist->class))
        return FALSE;

    clist = clist->next;
  }

  return TRUE;
}




///////////////////////////////
//////// TYPE CHECKING ////////
///////////////////////////////

ClassP getTypeITE(TreeP expr) {
  if(expr == NIL(Tree) || expr->opLabel != L_IFTHENELSE) {
    printError("invalid argument in %s\n", __func__);
    exit(UNEXPECTED);
  }

  const ClassP thenType = getType(getChild(expr, 1));
  const ClassP elseType = getType(getChild(expr, 2));

  if(thenType == elseType) return thenType;
  else {
    printError("different types in then & else\n");
    exit(CONTEXT_ERROR);
  }
}


ClassP getTypeNEW(TreeP expr) {
  TreeP classNode = getChild(expr, 0);
  return classNode->u.class;
}


ClassP getTypeSELEC(TreeP expr) {
  return getType(getChild(expr, 1));
}


ClassP getTypeMSG(TreeP expr) {
  MethodP calledMethod = getChild(expr, 1)->u.method;

  //malformed tree
  if(calledMethod == NIL(Method)) exit(UNEXPECTED);

  return calledMethod->returnType;
}


ClassP getTypeCAST(TreeP expr) {
  return getChild(expr, 0)->u.class;
}

ClassP getTypeID(TreeP expr){
  return expr->u.ListDecl->type;
}

/* Returns the type (ClassP) returned by an expression */
ClassP getType(TreeP expr) {
  switch(expr->opLabel) {
    case L_IFTHENELSE: return getTypeITE(expr);
    case L_NEW: return getTypeNEW(expr);


    case L_CONSTINT:
    case L_ADD: case L_SUB:
    case L_MULT: case L_DIV:
    case L_UNARYPLUS: case L_UNARYMINUS:
      return Integer;

    case L_CONSTSTR:
    case L_CONCAT:
      return String;

    case L_SELECTION: return getTypeSELEC(expr);
    case L_MESSAGE: return getTypeMSG(expr);
    case L_CAST: return getTypeCAST(expr);
    case L_ID: return getTypeID(expr);

    default:
      printError("In %s, unrecognised type of expression (label=%d)\n",
        __func__, expr->opLabel);
      exit(UNEXPECTED);
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
