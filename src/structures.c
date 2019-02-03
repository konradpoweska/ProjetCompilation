#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include "structures.h"
#include "common.h"
#include "init.h"


/* Predef classes */
extern Class Integer;
extern Class String;

/* A list that stores all the classes of the programm in order to use them for context verif */
extern ClassDeclP classList;


/********************************* Functions relative to the AST (Tree struct) *********************************/

/* Fonction AUXILIAIRE pour la construction d'arbre : renvoie un squelette
 * d'arbre pour 'nbChildren' fils et d'etiquette 'op' donnee. L'appelant
 * doit lui-même stocker ses fils dans la strucutre que MakeNode renvoie
 */
/**
 *
 *
 */
TreeP makeNode(int nbChildren, Label label) {
	TreeP tree = NEW(1, Tree);
	tree->opLabel = label; tree->nbChildren = nbChildren;
	tree->u.children = nbChildren > 0 ? NEW(nbChildren, TreeP) : NIL(TreeP);
	return(tree);
}

/* Construction d'un arbre a nbChildren branches, passees en parametres
 * 'op' est une etiquette symbolique qui permet de memoriser la construction
 * dans le programme source qui est representee par cet arbre.
 * Une liste preliminaire d'etiquettes est dans tp.h; il faut l'enrichir selon
 * vos besoins.
 * Cette fonction prend un nombre variable d'arguments: au moins deux.
 * Les eventuels arguments supplementaires doivent etre de type TreeP
 * (defini dans tp.h)
 */
/** A FINIR
 */
TreeP makeTree(Label label, int nbChildren, ...) {
  va_list args;
  int i;
  TreeP tree = makeNode(nbChildren, label);
  va_start(args, nbChildren);
  for (i = 0; i < nbChildren; i++) {
    tree->u.children[i] = va_arg(args, TreeP);
  }
  va_end(args);
  return(tree);
}


/* Constructeur de feuille dont la valeur est un entier */
TreeP makeLeafInt(Label label, int val) {
  TreeP tree = makeNode(0, label);
  tree->u.valInt = val;
  return(tree);
}


/* Constructeur de feuille dont la valeur est une chaine de caracteres.
 * Construit un doublet pour la future variable et stocke son nom dedans.
 */
TreeP makeLeafStr(Label label, char *str) {
  TreeP tree = makeNode(0, label);
  tree->u.valStr = str;
  return(tree);
}

/* Constructeur de feuille dont la valeur est une chaine de caracteres.
 * Construit un doublet pour la future variable et stocke son nom dedans.
 */
TreeP makeLeafIdent(Label label, VarDeclP ident) {
  TreeP tree = makeNode(0, label);
  tree->u.ListDecl = ident;
  return(tree);
}


TreeP makeLeafClass(Label label, ClassP class){

	TreeP tree = makeNode(0, label);
    tree->u.class = class;
    return(tree);

}

TreeP makeLeafMethod(Label label, MethodP method){

	TreeP tree = makeNode(0, label);
    tree->u.method = method;
    return(tree);

}

/* return child N°i (i goes from 0 to N-1) */
TreeP getChild(TreeP tree, int i) {
  return tree->u.children[i];
}

/* Printing the AST and lists */
void printAST(TreeP main){

	/* TODO */

}

