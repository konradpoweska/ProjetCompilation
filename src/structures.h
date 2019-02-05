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
	L_CONCAT,
	L_ADD,
	L_SUB,
	L_MULT,
	L_DIV,

	/* Affectation Operator (binary Operator) */
	L_AFFECT,		/* Affectation, 2 children : 1 = TreeP (Instance), 2= TreeP (Expression) */

	/* Comparison Operators (binary Operators) */
	L_NOTEQ,
	L_EQ,
	L_INF,
	L_INFEQ,
	L_SUP,
	L_SUPEQ,

	/* Unary Operator */
	L_UNARYPLUS,
	L_UNARYMINUS,

	/* KeyWord */
	L_IFTHENELSE,	/* If then else,  3 children : 1 = TreeP, 2 = TreeP, 3 = TreeP */
	L_NEW,			/* Creation of a NEW INSTANCE of a class, 2 children : 1=TreeP (leaf(Class)), 2=TreeP (ListParam) */


	L_PARAMLIST,	/* list of parameters of a function call (1 or 2 children : 1=Param(TreeP), (2=ListParam(TreeP)) ) */

	/* AST Leaf (TreeP) */
	L_LISTVAR,		/* Label for a List of declaration of variables into our AST (list of decl is a leaf) */
	L_CONSTINT,		/* Label for an integer const 	*/
	L_CONSTSTR,		/* Label for a string const 	*/
	L_ID,			/* Label for a variable 		*/
	L_CLASS,		/* Label for a class leaf 		*/
	L_METHOD,		/* Label for a method leaf 		*/

	/* AST  Node */

	L_BLOC,			/* Label for a bloc : 1 or 2 children 1 = TreeP, (2 = TreeP)
													either 1 = TreeP (leaf(VarDeclP) = ListDeclarations) and 2 = TreeP (ListInstructions)
													either 1 = TreeP (ListInstructions) */

	L_LISTINST,		/* Label for a list of instruction, 1 or 2 Children : 1 = TreeP (Instruction), (2 = TreeP (ListInst)) */

	L_SELECTION,	/* Selection : 2 children : 1=TreeP (Instance rule), 2=TreeP (leaf(VarDeclP))*/
	L_MESSAGE,		/* Message : => 3 children :  1 = TreeP, 2 = TreeP, 3 = TreeP but different meanings :
													either : 1 = TreeP (Instance), 2 = TreeP (leaf(Method)), 3 = TreeP (ListParam)
													either : 1 = TreeP (leaf(Class)), 2 = TreeP (leaf(Method)), 3 = TreeP (ListParam) */

	L_CAST,			/* Cast : (ClassName Expression) => AST with 2 children : 1=TreeP (is a leaf(Class)), 2=TreeP */





} Label;


/* Definition of the different possible types of Ident */
typedef enum _IdentNature{
	UNDEFINED,
	GLOBALVAR,		/* Variable of the main programm bloc */
	LOCALVAR,		/* Variable of a local bloc (ex : function call) */
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
	IdentNature nature;

	bool isVar;

	struct _Class* type;

	struct _Tree* initialValue;

	struct _Decl *next;

} VarDecl, *VarDeclP;


/**
 * struct that represents a Tree (node or leaf)
 */
typedef struct _Tree {
  Label opLabel;         /* Label for the operator */
  short nbChildren; 	 /* number of children */
  union {
    char *valStr;      		 /* value of leaf if op = CSTSTR */
    int valInt;			     /* value of leaf if op = CSTINT */
	VarDeclP ListDecl;		 /* value of leaf if op = LISTVAR */
	struct _Class* class;	 /* value of one leaf if op = L_NEW */
	struct _Meth* method;	 /* value of one leaf if op = L_MESSAGE */

    struct _Tree **children; /* Tree of the children of the node */
  } u;
} Tree, *TreeP;

/**
 * Structure that represent a class (in our compilation programm)
 */
