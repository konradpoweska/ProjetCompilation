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