void printExpr (TreeP expr, int depth){

	if(expr==NIL(Tree)){
		return;
	}

	/*printf("\nDepth %d:\n",depth);*/

	switch (expr->opLabel){
	case L_CONCAT :
	case L_ADD :
	case L_SUB :
	case L_MULT :
	case L_DIV :
	case L_AFFECT :
	case L_NOTEQ :
	case L_EQ :
	case L_INF :
	case L_INFEQ :
	case L_SUP :
	case L_SUPEQ :     	printExpr(getChild(expr, 0),depth+1);
   						printOpBinaire(expr->opLabel);
    					printExpr(getChild(expr, 1),depth+1);
    					break;

	case L_UNARYPLUS :	printf("\n+");
   						printExpr(getChild(expr, 0),depth+1);
    					break;

	case L_UNARYMINUS :	printf("\n-");
   						printExpr(getChild(expr, 0),depth+1);
    					break;

	case L_IFTHENELSE : printf("\nIF("); printExpr(getChild(expr, 0),depth+1); 		/* condition */
    					printf(")\nTHEN{\n"); printExpr(getChild(expr, 1),depth+1); /* 'then' part */
   						printf("}\nELSE{\n"); printExpr(getChild(expr, 2),depth+1); /* 'else' part */
   						printf("}\n");
    					break;

	case L_NEW : printf("NEW "); printClass(getChild(expr,0)->u.class); printExpr(getChild(expr,1),depth+1); break;
	case L_PARAMLIST : 	printExpr(getChild(expr,0),depth+1); if(expr->nbChildren>1) { printExpr(getChild(expr,1),depth+1); } break;
	
	/* --- AST LEAF --- */

	case L_LISTVAR : 	printf("Var Declarations : ");printVarList(expr->u.ListDecl); printf("\n"); break;
	case L_CONSTINT : 	printf("%d", expr->u.valInt); break;
	case L_CONSTSTR : 	printf("\"%s\"", expr->u.valStr); break;
	case L_ID :			printVarList(expr->u.ListDecl); break;
	case L_CLASS :		printClass(expr->u.class); break;
	case L_METHOD :		printMethod(expr->u.method); break;

	/* ---------------- */
	case L_BLOC :		printExpr(getChild(expr,0),depth+1); if(expr->nbChildren>1) { printExpr(getChild(expr,1),depth+1); } break;
	case L_LISTINST : 	printExpr(getChild(expr,0),depth+1); if(expr->nbChildren>1) { printExpr(getChild(expr,1),depth+1); } break;

	case L_SELECTION : printExpr(getChild(expr,0),depth+1); printf("."); printExpr(getChild(expr,1),depth+1); break;
	case L_MESSAGE : 	printExpr(getChild(expr,0),depth+1);
						printf(".");
						printExpr(getChild(expr,1),depth+1);
						printf("(");
						printExpr(getChild(expr,2),depth+1);
						printf(")");
						break;

	case L_CAST : printf("CAST : ");printExpr(getChild(expr,0),depth+1); printExpr(getChild(expr,1),depth+1); break;
	}
}

void printOpBinaire(char op) {
	switch(op) {
	case L_CONCAT : printf("&"); 	break;
	case L_EQ:    	printf("="); 	break;
	case L_NOTEQ:   printf("<>"); 	break;
	case L_SUP:    	printf(">"); 	break;
	case L_SUPEQ:   printf(">="); 	break;
	case L_INF:   	printf("<"); 	break;
	case L_INFEQ: 	printf("<="); 	break;
	case L_ADD:  	printf("+"); 	break;
	case L_SUB:		printf("-"); 	break;
	case L_MULT:	printf("*"); 	break;
	case L_DIV:  	printf("/"); 	break;
	case L_AFFECT: 	printf(":="); 	break;
	default:
		fprintf(stderr, "\nUnexpected binary operator of code: %d\n", op);
		exit(UNEXPECTED);
	}
}

void printClass(ClassP c){

	if(c == NIL(Class)){
		return;
	}

	if(c->isObject){
		printf("Object : %s",c->name);
	}
	else{
		printf("Class : %s",c->name);
	}


	ClassP superC = c->superClass;

	if(superC!=NIL(Class)){
		printf(" extends %s", superC->name);
	}

}

void printMethod(MethodP m){

	if(m == NIL(Method)){
		return;
	}

	if(m->redef){
		printf("[Override] ");
	}

	if(m->returnType==NIL(Class)){
		printf("void %s(",m->name);
	}
	else{
		printf("%s %s(",m->returnType->name,m->name);
	}

	printVarList(m->parameters);

	printf(")\n");

}

