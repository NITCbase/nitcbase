#ifndef B18_CODE_SCHEMA_H
#define B18_CODE_SCHEMA_H
#include "define/constants.h"
#include "disk_structures.h"

int createRel(char relname[16], int nAttrs, char attrs[][ATTR_SIZE], int attrtypes[]);
int openRel(char RelName[16]);
int closeRel(int relid);
Attribute *make_relcatrec(char relname[16], int nAttrs, int nRecords, int firstBlock, int lastBlock);
Attribute* make_attrcatrec(char relname[ATTR_SIZE], char attrname[ATTR_SIZE], int attrtype, int rootBlock, int offset);
int getRelId(char relname[16]);

#endif //B18_CODE_SCHEMA_H