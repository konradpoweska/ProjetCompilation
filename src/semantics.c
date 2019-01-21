#include "semantics.h"

ClassP getType(TreeP expr) {
  return NIL(Class);
}

bool derivesType(ClassP thisType, ClassP targetType) {
  return FALSE;
}
