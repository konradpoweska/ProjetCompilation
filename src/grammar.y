%token T_IF T_THEN T_ELSE T_CLASS T_EXTENDS T_VAR T_IS T_DEF T_OBJECT T_RETURN T_OVERRIDE T_NEW T_ADD T_SUB T_MULT T_DIV T_AFFECT T_COMP T_IDENTCLASS
%token <S> T_STRING
%token <I> T_CONST
%token <D> T_IDENT

%type <T> 


%left T_ADD T_SUB
%left T_MULT T_DIV
%nonassoc UNARY
%nonassoc T_COMP

%{
#include "struct.h"   

extern int yylex();
extern void yyerror(); 
%}

%%
Program : LDeclsOpt Bloc;
LDeclsOpt : Class LDeclsOpt | Object LDeclsOpt | ;
Class : T_CLASS T_IDENTCLASS '(' LParamOpt ')' ExtendsOpt T_IS '{' ListOptDecl DefConstruct ListOptMethod '}';
ExtendsOpt : T_EXTENDS T_IDENTCLASS | ;
LParamOpt : LParam | ;
LParam : Param','LParam|Param;
Param : T_VAR T_IDENT ':' T_IDENTCLASS;
DefConstruct : T_DEF T_IDENT '(' LParamOpt ')' ConstructSuper T_IS Bloc;
ConstructSuper : ':' T_IDENTCLASS '(' LParamOpt ')'| ;
ListOptDecl : Decl ListOptDecl| ;
Object : T_OBJECT T_IDENTCLASS T_IS '{' ListOptDecl DefConstruct ListOptMethod '}';
Decl : T_VAR T_IDENT ':' T_IDENTCLASS';';
ListOptMethod : Method ListOptMethod| ;
Method : Override T_DEF T_IDENT '('LParamOpt')'':'T_IDENTCLASS T_AFFECT Expression | Override T_DEF T_IDENT'('LParamOpt')' ClassOpt T_IS Bloc;
Override : T_OVERRIDE | ;
ClassOpt : ':' T_IDENTCLASS | ;
Expression : T_CONST | T_IDENT | T_STRING | '(' Expression ')' |'(' T_IDENTCLASS Expression ')' | Selection | Instantiation | Expression T_COMP Expression | Expression T_ADD Expression | Expression T_SUB Expression | Expression T_DIV Expression | Expression T_MULT Expression | T_ADD Expression %prec UNARY | T_SUB Expression %prec UNARY;


Selection : Instance'.'T_IDENT;
Instance : T_IDENT | Selection | Message;
Instantiation : T_NEW T_IDENTCLASS'('LParamOpt')';
Message : Instance'.'T_IDENT'('LParamOpt')';
ListInst : Instruction ListInst | Instruction;

Instruction : Expression';' | Bloc | T_RETURN';' | Affectation | Ifte;
Bloc : '{' ListOptInstruct '}' | '{' ListDeclVar T_IS ListInst '}';
ListOptInstruct : Instruction ListOptInstruct | ;
ListDeclVar : DeclVar ListDeclVar | DeclVar;
DeclVar : T_IDENT ':' T_IDENTCLASS AffectOpt';';
AffectOpt : T_AFFECT Expression | ;
Affectation : T_IDENT T_AFFECT Expression';';
Ifte : T_IF Expression T_THEN Instruction T_ELSE Instruction;