/**
 * Function that prints a class list (full)
 * TODO
 */
void printFullClassList(ClassDeclP list){

	/* Empty list */
	if(list == NIL(ClassDecl) || list->class == NIL(Class)){
		fprintf(stdout, "\n\nEnd of class list\n");
		return;
	}
	

	if(list->class->isObject){
		printf("\nObject : %s",list->class->name);
	}
	else{
		printf("\nClass : %s",list->class->name);
	}


	ClassP superC = list->class->superClass;

	if(superC!=NIL(Class)){
		printf(" extends %s", superC->name);
	}
	


	fprintf(stdout, "\nAttributes : ");
	printVarList(list->class->attributes);
	fprintf(stdout, "\nMethods :\n" );
	printMethodList(list->class->methods);


	printFullClassList(list->next);

}

void printClassList(ClassDeclP list){

	/* Empty list */
	if(list == NIL(ClassDecl) || list->class == NIL(Class)){
		fprintf(stdout, "End of class list\n");
		return;
	}
	
	printClass(list->class);
	printf("\n");
	printClassList(list->next);

}

void printMethodList(MethDeclP list){

	if(list==NIL(MethDecl)){
		printf("\n");
		return;
	}

	printf("\t");
	printMethod(list->method);

	printMethodList(list->next);

}

void printVarList(VarDeclP list){

	if(list==NIL(VarDecl)){
		/*printf("End of var list\n");*/
		return;
	}
	if(list->type == NIL(Class)){
		if(list->next!=NIL(VarDecl)){
			printf("UnknownType %s,",list->name);
		}
		else{
			printf("UnknownType %s",list->name);
		}
	}
	else{
		if(list->next!=NIL(VarDecl)){
			printf("%s %s,",list->type->name,list->name);
		}
		else{
			printf("%s %s",list->type->name,list->name);
		}
		
	}
	

	printVarList(list->next);

}

/********************************* Functions relative to the Variables (VarDecl struct) *********************************/

/**
 * ""Constructor"" for an Ident (variable)
 * @param name_param, the name of the Variable
 * @param nature_param, the nature of the variable => can be UNDEFINED if not known yet
 * @param type_param, the class type of the variable => can be N
 * @return the variable pointer
 */
VarDeclP ConstructVar(char * name_param,IdentNature nature_param, ClassP type_param, bool var_param){

	if(name_param == NIL(char)){
		fprintf(stderr, "ERROR : INVALID EMPTY VAR NAME !\n");
		exit(EXIT_FAILURE);
	}

	VarDeclP var = NEW(1,VarDecl);

	var->name = name_param;
	var->nature = nature_param;
	var->type = type_param;

	var->initialValue = NIL(Tree);

	var->isVar = var_param;

	var->next = NIL(VarDecl);

	return var;


}

/**
 * ""Constructor"" for an Ident (variable)
 * @param name_param, the name of the Variable
 * @param nature_param, the nature of the variable => can be UNDEFINED if not known yet
 * @param type_param, the class type of the variable
 * @param initValu_param, the value to initialise the variable with
 * @return the variable pointer
 */
VarDeclP ConstructInitialisedVar(char * name_param,IdentNature nature_param, ClassP type_param, TreeP initValue_param){

	if(name_param == NIL(char)){
		fprintf(stderr, "ERROR : INVALID EMPTY VAR NAME !\n");
		exit(EXIT_FAILURE);
	}

	VarDeclP var = NEW(1,VarDecl);

	var->name = name_param;
	var->nature = nature_param;
	var->type = type_param;

	var->initialValue = initValue_param;

	return var;


}

/**
 * Funcion to get a var into a list of var
 * @param list, the list search in
 * @param name, the name of the var o search
 * @return the pointer to the var or NIL(VaDecl)
 */
