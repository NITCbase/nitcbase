#ifndef B18_CODE_SCHEMA_H
#define B18_CODE_SCHEMA_H
#include "define/constants.h"

int createRel(char relname[16], int nAttrs, char attrs[][ATTR_SIZE], int attrtype[]);

#endif //B18_CODE_SCHEMA_H