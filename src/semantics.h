#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "structures.h"


///////////////////////////
//////// UTILITIES ////////
///////////////////////////

/* Functions to concat 2 variable environments.
 * Please use undoConcatEnv once you're done, to avoid errors in case of
 * multiple tree traversals.
 */

void concatEnv(VarDeclP* localEnv, const VarDeclP superEnv,
                VarDeclP** const undoBuff);

void undoConcatEnv(VarDeclP** const undoBuff);

/* Usage example:
VarDeclP localEnv, superEnv; // works with empty pointers
VarDeclP* undoBuff; // mandatory buffer

appendEnv(&localEnv, superEnv, &undoBuff);
// here localEnv 'contains" superEnv
undoAppendEnv(&undoBuff);
// and here it's back to normal
 */




/////////////////////////
//////// LINKERS ////////
/////////////////////////

/* Functions linking a temp class/method/var to its real definition.
 * The ClassP/MethodP/VarDeclP is passed by its adress.
 * If the objectP is temporary, it will try to match it with the its real
 * definition based on its name.
 * Prints error and exits if not found.
 * Is always returned the real, non-temporary definition.
*/

ClassP getClass(ClassP* class);

VarDeclP getVarDecl(VarDeclP* var, VarDeclP env);

MethodP getMethod(MethodP* method, ClassP class);




/////////////////////////////////////
//////// EXPRESSION CHECKING ////////
/////////////////////////////////////

/* Function to recursively check if tree is properly constructed,
 * including linking variables/methods/classes and scope checking.
 */

bool checkExpression(TreeP block, VarDeclP env);




////////////////////////////////
//////// CLASS CHECKING ////////
////////////////////////////////

bool checkClass(ClassP class);

bool checkAllClasses();




///////////////////////////////
//////// TYPE CHECKING ////////
///////////////////////////////

/* Functions to get type returned by an expression.
   WARNING : use checkExpression() before !
*/

ClassP getType(TreeP expr);
/* Returns the type (ClassP) returned by an expression */

bool derivesType(ClassP type, ClassP superType);
/* Returns TRUE if "type" is equal to or subtype of "superType" */

#endif
