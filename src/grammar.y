%token T_IF T_THEN T_ELSE T_CLASS T_EXTENDS T_VAR T_IS T_DEF T_OBJECT T_RETURN T_OVERRIDE T_NEW T_ADD T_SUB T_MULT T_DIV T_AFFECT T_CONCAT
%token <S> T_STRING
%token <I> T_CONST
%token <S> T_IDENT
%token <S> T_IDENTCLASS
%token <C> T_COMP


%type <T>  Expression Selection Message ListInst Instruction Bloc Affectation Ifte Instance Instantiation ListOptInstruct AffectOpt LParamOpt LParam
%type <D> LParamDecl ParamDecl ListDeclVar DeclVar LParamDeclOpt ListOptDecl Decl
%type <Class> ExtendsOpt
%type <M> DefConstruct DefConstructObj Method 
%type <MList> ListOptMethod
%type <B> Override
%type <S> ClassOpt

%nonassoc T_COMP
%left T_CONCAT
%left T_ADD T_SUB
%left T_MULT T_DIV
%nonassoc UNARY


%{
	#include "string.h"
	#include "structures.h"
	#include "common.h"
	#include "semantics.h"

	#include "stdio.h"

	extern int yylex();
	extern void yyerror();
	extern ClassDeclP classList;

	IdentNature chooseNature(IdentNature defaultOption, char *target){

		IdentNature nature = defaultOption;

		if(strcmp(target, "result") == 0){
			nature = RESULT;
		} 
		else if(strcmp(target, "this") == 0){
			nature = THIS;
		} 
		else if(strcmp(target, "super") == 0){
			nature = SUPER;
		} 
		else {
			nature = defaultOption;
		}
		return nature;
	}

	ClassP typeForVariables(char* typeToFind){


		ClassP type = getClassInList(classList,typeToFind);	/* look for the class type */

			if(type == NIL(Class)){
				type = IncompleteClassConstruct(typeToFind);
			}

		return type;

	}

%}

%%
Program : LDeclsOpt Bloc { /*printFullClassList(classList); printExpr($2,0);*/ checkAllClasses(); checkExpression($2, NIL(VarDecl));};

LDeclsOpt : Class LDeclsOpt 
| Object LDeclsOpt 
| ;

Class : T_CLASS T_IDENTCLASS '(' LParamDeclOpt ')' ExtendsOpt T_IS '{' ListOptDecl DefConstruct ListOptMethod '}'	{
								/* 	(T_IDENTCLASS, ExtendsOpt, DefConstruct, LParamDeclOpt, ListOptDecl, ListOptMethod, predef, Object) */
								/* 		(className, SuperClass, Constructor,    Header    , Attributes, Methods, predef, Object) */
								ConstructClass($2,         $6,         $10,     $4,                 $9,     $11, FALSE, FALSE);};

ExtendsOpt : T_EXTENDS T_IDENTCLASS { 	
										ClassP type = typeForVariables($2);

										$$ = type /* We provide the class if it is already defined (temp class otherwhise*/;}
| {$$ = NIL(Class);};

LParamDeclOpt : LParamDecl {$$ = $1; /* return the head of the chained list */}
| {$$ = NIL(VarDecl); /* Nothing... */ };

LParamDecl : ParamDecl','LParamDecl {	$1->next = $3;		/* chaining the list */
										$$ = $1;		/* return the head of the chained list */}
| ParamDecl {$$ = $1;} ;

ParamDecl : T_VAR T_IDENT ':' T_IDENTCLASS {	

				IdentNature nature = chooseNature(PARAMETER, $2); /* Try to find the nature of the variable */

				ClassP type =  typeForVariables($4);

				VarDeclP var = ConstructVar($2, nature,type, TRUE); /* Construct the variable (with what we have) */
				$$ = var; /* Add to the AST the fact that we found a variable */}

| T_IDENT ':' T_IDENTCLASS {	
				IdentNature nature = chooseNature(PARAMETER, $1); /* Try to find the nature of the variable */

				ClassP type =  typeForVariables($3);

				VarDeclP var = ConstructVar($1, nature,type, FALSE); /* Construct the variable (with what we have) */
				$$ = var; /* Add to the AST the fact that we found a variable */};

