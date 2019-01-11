%token IF THEN ELSE CLASS EXTENDS VAR IS DEF OBJECT RETURN OVERRIDE NEW ADD SUB MULT DIV AFFECT IDENT CONST COMP

%left ADD SUB
%left MULT DIV
%nonassoc COMP

%%
Class : CLASS IDENT '(' LParamOpt ')' ExtendsOpt IS '{' ListOptDecl DefConstruct ListOptMethod '}'
ExtendsOpt : EXTENDS IDENT | ;
LParamOpt : LParam | ;
LParam : Param','LParam|Param;
Param : VAR IDENT ':' IDENT;
DefConstruct : DEF IDENT '(' LParamOpt ')' ConstructSuper IS '{' Bloc '}';
ConstructSuper : ':' IDENT '(' LParamOpt ')'| ;
ListOptDecl : Decl ListOptDecl| ;
Decl : VAR IDENT ':' IDENT';';
ListOptMethod : Method ListOptMethod| ;
Method : Override DEF IDENT '('LParamOpt')'':'IDENT AFFECT Expression | Override DEF IDENT'('LParamOpt')'IDENTClassOpt IS Bloc;
Override : OVERRIDE | ;
IDENTClassOpt : ':' IDENT | ;
Objet : OBJECT IDENT IS '{' ListOptDecl DefConstruct ListOptMethod '}';
Expression : Terminal | '(' Expression ')' |'(' IDENT Expression ')' | Selection | Instantiation | Message | Expression COMP Expression | Expression ADD Expression | Expression SUB Expression | Expression DIV Expression | Expression MULT Expression;
Selection : IDENT'.'IDENT | Selection'.'IDENT;
Instantiation : NEW IDENT'('LParamOpt')';
Message : Selection'('LParam')';
ListInst : Instruction ListInst | Instruction;
Terminal : CONST | IDENT;
Instruction : Expression';' | Bloc | RETURN';' | Affectation | Ifte;
Bloc : '{' ListOptInstruct '}' | '{' ListDeclVar IS ListInst '}';
ListOptInstruct : Instruction ListOptInstruct | ;
ListDeclVar : DeclVar ListDeclVar | DeclVar;
DeclVar : IDENT ':' IDENT AffectOpt';';
AffectOpt : AFFECT Expression | ;
Affectation : IDENT AFFECT Expression';';
Ifte : IF Expression THEN Instruction ELSE Instruction;
