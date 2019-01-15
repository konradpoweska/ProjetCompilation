#include <stdlib.h>

/**
 * NEW : Macro used for dynamic memory allocation.
 * @param howmany, number of object to create
 * @param type, type of the structure to create (not the pointer but the struct)
 * @return the new pointer of the structure
 */
#define NEW(howmany, type) (type *) calloc((unsigned) howmany, sizeof(type))

/**
 * NIL : Macro used for dynamic memory allocation.
 * @param type, type of the structure to create (not the pointer but the struct)
 * @return the NIL pointer corresponding to the structure
 */
#define NIL(type) (type *) 0

#define TRUE 1
#define FALSE 0
typedef int bool;


/* Error code */
#define NO_ERROR	0
#define USAGE_ERROR	1
#define LEXICAL_ERROR	2
#define SYNTAX_ERROR    3
#define CONTEXT_ERROR	4
#define EVAL_ERROR	5
#define UNEXPECTED	10

/* Definition of the labels for our AST */
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

	/* A FINIR LES MOTS CLEFS */

	L_CONSTINT = 255,	/* A FINIR */
	L_CONSTSTR = 256,
	L_ID = 512/* A FINIR */
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
 * struct that represents a Tree (node or leaf) 
 */
typedef struct _Tree {
  Label Oplabel;         /* Label for the operator */
  short nbChildren; 	/* number of children */
  union {
    char *name;      		 /* value of leaf if op = ID */
    struct _Class* type;     /* value of leaf if op = CST */
    struct _Tree **children; /* Tree of the children of the node */
  } u;
} Tree, *TreeP;

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
 * Structure that represent a class (in our compilation programm)
 */
typedef struct _Class{
	
	char* name;					/* Name of the class */
	struct _Class* superClass;	/* Represents the super class */
	struct _Method* constructor;/* Represents the class constructor */
	VarDeclP header;			/* Header of the class */
	VarDeclP attributes;		/* List of attributes for the class */
	
	struct _MethDecl* methods;			/* Represent the list of methods of the class */

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

/********************************* Methods relative to the AST (Tree struct) *********************************/

/* Methods used for the construction of the AST */
TreeP makeLeafStr(Label label, char *str); 	    		/* leaf (string value) */
TreeP makeLeafInt(Label label, int val);	            /* leaf (int value) */
TreeP makeTree(Label label, int nbChildren, ...);	    /* node of the tree */

/* Printing the AST */
void printAST(TreeP decls, TreeP main);



/********************************* Methods relative to the class struct *********************************/

void addMethodToClass(ClassP class, MethodP method);	/* method to add a method to a class */
void addAttribToClass(ClassP class, VarDeclP var);		/* method to add an attribute to a class */