LParamOpt : LParam  {$$ = $1; /* List of parameters (AST) */}	
| {$$ = NIL(Tree);};

LParam : Expression','LParam { $$ = makeTree(L_PARAMLIST,2,$1,$3); }
|Expression {$$ = makeTree(L_PARAMLIST,1,$1);};

/*-------------------------------------------------------------------------------------------------------*/

DefConstruct : T_DEF T_IDENTCLASS '(' LParamDeclOpt ')' ConstructSuper T_IS Bloc {
										ClassP tmpClass = IncompleteClassConstruct($2);	/* Build a temp version of the class (before linking) */
										$$ = ConstructMethod($2,$4,tmpClass,tmpClass,$8,FALSE); /* Construct the constructor with the current infos we have */};

ConstructSuper : ':' T_IDENTCLASS '(' LParamOpt ')'
| ;

ListOptDecl : Decl ListOptDecl {$1->next = $2; /* chaining the list */
								$$ = $1; 	/*Return the actual list*/}
| {$$ = NIL(VarDecl); /*End of list*/};

Decl : T_VAR T_IDENT ':' T_IDENTCLASS';'	{	IdentNature nature = chooseNature(ATTRIBUTE, $2);
							VarDeclP var = ConstructVar($2, nature, typeForVariables($4), TRUE);
							$$ = var;};


Object : T_OBJECT T_IDENTCLASS T_IS '{' ListOptDecl DefConstructObj ListOptMethod '}' {
								/* 	(T_IDENTCLASS, ..........., DefConstruct, LParamDeclOpt, ListOptDecl, ListOptMethod, predef, Object) */
								/* 		(className, SuperClass, Constructor,    Header    , Attributes, Methods, predef, Object) */
								ConstructClass($2,  NIL(Class),         $6,   NIL(VarDecl),        $5,     $7, FALSE, TRUE);};

DefConstructObj : T_DEF T_IDENTCLASS T_IS Bloc {
							ClassP tmpClass = IncompleteClassConstruct($2);	/* Build a temp version of the class (before linking) */
							$$ = ConstructMethod($2,NIL(VarDecl),tmpClass,tmpClass,$4,FALSE); /* Construct the constructor with the current infos we have */};


ListOptMethod : Method ListOptMethod {$$=addMethodToList($2,$1); /*Chaining the list*/}
| {$$ = NIL(MethDecl);};

Method : Override T_DEF T_IDENT '('LParamDeclOpt')'':'T_IDENTCLASS T_AFFECT Expression { 

						ClassP returnType = typeForVariables($8);	/* look for the return type */

						$$ = ConstructMethod($3, $5, NIL(Class), returnType, $10, $1); /* construct the method */}

| Override T_DEF T_IDENT'('LParamDeclOpt')' ClassOpt T_IS Bloc {

						ClassP returnType = getClassInList(classList,$7);	/* look for the return type */

						if($7 == NIL(char)){
							returnType = NIL(Class);					/*No return type*/
						}
						else if(returnType == NIL(Class)){
							returnType = IncompleteClassConstruct($7); /* return type not defined yet*/
						}

						$$ = ConstructMethod($3, $5, NIL(Class), returnType, $9, $1); /* construct the method */};

Override : T_OVERRIDE { $$ = TRUE;}
| {$$ = FALSE;};

ClassOpt : ':' T_IDENTCLASS {$$ = $2;}
| {$$ = NIL(char);};

Expression : Instance {$$ = $1;}
| Instantiation {$$ = $1;}
| Expression T_COMP Expression 	{$$ = makeTree($2, 2, $1, $3);}
| Expression T_ADD Expression 	{$$ = makeTree(L_ADD, 2, $1, $3);}
| Expression T_SUB Expression 	{$$ = makeTree(L_SUB, 2, $1, $3);}
| Expression T_DIV Expression 	{$$ = makeTree(L_DIV, 2, $1, $3);}
| Expression T_MULT Expression 	{$$ = makeTree(L_MULT, 2, $1, $3);}
| T_ADD Expression %prec UNARY 	{$$ = makeTree(L_UNARYPLUS, 1, $2);}
| T_SUB Expression %prec UNARY 	{$$ = makeTree(L_UNARYMINUS, 1, $2);}
| Expression T_CONCAT Expression{$$ = makeTree(L_CONCAT, 2, $1, $3);} ;

