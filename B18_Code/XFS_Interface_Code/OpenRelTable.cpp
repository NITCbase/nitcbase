//
// Created by Jessiya Joy on 12/09/21.
//
#include <string>
#include <cstring>
#include "../define/constants.h"
#include "../define/errors.h"
#include "disk_structures.h"
#include "OpenRelTable.h"

OpenRelTableMetaInfo OpenRelTable::tableMetaInfo[MAX_OPEN];;

void OpenRelTable::initializeOpenRelationTable() {
	for (int i = 0; i < MAX_OPEN; i++) {
		if (i == RELCAT_RELID) {
			tableMetaInfo[i].free = OCCUPIED;
			strcpy(tableMetaInfo[i].relName, "RELATIONCAT");
		}
		else if (i == ATTRCAT_RELID) {
			tableMetaInfo[i].free = OCCUPIED;
			strcpy(tableMetaInfo[i].relName, "ATTRIBUTECAT");
		}
		else {
			tableMetaInfo[i].free = FREE;
			strcpy(tableMetaInfo[i].relName, "NULL");
		}
	}
}

int OpenRelTable::getRelationId(char relationName[ATTR_SIZE]) {
	for (int i = 0; i < MAX_OPEN; i++)
		if (tableMetaInfo[i].free == OCCUPIED && strcmp(tableMetaInfo[i].relName, relationName) == 0)
			return i;
	return E_RELNOTOPEN;
}

int OpenRelTable::getRelationName(int relationId, char relationName[ATTR_SIZE]) {
	if (relationId < 0 || relationId >= MAX_OPEN) {
		return E_OUTOFBOUND;
	}
	strcpy(relationName, tableMetaInfo[relationId].relName);
	return SUCCESS;
}

int OpenRelTable::openRelation(char relationName[ATTR_SIZE]) {
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
		if (tableMetaInfo[i].free == OCCUPIED && strcmp(relationName, tableMetaInfo[i].relName) == 0) {
			return i;
		}
	}

	for (i = 0; i < MAX_OPEN; i++) {
		if (tableMetaInfo[i].free == FREE) {
			strcpy(tableMetaInfo[i].relName, relationName);
			return i;
		}
	}

	// if open relation table is already full
	if (i == MAX_OPEN) {
		return E_CACHEFULL;
	}
}

int OpenRelTable::closeRelation(int relationId) {
	if (relationId < 0 || relationId >= MAX_OPEN) {
		return E_OUTOFBOUND;
	}
    if (relationId == RELCAT_RELID || relationId == ATTRCAT_RELID) {
        return E_INVALID;
    }
	if (tableMetaInfo[relationId].free == FREE) {
		return E_RELNOTOPEN;
	}
	tableMetaInfo[relationId].free = FREE;
	strcpy(tableMetaInfo[relationId].relName, "NULL");
	return SUCCESS;
}

int OpenRelTable::checkIfRelationOpen(char relationName[ATTR_SIZE]) {
	for (auto relationIterator: tableMetaInfo) {
		if (relationIterator.free == OCCUPIED && strcmp(relationIterator.relName, relationName) == 0) {
			return SUCCESS;
		}
	}
	return FAILURE;
}

int OpenRelTable::checkIfRelationOpen(int relationId) {
	if (relationId < 0 || relationId >= MAX_OPEN) {
		return E_OUTOFBOUND;
	}
	if (tableMetaInfo[relationId].free == FREE) {
		return FAILURE;
	}
	else {
		return SUCCESS;
	}
}