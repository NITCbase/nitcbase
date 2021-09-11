//
// Created by Jessiya Joy on 18/08/21.
//

#ifndef B18_CODE_OPENRELTABLE_H
#define B18_CODE_OPENRELTABLE_H
#include "../define/constants.h"

extern char OpenRelTable[MAX_OPEN][16];

class OpenRelations {
	static char OpenRelTable[MAX_OPEN][16];

	int getRelationId(char relationName[16])
	{
	}

	int openRelation(char relationName[16])
	{
	}

	int closeRelation(int relationId)
	{
	}
};

#endif //B18_CODE_OPENRELTABLE_H
