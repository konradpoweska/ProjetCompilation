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
extern ClassDeclP ClassList;


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


void printAST(TreeP decls, TreeP main){

	/* TODO */

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

	/* We add the newly constructed class to our list of available classes */
	addClassToList(ClassList, class);

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


MethodP ConstructMethod(char* methodName_param, VarDeclP parameters_param, ClassP owner_param,
						 ClassP returnType_param, TreeP body_param, bool redef_param){


	if(methodName_param == NIL(char)){
		fprintf(stderr, "ERROR : INVALID EMPTY METHOD NAME !\n");
		exit(EXIT_FAILURE);
	}

	/* TODO */
	return NIL(Method);
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
 * @param count, the number of methods to add
 * @param ...,the multiples MethodP to add
 * @return nothing...
 */
void addMethodsToList(MethDeclP list, int count, ...){

	va_list methods;

    va_start(methods, count); 				/* Requires the last fixed parameter (to get the address) */

	/* Iterates trough the methods in order to add them */
    for (int j = 0; j < count; j++) {
        addMethodToList(list,va_arg(methods, MethodP));
    }
    va_end(methods);

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
