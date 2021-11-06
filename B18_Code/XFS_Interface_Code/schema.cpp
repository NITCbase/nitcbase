#include "../define/constants.h"
#include "../define/errors.h"
#include "disk_structures.h"
#include "schema.h"
#include "block_access.h"
#include "OpenRelTable.h"
#include "BPlusTree.h"

#include <string>
#include <cstring>
#include <cstdlib>
#include <iostream>

int check_duplicate_attributes(int nAttrs, char attrs[][ATTR_SIZE]);

Attribute *make_relcatrec(char relname[16], int nAttrs, int nRecords, int firstBlock, int lastBlock);

Attribute *make_attrcatrec(char relname[ATTR_SIZE], char attrname[ATTR_SIZE], int attrtype, int rootBlock, int offset);

/*
 * Schema Layer function for Creating a Relation/Table from the given name and attributes
 */
// TODO : review whether relation should be opened after creation
int createRel(char relname[ATTR_SIZE], int nAttrs, char attrs[][ATTR_SIZE], int attrtypes[]) {

	int status = OpenRelTable::checkIfOpenRelTableHasFreeEntry();
	if (status == FAILURE) {
		std::cout << "createRel not possible as openRel failed\n";
		return E_CACHEFULL;
	}

	Attribute attrval;
	strcpy(attrval.sval, relname);
	/*
	 * Check if Relation name already exists
	 *      prev_recid {-1, -1} => search from starting block, starting slot for the record
	 */
	recId prev_recid;
	prev_recid.block = -1;
	prev_recid.slot = -1;
	Attribute existing_relcatrec[6];
	int flag;
	flag = ba_search(0, existing_relcatrec, "RelName", attrval, EQ, &prev_recid);
	if (flag == SUCCESS) {
		return E_RELEXIST;
	}

	if (check_duplicate_attributes(nAttrs, attrs) == E_DUPLICATEATTR) {
		return E_DUPLICATEATTR;
	}

	int relId = OpenRelTable::openRelation(relname);

	Attribute *relcatrec = make_relcatrec(relname, nAttrs, 0, -1,
	                                      -1);
	// Relcat Entry: relname, #attrs, #records, first_blk, #slots_per_blkflag
	flag = ba_insert(RELCAT_RELID, relcatrec);
	if (flag != SUCCESS) {
		ba_delete(relId);
		return flag;
	}

	for (int offset = 0; offset < nAttrs; offset++) {
		Attribute *attrcatrec = make_attrcatrec(relname, attrs[offset], attrtypes[offset], -1,
		                                        offset); // Attrcat Entry : relname, attr_name, attr_type, primaryflag, root_blk, offset
		flag = ba_insert(ATTRCAT_RELID, attrcatrec);

		if (flag != SUCCESS) {
			ba_delete(relId);
			return flag;
		}
	}
	closeRel(relId);
	return SUCCESS;
}

int deleteRel(char relname[ATTR_SIZE]) {
	// get the relation's open relation id
	int relId = OpenRelTable::getRelationId(relname);

	if (relId == RELCAT_RELID || relId == ATTRCAT_RELID) {
		return E_INVALID;
	}

	// if relation is not open return E_RELNOTOPEN - cannot be deleted
	if (relId == E_RELNOTOPEN)
		return E_RELNOTOPEN;

	int retval = ba_delete(relId);

	return retval;
}

int renameRel(char oldRelName[ATTR_SIZE], char newRelName[ATTR_SIZE]) {
	if (strcmp(oldRelName, "RELATIONCAT") == 0 || strcmp(oldRelName, "ATTRIBUTECAT") == 0) {
		return E_INVALID;
	}

	int relId = OpenRelTable::checkIfRelationOpen(oldRelName);
	if (relId >= 0 && relId < 12) {
		return E_RELOPEN;
	}

	int retVal = ba_renamerel(oldRelName, newRelName);
	return retVal;
}

int renameAtrribute(char relName[ATTR_SIZE], char oldAttrName[ATTR_SIZE], char newAttrName[ATTR_SIZE]) {
	if (strcmp(relName, "RELATIONCAT") == 0 || strcmp(relName, "ATTRIBUTECAT") == 0) {
		return E_INVALID;
	}

	int relId = OpenRelTable::checkIfRelationOpen(relName);
	if (relId >= 0 && relId < 12) {
		return E_RELOPEN;
	}

	int retVal = ba_renameattr(relName, oldAttrName, newAttrName);
	return retVal;
}

