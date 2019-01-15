#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include "structures.h"

/* Fonction AUXILIAIRE pour la construction d'arbre : renvoie un squelette
 * d'arbre pour 'nbChildren' fils et d'etiquette 'op' donnee. L'appelant
 * doit lui-même stocker ses fils dans la strucutre que MakeNode renvoie
 */
/** A FINIR 
 */
TreeP makeNode(int nbChildren, Label label) {
	TreeP tree = NEW(1, Tree);
	tree->Oplabel = label; tree->nbChildren = nbChildren;
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

/**
 * Method to add a method to a class struct (to it's chained list of methods)
 * @param class, the pointer representing the @ of the class we want to modify
 * @param method, the poiter to method to add to the class
 * @return nothing...
 */
void addMethodToClass(ClassP class, MethodP method){

	/* We create the node we want to insert */
	MethDeclP node = NEW(1,MethDecl);
	node->method=method;
	node->next=NIL(MethDecl);

	/* We iterate to go to the end of the chained list of methods */
	MethDeclP* p = &(class->methods);
	while((*p)->next!=NIL(MethDecl)){
		p=&((*p)->next);
	}

	/* we add our method to the list of method */
	(*p)->next=node;
	
}

/**
 * Method to add an attribute to a class struct (to it's chained list of methods)
 * @param class, the pointer representing the @ of the class we want to modify
 * @param var, the poiter to attribute to add to the class
 * @return nothing...
 */
void addAttribToClass(ClassP class, VarDeclP var){

	/* We iterate to go to the end of the chained list of methods */
	VarDeclP* p = &(class->attributes);
	while((*p)->next!=NIL(VarDecl)){
		p=&((*p)->next);
	}

	/* we add our method to the list of method */
	(*p)->next=var;
}

/*int main(){
	return 0;
}*/