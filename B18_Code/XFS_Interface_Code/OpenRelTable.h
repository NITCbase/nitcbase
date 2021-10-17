//
// Created by Jessiya Joy on 18/08/21.
//

#ifndef B18_CODE_OPENRELTABLE_H
#define B18_CODE_OPENRELTABLE_H

#include "../define/constants.h"
#include "block_access.h"

typedef struct OpenRelTableMetaInfo{
	bool free;
	char relName[ATTR_SIZE];
} OpenRelTableMetaInfo;

class OpenRelTable {
//	static char OpenRelTable[MAX_OPEN][ATTR_SIZE];
	static OpenRelTableMetaInfo tableMetaInfo[MAX_OPEN];
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