typedef struct _Class{

	char* name;					/* Name of the class */
	struct _Class* superClass;	/* Represents the super class */
	struct _Meth* constructor;	/* Represents the class constructor */
	VarDeclP header;			/* Header of the class */
	VarDeclP attributes;		/* List of attributes for the class */

	struct _MethDecl* methods;	/* Represent the list of methods of the class */

	bool predef;				/* Is the class a predefined class ? (if yes => no subClass)*/
	bool isObject;				/* Is the class an Object (static class) ?  (object cannot be derived (no SubClass)) */

	bool tmp;			/*Is the class a temporary found name ? (before the declaration) */
	bool alreadyMet;	/* For context verif : do we have already met the class ? */

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

	bool tmp;

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
	char C;				/* alone char */
	char *S;			/* string */
  	int I;				/* int value */

	bool B;

  	VarDeclP D;			/* pairs of (variable, type) */
  	ClassP Class;		/* class pointer */
  	MethodP M;			/* method */
  	MethDeclP MList;	/* List of method */
  	TreeP T;			/* AST */

	/*To be continued */

} YYSTYPE;


/* for Bison otherwhise YYSTYPE is an int ! */
#define YYSTYPE YYSTYPE


/***************************************************************************************************************************/

								/* Headers of the functions used with the structure */

/***************************************************************************************************************************/

/********************************* Functions relative to the AST (Tree struct) *********************************/

/* Functions used for the construction of the AST */
TreeP makeLeafStr(Label label, char *str); 	    		/* leaf (string value) 	*/
TreeP makeLeafInt(Label label, int val);	            /* leaf (int value) 	*/
TreeP makeLeafIdent(Label label, VarDeclP ident);		/* leaf (variable) 		*/
TreeP makeLeafClass(Label label, ClassP class);			/* leaf (class) 		*/
TreeP makeLeafMethod(Label label, MethodP method);		/* leaf (method) 		*/
TreeP makeTree(Label label, int nbChildren, ...);	    /* node of the tree 	*/

/* return child N°i (i goes from 0 to N-1) */
TreeP getChild(TreeP tree, int i);

/* Printing the AST and lists */
void printAST(TreeP main);
void printExpr(TreeP expr, int depth);
void printOpBinaire(char op);
void printClass(ClassP c);
void printMethod(MethodP m);
void printFullClassList(ClassDeclP list);
void printClassList(ClassDeclP list);
void printMethodList(MethDeclP list);
void printVarList(VarDeclP list);



/********************************* Functions relative to the Variables (VarDecl struct) *********************************/

/* ""Constructor"" for an Ident (variable) */
VarDeclP ConstructVar(char * name_param,IdentNature nature_param, ClassP type_param, bool var_param);
VarDeclP ConstructInitialisedVar(char * name_param,IdentNature nature_param, ClassP type_param, TreeP initValue_param);

/* Method to get a var into a list of var */
VarDeclP getVarInList(VarDeclP list, char* name);


/********************************* Functions relative to the class struct *********************************/

/* ""Constructor"" of a class structure
 * NOTE : All pointers can be NIL(...) if we don't have all the info at the construction
 * NOTE2: _param = it is a parameter of the constructor (not a field of the struct yet) (for the implementation of the function)
 */
ClassP ConstructClass(char* className_param,ClassP superClass_param, MethodP constructor_param, VarDeclP header_param,
						VarDeclP attributes_param, MethDeclP methods_param, bool predef_param, bool isObject_param);

ClassP IncompleteClassConstruct(char* className_param);		/* Construct an temporary class (use to check if the class is defined) */

/********************************* Functions relative to the list of class struct (ClassDecl) *********************************/

ClassDeclP addClassToList(ClassDeclP list, ClassP class);	 /* function to add a class to a list of class */
ClassP getClassInList(ClassDeclP list, char* className); /* function to find a class in a list of class */


/********************************* Functions relative to the method struct *********************************/

/* ""Constructor"" of a method structure
 * NOTE : Only return type can be NIL(...) => all the rest must not be NIL(...) => MUST BE CHECK IN THE IMPLEMENTATION
 * NOTE2: _param = it is a parameter of the constructor (not a field of the struct yet) (for the implementation of the function)
 */
MethodP ConstructMethod(char* methodName_param, VarDeclP parameters_param, ClassP owner_param,
						 ClassP returnType_param, TreeP body_param, bool redef_param);

MethodP IncompleteMethodConstruct(char* methodName_param); /* Construct an temporary method (use to check if the method is defined) */

/********************************* Functions relative to the list of method struct (MethDecl) *********************************/

MethDeclP addMethodToList(MethDeclP list, MethodP method);			/* function to add a method to a list of method */
MethodP getMethodInList(MethDeclP list, char* methodName); 		/* function to find a method in a list of method */

#endif /* end of include guard: STRUCTURES_H */