Selection : Instance'.'T_IDENT {$$ = makeTree(L_SELECTION, 2, $1, makeLeafIdent(L_ID,ConstructVar($3,chooseNature(ATTRIBUTE,$3),NIL(Class), FALSE)));
									/*AST with the selection, we create a var (of unknown type for the moment) that must be an attribute*/} ;

Instance : '('T_IDENTCLASS Expression ')' {

	/* For a cast : we try to find the class into cast (if already built)*/

	ClassP type = typeForVariables($2);

	/* For a cast : construct the tree */
	$$ = makeTree(L_CAST, 2, makeLeafClass(L_CLASS,type), $3);}

| T_IDENT {	IdentNature nature = chooseNature(UNDEFINED, $1);
			VarDeclP var = ConstructVar($1, nature, NIL(Class), FALSE);
			$$ = makeLeafIdent(L_ID, var);}

| T_CONST {$$ = makeLeafInt(L_CONSTINT, $1);}
| Selection {$$ = $1;}
| Message {$$ = $1;}
| T_STRING {$$ = makeLeafStr(L_CONSTSTR, $1);}
| '(' Expression')' {$$ = $2;};


Instantiation : T_NEW T_IDENTCLASS'('LParamOpt')' {

				/* For a new instance of a class => We search for the class (if already built) */

				ClassP type = typeForVariables($2);

				/* Construct the tree */
				$$ = makeTree(L_NEW,2,makeLeafClass(L_CLASS,type),$4);

};

Message : Instance'.'T_IDENT'('LParamOpt')' {$$ = makeTree(L_MESSAGE, 3, $1, makeLeafMethod(L_METHOD,IncompleteMethodConstruct($3)), $5);}
| T_IDENTCLASS'.'T_IDENT'('LParamOpt')' {

											/* static method call => We search for the class it belongs (if already built) */

											ClassP type = typeForVariables($1);

											/* Construct the tree */
											$$ = makeTree(L_MESSAGE, 3,
																		makeLeafClass(L_CLASS, type),
											 							makeLeafMethod(L_METHOD,IncompleteMethodConstruct($3)),
																		$5);};

ListInst : Instruction ListInst {$$ = makeTree(L_LISTINST, 2, $1, $2);}
| Instruction {$$ = makeTree(L_LISTINST, 1, $1);};

Instruction : Expression';' {$$ = $1;} 
| Bloc {$$ = $1;}
| T_RETURN';' {$$ = NIL(Tree);}
| Affectation {$$ = $1;}
| Ifte {$$ = $1;};

Bloc : '{' ListOptInstruct '}' {$$ = makeTree(L_BLOC, 1, $2);}   
| '{' ListDeclVar T_IS ListInst '}' {$$ = makeTree(L_BLOC, 2, makeLeafIdent(L_LISTVAR,$2), $4);};

ListOptInstruct : Instruction ListOptInstruct {$$ = makeTree(L_LISTINST, 2, $1, $2);}
| {$$ = NIL(Tree);};

ListDeclVar : DeclVar ListDeclVar {	$1->next = $2;		/* chaining the list */
									$$ = $1;		/* return the head of the chained list */}
| DeclVar {$$ = $1;};

DeclVar : T_IDENT ':' T_IDENTCLASS AffectOpt';' {	IdentNature nature = chooseNature(UNDEFINED, $1);

													ClassP type = typeForVariables($3);

													$$ = ConstructInitialisedVar($1, nature, type, $4);};

AffectOpt : T_AFFECT Expression { $$ = $2; }
| {$$ = NIL(Tree);};

Affectation : Instance T_AFFECT Expression';'{$$ = makeTree(L_AFFECT, 2, $1, $3);};

Ifte : T_IF Expression T_THEN Instruction T_ELSE Instruction {$$ = makeTree(L_IFTHENELSE, 3, $2, $4, $6);};
