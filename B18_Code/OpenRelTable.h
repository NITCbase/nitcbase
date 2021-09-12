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
	static void initializeOpenRelationTable() {
		for (int i = 0; i < MAX_OPEN; i++) {
			if (i == RELCAT_RELID)
				strcpy(OpenRelTable[i], "RELATIONCAT");
			else if (i == ATTRCAT_RELID)
				strcpy(OpenRelTable[i], "ATTRIBUTECAT");
			else
				strcpy(OpenRelTable[i], "NULL");
		}
	}

	static int getRelationId(char relationName[ATTR_SIZE]) {
		for (int i = 0; i < MAX_OPEN; i++)
			if (strcmp(OpenRelTable[i], relationName) == 0)
				return i;
		return E_RELNOTOPEN;
	}

	static int getRelationName(int relationId, char relationName[ATTR_SIZE]) {
		if (relationId < 0 || relationId >= MAX_OPEN) {
			return E_OUTOFBOUND;
		}
		strcpy(relationName, OpenRelTable[relationId]);
		return 0;
	}

	static int openRelation(char relationName[ATTR_SIZE]) {
		Attribute relationCatalog[6];

		/* check if relation exists
		 *      for this check each entry in relation catalog
		 */
		int i;
		for (i = 0; i < SLOTMAP_SIZE_RELCAT_ATTRCAT; i++) {
			getRecord(relationCatalog, 4, i);
			if (strcmp(relationCatalog[0].sval, relationName) == 0) {
				break;
			}
		}

		// if relation does not exist
		if (i == SLOTMAP_SIZE_RELCAT_ATTRCAT) {
			return E_RELNOTEXIST;
		}

		/* check if relation is already open
		 *      if yes, return open relation id
		 *  otherwise search for a free slot in open relation table
		 */
		for (i = 0; i < MAX_OPEN; i++) {
			if (strcmp(relationName, OpenRelTable[i]) == 0) {
				return i;
			}
		}

		for (i = 0; i < MAX_OPEN; i++) {
			if (strcmp(OpenRelTable[i], "NULL") == 0) {
				strcpy(OpenRelTable[i], relationName);
				return i;
			}
		}

		// if open relation table is already full
		if (i == MAX_OPEN) {
			return E_CACHEFULL;
		}
	}

	static int closeRelation(int relationId) {
		if (relationId < 0 || relationId >= MAX_OPEN) {
			return E_OUTOFBOUND;
		}
		if (strcmp(OpenRelTable[relationId], "NULL") == 0) {
			return E_RELNOTOPEN;
		}
		strcpy(OpenRelTable[relationId], "NULL");
		return 0;
	}

	/*
	 * If yes, return 1
	 * else, 0
	 */
	static int checkIfRelationOpen(char relationName[ATTR_SIZE]) {
		for (auto relationIterator: OpenRelTable) {
			if (strcmp(relationIterator, relationName) == 0) {
				return 1;
			}
		}
		return 0;
	}

	static int checkIfRelationOpen(int relationId) {
		if (relationId < 0 || relationId >= MAX_OPEN) {
			return E_OUTOFBOUND;
		}
		if (strcmp(OpenRelTable[relationId], "NULL") == 0) {
			return 0;
		}
		else {
			return 1;
		}
	}
};

#endif //B18_CODE_OPENRELTABLE_H
