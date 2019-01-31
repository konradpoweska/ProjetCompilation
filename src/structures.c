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

void printAST(TreeP decls, TreeP main){

	/* TODO */

}

/********************************* Functions relative to the Variables (VarDecl struct) *********************************/

/**
 * ""Constructor"" for an Ident (variable)
 * @param name_param, the name of the Variable
 * @param nature_param, the nature of the variable => can be UNDEFINED if not known yet
 * @param type_param, the class type of the variable => can be N
 * @return the variable pointer
 */
VarDeclP ConstructVar(char * name_param,IdentNature nature_param, ClassP type_param){

	if(name_param == NIL(char)){
		fprintf(stderr, "ERROR : INVALID EMPTY VAR NAME !\n");
		exit(EXIT_FAILURE);
	}

	VarDeclP var = NEW(1,VarDecl);

	var->name = name_param;
	var->nature = nature_param;
	var->type = type_param;

	var->initialValue = NIL(Tree);

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

	/* If no class defined yet */
	if(list->name==NIL(char)){
		return NIL(VarDecl);
	}

	/* We iterate to find the class into the chained list of class */
	VarDeclP* p = &(list->next);
	while((*p)->next!=NIL(VarDecl)){

		/* if we find the class => return */
		if(strcmp(name, (*p)->name)){
			return (*p);
		}
		p=&((*p)->next);
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

	/* We add the newly constructed class to our list of available classes */
	addClassToList(classList, class);

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

	/*Note : the class is not add to class list because not yet defined*/

	return class;

}

/**
 * Function to add a method to a class struct (to it's chained list of methods)
 * @param class, the pointer representing the @ of the class we want to modify
 * @param method, the poiter to method to add to the class
 * @return nothing...
 */
void addMethodToClass(ClassP class, MethodP method){

	/* We add the method to the methods of the class */
	addMethodToList(class->methods, method);

}

/**
 * function to add a list of method to a class
 * @param class, the class struct that will get the method add
 * @param methods, the list of methods to add
 * @return nothing...
 */
void addMethodsToClass(ClassP class, MethDeclP methods){

	/* If not an empty list of methods*/
	if(methods!=NIL(MethDecl) && methods->method != NIL(Method)){

		/* We iterate and add those methods */
		MethDeclP* p =&(methods);
		while((*p)->next!=NIL(MethDecl)){
			addMethodToClass(class,(*p)->method);
			p=&((*p)->next);
		}
	}


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
	while((*p)->next!=NIL(VarDecl)){
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
 * @return nothing...
 */
void addClassToList(ClassDeclP list, ClassP class){

	/* Empty list */
	if(list->class == NIL(Class)){
		list->class = class;
		return;
	}

	/* We iterate to go to the end of the chained list of class */
	ClassDeclP* p = &(list->next);
	while((*p)->next!=NIL(ClassDecl)){
		p=&((*p)->next);
	}

	/* rechain the list corectly */
	ClassDeclP node = NEW(1,ClassDecl);
	node->class = class;
	node->next = NIL(ClassDecl);

	(*p)->next = node;
}

 /**
 * Function to find a class in a list of class
 * @param list, the chained list of class to search in
 * @param className, the string name of the class we are looking for
 * @return the class pointer (if found) or NIL(Class) (if not found)
 */
ClassP getClassInList(ClassDeclP list, char* className){

	/* If no class defined yet */
	if(list->class==NIL(Class)){
		return NIL(Class);
	}

	/* If no className => not in list */
	if(className == NIL(char)){
		return NIL(Class);
	}

	/* We iterate to find the class into the chained list of class */
	ClassDeclP* p = &(list->next);
	while((*p)->next!=NIL(ClassDecl)){

		/* if we find the class => return */
		if(strcmp(className, (*p)->class->name)){
			return (*p)->class;
		}
		p=&((*p)->next);
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
 * @return nothing...
 */
void addMethodToList(MethDeclP list, MethodP method){

	/* Empty list case */
	if(list->method==NIL(Method)){
		list->method = method;
		list->next = NIL(MethDecl);
		return;
	}

	/* We iterate to go to the end of the chained list of methods */
	MethDeclP* p = &(list);

	while((*p)->next!=NIL(MethDecl)){
		p=&((*p)->next);
	}

	MethDeclP node = NEW(1,MethDecl);
	node->method = method;
	node->next = NIL(MethDecl);

	/* we add our method to the list of methods */
	(*p)->next=node;

}

/**
 * Function to add multiples methods to a list of method
 * @param list, the list of Methods to modify
 * @param list2, the list of methods to add
 * @return nothing...
 */
void addMethodsToList(MethDeclP list, MethDeclP list2){

	/* Empty list cases */
	if(list2 == NIL(MethDecl)){
		return;	/* List to add is empty => nothing to do */
	}
	if(list == NIL(MethDecl)){
		list->method = list2->method;
		list->next = list2->next;
		return;	
	}
	
	/* We iterate to go to the end of the chained list of methods */
	MethDeclP* p = &(list);

	while((*p)->next!=NIL(MethDecl)){
		p=&((*p)->next);
	}

	MethDeclP node = NEW(1,MethDecl);
	node->method = list2->method;
	node->next = list2->next;

	/* we add our method to the list of methods */
	(*p)->next=node;


}

/* Function to find a method in a list of method
 * @param list, the list we are searching in
 * @param methodName, the name of the method we are looking for
 * @return, the method pointer (if found), NIL(Method) otherwhise
*/
MethodP getMethodInList(MethDeclP list, char* methodName){

	/* If no method in the list yet */
	if(list->method==NIL(Method)){
		return NIL(Method);
	}

	/* We iterate to find the class into the chained list of class */
	MethDeclP* p = &(list->next);
	while((*p)->next!=NIL(MethDecl)){

		/* if we find the class => return */
		if(strcmp(methodName, (*p)->method->name)){
			return (*p)->method;
		}
		p=&((*p)->next);
	}

	return NIL(Method);
}
