//
// Created by Jessiya Joy on 18/08/21.
//

#ifndef B18_CODE_OPENRELTABLE_H
#define B18_CODE_OPENRELTABLE_H

#include "../define/constants.h"
#include "block_access.h"

//extern char OpenRelTable[MAX_OPEN][16];

class OpenRelations {
	static char OpenRelTable[MAX_OPEN][ATTR_SIZE];

public:

	static void initializeOpenRelationTable();
	static int getRelationId(char relationName[ATTR_SIZE]);
	static int getRelationName(int relationId, char relationName[ATTR_SIZE]);
	static int openRelation(char relationName[ATTR_SIZE]);
	static int closeRelation(int relationId);
	static int checkIfRelationOpen(char relationName[ATTR_SIZE]);
	static int checkIfRelationOpen(int relationId);
};

#endif //B18_CODE_OPENRELTABLE_H