int openRel(char relationName[ATTR_SIZE]) {
	return OpenRelTable::openRelation(relationName);
}

int closeRel(char relationName[ATTR_SIZE]) {
	int relId = OpenRelTable::getRelationId(relationName);
	if(relId< 0 || relId >= MAX_OPEN)
		return E_RELNOTOPEN;
	return OpenRelTable::closeRelation(relId);
}

int closeRel(int relid) {
	return OpenRelTable::closeRelation(relid);
}

int createIndex(char *relationName, char *attrName){
	// get the src relation's open relation id, using getRelId() method of Openreltable.
	int relId = OpenRelTable::getRelationId(relationName);

	// if source not opened in open relation table, return E_RELNOTOPEN
	if(relId == E_RELNOTOPEN) {
		return E_RELNOTOPEN;
	}
	BPlusTree bPlusTree = BPlusTree(relId, attrName);
	int rootBlock = bPlusTree.getRootBlock();
	return SUCCESS;
}

int dropIndex(char *relationName, char *attrName){
	// get the src relation's open relation id, using getRelId() method of Openreltable.
	int relId = OpenRelTable::getRelationId(relationName);

	// if source opened in open relation table, return E_RELOPEN
	if(relId == E_RELNOTOPEN) {
		return E_RELNOTOPEN;
	}

	Attribute attrCatEntry[6];
	int flag = getAttrCatEntry(relId, attrName, attrCatEntry);
	// in case attribute does not exist
	if (flag != SUCCESS) {
		return E_ATTRNOTEXIST;
	}
	int rootBlock = (int) attrCatEntry[ATTRCAT_ROOT_BLOCK_INDEX].nval;
	if (rootBlock == -1) {
		return E_NOINDEX;
	}

	BPlusTree bPlusTree = BPlusTree(relId, attrName);
	int retVal =  bPlusTree.bPlusDestroy(rootBlock);

	if (retVal == SUCCESS) {
		attrCatEntry[ATTRCAT_ROOT_BLOCK_INDEX].nval = -1;
		setAttrCatEntry(relId, attrName, attrCatEntry);
	}

	return retVal;
}

/*gokul
 * Creates and returns a Relation Catalog Record Entry with the parameters provided as argument
 */
Attribute *make_relcatrec(char relname[ATTR_SIZE], int nAttrs, int nRecords, int firstBlock, int lastBlock) {
	Attribute *relcatrec = (Attribute *) malloc(sizeof(Attribute) * 6);
	int nSlotsPerBlock = (2016 / (16 * nAttrs + 1));
	strcpy(relcatrec[0].sval, relname);
	relcatrec[1].nval = nAttrs;
	relcatrec[2].nval = nRecords;
	relcatrec[3].nval = firstBlock;     // first block = -1, earlier it was 0
	relcatrec[4].nval = lastBlock;
	relcatrec[5].nval = nSlotsPerBlock;

	return relcatrec;
}

/*gokul
 * Creates and returns an Attrbiute Catalog Record Entry with the parameters provided as argument
 */
Attribute *make_attrcatrec(char relname[ATTR_SIZE], char attrname[ATTR_SIZE], int attrtype, int rootBlock, int offset) {
	int primaryFlag = -1; // presently unused
	Attribute *attrcatrec = (Attribute *) malloc(sizeof(Attribute) * 6);
	strcpy(attrcatrec[0].sval, relname);
	strcpy(attrcatrec[1].sval, attrname);
	attrcatrec[2].nval = attrtype;
	attrcatrec[3].nval = primaryFlag;
	attrcatrec[4].nval = rootBlock;
	attrcatrec[5].nval = offset;

	return attrcatrec;
}


/*gokul
 * Given a list of attributes, checks if Duplicates are present
 * Duplicates - Distinct attributes having same name
 */
int check_duplicate_attributes(int nAttrs, char attrs[][ATTR_SIZE]) {
	for (int i = 0; i < nAttrs; i++) {
		for (int j = i + 1; j < nAttrs; j++) {
			if (strcmp(attrs[i], attrs[j]) == 0) {
				return E_DUPLICATEATTR;
			}
		}
	}
	return 0;
}