VarDeclP getVarInList(VarDeclP list, char* name){

	if(list == NIL(VarDecl)){
         return NIL(VarDecl);
    }


    /* We iterate to find the class into the chained list of class */
    VarDeclP p =list;
    while(p !=NIL(VarDecl)){

        /* if we find the var => return */
		if (strcmp(name, p->name) == 0){
			return p;
		}
		else{
			p = p->next;
		}

    }

    return NIL(VarDecl);


}


/********************************* Functions relative to the class struct *********************************/

/**
 * ""Constructor"" of a class structure
  * NOTE : All pointers can be NIL(...) if we don't have all the info at the construction
  * @param className_param, the name of the class (not NULL nor NIL(char) !!)
  * @param superClass_param, the classPointer of the superClass
  * @param constructor_param, the Method Pointer of the constructor
  * @param header_param, the variables chained List (VarDeclP) representing the header
  * @param attributes_param, the variables chained List (VarDeclP) representing the attributes
  * @param methods_param, the chained list of available methods in the class
  * @param bool predef_param,the bool for predef classes (Integer and String)
  * @param bool isObject_param, the bool for Object (Object in the langage not class)
  * @return the constructed class struct
 */
ClassP ConstructClass(char* className_param,ClassP superClass_param, MethodP constructor_param, VarDeclP header_param,
						VarDeclP attributes_param, MethDeclP methods_param, bool predef_param, bool isObject_param){

	if(className_param == NIL(char)){
		fprintf(stderr, "ERROR : INVALID EMPTY CLASSNAME !\n");
		exit(EXIT_FAILURE);
	}

	/* We construct the class */
	ClassP class = NEW(1,Class);

	class->name = className_param;
	class->superClass = superClass_param;
	class->constructor = constructor_param;
	class->header = header_param;
	class->attributes = attributes_param;

	class->methods = methods_param;

 	class->predef = predef_param;
	class->isObject = isObject_param;

	class->tmp = FALSE; /* Object is complete ! */
	class->alreadyMet=FALSE;

	/* We add the newly constructed class to our list of available classes */
	classList = addClassToList(classList, class);

	return class;
}

/**
 * Empty ""Constructor"" of a class structure
  * NOTE : All pointers are NIL(...) because we don't have all the info at the construction here
  * @param className_param, the name of the class (not NULL nor NIL(char) !!)
  * @return the incomplete constructed class struct
 */
ClassP IncompleteClassConstruct(char* className_param){

	if(className_param == NIL(char)){
		fprintf(stderr, "ERROR : INVALID EMPTY CLASSNAME !\n");
		exit(EXIT_FAILURE);
	}

	/* We construct the class */
	ClassP class = NEW(1,Class);

	class->name = className_param;
	class->superClass = NIL(Class);
	class->constructor = NIL(Method);
	class->header = NIL(VarDecl);
	class->attributes = NIL(VarDecl);

	class->methods = NIL(MethDecl);

 	class->predef = FALSE;
	class->isObject = FALSE;

	class->tmp = TRUE; /* Object is incomplete ! */
	class->alreadyMet=FALSE;

	/*Note : the class is not add to class list because not yet defined*/

	return class;

}


/**
 * Function to add an attribute to a class struct (to it's chained list of attributes)
 * @param class, the pointer representing the @ of the class we want to modify
 * @param var, the poiter to attribute to add to the class
 * @return nothing...
 */
void addAttribToClass(ClassP class, VarDeclP var){

	/* We iterate to go to the end of the chained list of attributes */
	VarDeclP* p = &(class->attributes);
	while((*p)!=NIL(VarDecl)){

		if((*p)->next==NIL(VarDecl)){
			break; /* break just before the last node (to rechain correctly) */
		}

		p=&((*p)->next);
	}

	/* we add our method to the list of attributes */
	(*p)->next=var;

}


/********************************* Functions relative to the list of class struct (ClassDecl) *********************************/

/**
 * Function to add a class to a list of class
 * @param list, the chained list of class
 * @param class, the class to add
 * @return the chained list
 */
