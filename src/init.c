#include "init.h"

void _Init(){
	/* COMMENT INIT LES CLASSES PREDEF ?
	   QUEL TYPE DE RETOUR POUR LES CONSTRUCTEURS ?
	   QUELLE CLASSE POUR LE CHAMP DE LA CLASSE PREDEF ?
	   CLASSE PREDEF POSSEDE CHAMP ? */
	
	/* Predefined class */
	/* 		(className, SuperClass, Constructor,    Header    , Attributes, Methods, predef, Object) */
	Integer = ConstructClass("Integer",NIL(Class),NIL(Method),NIL(VarDecl), NIL(VarDecl),NIL(MethDecl), TRUE, FALSE);
	String  = ConstructClass("String",NIL(Class),NIL(Method),NIL(VarDecl), NIL(VarDecl),NIL(MethDecl), TRUE, FALSE);

	
}
