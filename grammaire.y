Class : CLASS NomClasse '(' LParamOpt ')' ExtendsOpt IS '{' ListOptDecl DefContruct ListOptMethod '}'
ExtendsOpt : EXTENDS NomClasse | ;
LParamOpt : Param','LParamOpt|Param| ;
Param : VAR Nom ':' NomClasse | ;
DefConstruct : DEF NomClasse '(' LParamOut ')' ConstructSuper IS '{' Bloc '}';
ConstructSuper : ':' SuperClass '(' LParamOpt ')'| ;
ListOptDecl : Decl ListOptDecl| ;
Decl : VAR Nom ':' NomClasse';';
ListOptMethod : Method ListOptMethod| ;
Method : Override DEF Nom '('ListParamOpt')'':'NomClasse AFFECT Expression | Override DEF Nom'('ListParamOpt')'NomClassOpt IS Bloc;
Override : OVERRIDE | ;
NomClassOpt : ':' NomClasse | ;
NomClasse : IDENT;
Objet : OBJECT Nom IS '{' ListOptDecl DefConstruct ListOptMethod '}';
Expression : IDENT | CONST | '(' Expression ')' |'(' NomClasse Expression ')' | Selection | Instantiation | Message | ExpressionOperateur;
Selection : Expression'.'Nom;
CONST : Integer | String; //?CONST IS ALREADY A TOKEN, AND AS SUCH IS TERMINAL? 
Instantiation : NEW NomClass'('ListParamOpt')';
Message : IDENT'.'IDENT'('ListExpression')';
ListExpression : Expression ListExpression | ;
ExpressionOperateur : '('OperateurComp')' E | '('OperateurCalcul')' G;
E : E COMP E; //?COMP?
G : G ADD H | G SUB H | H;
H : H MULT J | H DIV J | J;
J : IDENT | '(' E ')';
Instruction : Expression';' | Bloc | RETURN';' | Affectation | Ifte;
Bloc : '{' ListOptInstruct '}' | '{' ListDeclVar IS ListInst '}';
ListOptInstruct : Instruction ListOptInst | ;
ListDeclVar : DeclVar ListDeclVar | DeclVar;
DeclVar : Nom ':' NomClass ExpressionOpt;
ExpressionOpt : Expression | ;//? AFFECT ?
Affectation : Cible AFFECT Expression';';
Cible : IDENT;
Ifte : IF Expression THEN Instruction ELSE Instruction;//ELSE NOT OPTIONNAL