ClassDeclP addClassToList(ClassDeclP list, ClassP class){

	/* Empty list */
    if(class==NIL(Class)){
        return list;
    }

    ClassDeclP node = NEW(1,ClassDecl);
    node->class = class;
    node->next = NIL(ClassDecl);

    /* we add our class to the list of classes */
    node->next = list;

    return node;
}

 /**
 * Function to find a class in a list of class
 * @param list, the chained list of class to search in
 * @param className, the string name of the class we are looking for
 * @return the class pointer (if found) or NIL(Class) (if not found)
 */
ClassP getClassInList(ClassDeclP list, char* className){

	/* if list not defined yet */
	if(list == NIL(ClassDecl)){
		return NIL(Class);
	}

	/* If no class defined yet */
	if(list->class==NIL(Class)){
		return NIL(Class);
	}

	/* If no className => not in list */
	if(className == NIL(char)){
		return NIL(Class);
	}

	/* We iterate to find the class into the chained list of class */
	ClassDeclP p = list;
	while(p!=NIL(ClassDecl)){

		/* if we find the class => return */
		if(strcmp(className, p->class->name) == 0){
			return p->class;
		}

		p=p->next;
	}

	return NIL(Class);

}


/********************************* Functions relative to the method struct *********************************/


/**
 * Method """Constructor"""
 * TODO JAVADOC
 */
MethodP ConstructMethod(char* methodName_param, VarDeclP parameters_param, ClassP owner_param,
						 ClassP returnType_param, TreeP body_param, bool redef_param){


	if(methodName_param == NIL(char)){
		fprintf(stderr, "ERROR : INVALID EMPTY METHOD NAME !\n");
		exit(EXIT_FAILURE);
	}

	MethodP m = NEW(1,Method);

	m->name = methodName_param;
	m->parameters = parameters_param;
	m->owner = owner_param;
	m->returnType = returnType_param;
	m->body = body_param;

	m->redef = redef_param;

	m->tmp = FALSE;

	return m;
}

/**
 * Method incomplete constructor => create incomplete methods (to be checked later on)
 * TODO JAVADOC
 */
MethodP IncompleteMethodConstruct(char* methodName_param){


	if(methodName_param == NIL(char)){
		fprintf(stderr, "ERROR : INVALID EMPTY METHOD NAME !\n");
		exit(EXIT_FAILURE);
	}

	MethodP m = NEW(1,Method);

	m->name = methodName_param;

	m->parameters = NIL(VarDecl);
	m->owner = NIL(Class);
	m->returnType = NIL(Class);
	m->body = NIL(Tree);

	m->redef = FALSE;

	m->tmp=TRUE;

	return m;
}


/********************************* Functions relative to the list of method struct (MethDecl) *********************************/


/**
 * Function that adds a method to a list of methods
 * @param list, the list where we will add our method
 * @param method, the pointer of the method to add
 * @return the chained list
 */
MethDeclP addMethodToList(MethDeclP list, MethodP method){

    /* Empty method case */
    if(method==NIL(Method)){
        return list;
    }

    MethDeclP node = NEW(1,MethDecl);
    node->method = method;
    node->next = NIL(MethDecl);

    /* we add our method to the list of methods */
    node->next = list;

    return node;
}


/* Function to find a method in a list of method
 * @param list, the list we are searching in
 * @param methodName, the name of the method we are looking for
 * @return, the method pointer (if found), NIL(Method) otherwhise
*/
MethodP getMethodInList(MethDeclP list, char* methodName){

	/* If the list is not defined yet */
	if(list==NIL(MethDecl)){
		return NIL(Method);
	}

	/* If no method in the list yet */
	if(list->method==NIL(Method)){
		return NIL(Method);
	}

	/* We iterate to find the class into the chained list of class */
	MethDeclP p = list;
	while(p!=NIL(MethDecl)){

		/* if we find the class => return */
		if(strcmp(methodName, p->method->name) == 0){
			return p->method;
		}
		p=p->next;
	}

	return NIL(Method);
}
