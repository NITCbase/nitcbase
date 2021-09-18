//
// Created by Jessiya Joy on 12/09/21.
//
#include <string>
#include <cstring>
#include "define/constants.h"
#include "define/errors.h"
#include "disk_structures.h"
#include "OpenRelTable.h"

char OpenRelations::OpenRelTable[MAX_OPEN][ATTR_SIZE];

void OpenRelations::initializeOpenRelationTable() {
	for (int i = 0; i < MAX_OPEN; i++) {
		if (i == RELCAT_RELID)
			strcpy(OpenRelTable[i], "RELATIONCAT");
		else if (i == ATTRCAT_RELID)
			strcpy(OpenRelTable[i], "ATTRIBUTECAT");
		else
			strcpy(OpenRelTable[i], "NULL");
	}
}

int OpenRelations::getRelationId(char relationName[ATTR_SIZE]) {
	for (int i = 0; i < MAX_OPEN; i++)
		if (strcmp(OpenRelTable[i], relationName) == 0)
			return i;
	return E_RELNOTOPEN;
}

int OpenRelations::getRelationName(int relationId, char relationName[ATTR_SIZE]) {
	if (relationId < 0 || relationId >= MAX_OPEN) {
		return E_OUTOFBOUND;
	}
	strcpy(relationName, OpenRelTable[relationId]);
	return SUCCESS;
}

int OpenRelations::openRelation(char relationName[ATTR_SIZE]) {
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

int OpenRelations::closeRelation(int relationId) {
	if (relationId < 0 || relationId >= MAX_OPEN) {
		return E_OUTOFBOUND;
	}
    if (relationId == RELCAT_RELID || relationId == ATTRCAT_RELID) {
        return E_INVALID;
    }
	if (strcmp(OpenRelTable[relationId], "NULL") == 0) {
		return E_RELNOTOPEN;
	}
	strcpy(OpenRelTable[relationId], "NULL");
	return SUCCESS;
}

int OpenRelations::checkIfRelationOpen(char relationName[ATTR_SIZE]) {
	for (auto relationIterator: OpenRelTable) {
		if (strcmp(relationIterator, relationName) == 0) {
			return SUCCESS;
		}
	}
	return FAILURE;
}

int OpenRelations::checkIfRelationOpen(int relationId) {
	if (relationId < 0 || relationId >= MAX_OPEN) {
		return E_OUTOFBOUND;
	}
	if (strcmp(OpenRelTable[relationId], "NULL") == 0) {
		return FAILURE;
	}
	else {
		return SUCCESS;
	}
}