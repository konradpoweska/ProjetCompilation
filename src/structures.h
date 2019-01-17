#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <stdarg.h> /* for undifined number of param */
#include <stdlib.h>
#include "common.h"


/* Definition of the labels for our AST */
/**
 *  /!\ WARNING /!\
 * In the first place (before contextual verif) all the names might just be interpreted as ID
 * (because we don't know yet what it refers (check the environment to know what it is))
 *  /!\ WARNING /!\
 */
typedef enum _Label{
	/* Arithmetic Operators (binary Operators) */
	L_ADD=1,
	L_SUB=2,
	L_MULT=3,
	L_DIV=4,

	/* Affectation Operator (binary Operator) */
	L_AFFECT=5,

	/* Comparison Operators (binary Operators) */
	L_NOTEQ=6,
	L_EQ=7,
	L_INF=8,
	L_INFEQ=9,
	L_SUP=10,
	L_SUPEQ=11,

	/* KeyWord */
	L_IFTHENELSE=12,	/* (ternary Operator (3child)) */
	L_NEW = 13,			/* Creation of a new instance of a class (2 children : 0=ClassName, 1=ListParam) */


	/* AST Leaf */
	L_LISTVAR = 14,		/* Label for a List of declaration of variables into our AST (list of decl is a leaf) */
	L_CONSTINT = 15,	/* Label for integer const */
	L_CONSTSTR = 16,	/* Label for string const */
	L_ID = 17,			/* Label for a variable */

	/* AST  Node */
	L_BLOC = 18,		/* Label for a bloc */
	L_LISTINST = 19,	/* Label for a list of instruction */
	L_SELECTION = 20,	/* Selection : ClassObjectName.attribute => 2 children*/
	L_MESSAGE = 21,		/* Message : ClassObjectName.methodName(ListArg) => 3 children */



} Label;


/* Definition of the different possible types of Ident */
typedef enum _IdentNature{
	GLOBALVAR,
	LOCALVAR,
	PARAMETER,
	ATTRIBUTE,
	THIS,
	SUPER,
	RESULT
} IdentNature;


 /**
  * Structure that stores a list of pairs (variable, type)
  * We will build list like { (x, Integer), (y, String), (c, MyClass) } that will be used to check scope of variables
  */
typedef struct _Decl{
	char *name;
	struct _Class* type;
	struct _Decl *next;
} VarDecl, *VarDeclP;


/**
 * struct that represents a Tree (node or leaf)
 */
typedef struct _Tree {
  Label Oplabel;         /* Label for the operator */
  short nbChildren; 	 /* number of children */
  union {
    char *valStr;      		 /* value of leaf if op = CSTSTR */
    int valInt;			     /* value of leaf if op = CSTINT */
	VarDeclP ListDecl;		 /* value of leaf if op = LISTVAR */

    struct _Tree **children; /* Tree of the children of the node */
  } u;
} Tree, *TreeP;

/**
 * Structure that represent a class (in our compilation programm)
 */
typedef struct _Class{

	char* name;					/* Name of the class */
	struct _Class* superClass;	/* Represents the super class */
	struct _Method* constructor;/* Represents the class constructor */
	VarDeclP header;			/* Header of the class */
	VarDeclP attributes;		/* List of attributes for the class */

	struct _MethDecl* methods;	/* Represent the list of methods of the class */

	bool predef;				/* Is the class a predefined class ? (if yes => no subClass)*/
	bool isObject;				/* Is the class an Object (static class) ?  (object cannot be derived (no SubClass)) */

} Class, *ClassP;

/**
 * List of Class
 * @param class, the class (structure) element
 * @param next; pointer to the next element of the list
 */
typedef struct _ClassDecl{

	ClassP class;
	struct _ClassDecl* next;

}ClassDecl, *ClassDeclP;

/**
 * Structure that represent a method (in our compilation programm)
 */
typedef struct _Meth{
	char* name;			 /* name of the method */
	VarDeclP parameters; /* List of Parameters */
	ClassP owner; 		 /* class that own the method */

	ClassP returnType;	 /* return type of the method */
	TreeP body;			 /* Body of the method */
	bool redef;			 /* Is the method a redefinition ? */

	/*TO be continued.....*/

}Method, *MethodP;

/**
 * List of Methods
 * @param method, the method (structure) element
 * @param next; pointer to the next element of the list
 */
typedef struct _MethDecl{

	MethodP method;
	struct _MethDecl* next;

}MethDecl, *MethDeclP;


/* Return Type used for Bison's actions
 * First attribute is necessary (for FLEX)
 * other attributes corresponds to the actions associated to the grammar production
 * things like $$ = $1
 * For example $$ = makeTree(...) => needs to have TreeP in this enum
 * etc.
 */
typedef union{
	char C;			/* alone char */
	char *S;		/* string */
  	int I;			/* int value */
  	VarDeclP D;		/* pairs of (variable, type) */
  	TreeP T;		/* AST */

	/*To be continued */

} YYSTYPE;


/* for Bison otherwhise YYSTYPE is an int ! */
#define YYSTYPE YYSTYPE


/***************************************************************************************************************************/

								/* Headers of the functions used with the structure */

/***************************************************************************************************************************/

/********************************* Functions relative to the AST (Tree struct) *********************************/

/* Functions used for the construction of the AST */
TreeP makeLeafStr(Label label, char *str); 	    		/* leaf (string value) */
TreeP makeLeafInt(Label label, int val);	            /* leaf (int value) */
TreeP makeTree(Label label, int nbChildren, ...);	    /* node of the tree */

/* Printing the AST */
void printAST(TreeP decls, TreeP main);


/********************************* Functions relative to the class struct *********************************/

/* ""Constructor"" of a class structure 
 * NOTE : All pointers can be NIL(...) if we don't have all the info at the construction 
 * NOTE2: _param = it is a parameter of the constructor (not a field of the struct yet) (for the implementation of the function)
 */
ClassP ConstructClass(char* className_param,ClassP superClass_param, MethodP constructor_param, VarDeclP header_param, 
						VarDeclP attributes_param, MethDeclP methods_param, bool predef_param, bool isObject_param);

void addMethodToClass(ClassP class, MethodP method);		/* function to add a method to a class */
void addMethodsToClass(ClassP class, MethDeclP methods);	/* function to add a list of method to a class */
void addAttribToClass(ClassP class, VarDeclP var);			/* function to add an attribute to a class */

/********************************* Functions relative to the list of class struct (ClassDecl) *********************************/

void addClassToList(ClassDeclP list, ClassP class);	/* function to add a class to a list of class */


/********************************* Functions relative to the method struct *********************************/

/* ""Constructor"" of a method structure 
 * NOTE : Only return type can be NIL(...) => all the rest must not be NIL(...) => MUST BE CHECK IN THE IMPLEMENTATION
 * NOTE2: _param = it is a parameter of the constructor (not a field of the struct yet) (for the implementation of the function)
 */
MethodP ConstructMethod(char* methodName_param, VarDeclP parameters_param, ClassP owner_param,
						 ClassP returnType_param, TreeP body_param, bool redef_param);


/********************************* Functions relative to the list of method struct (MethDecl) *********************************/

void addMethodToList(MethDeclP list, MethodP method);			/* function to add a method to a list of method */
void addMethodsToList(MethDeclP list, unsigned int count, ...);	/* function to add multiples methods to a list of method */



#endif /* end of include guard: STRUCTURES_H */
