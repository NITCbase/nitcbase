#include "define/constants.h"
#include "define/errors.h"
#include "disk_structures.h"
#include "schema.h"
#include <string>

int check_duplicate_attributes(int nAttrs, char attrs[][ATTR_SIZE]);
Attribute *make_relcatrec(char relname[16], int nAttrs, int nRecords, int firstBlock, int lastBlock);

int createRel(char relname[16], int nAttrs, char attrs[][ATTR_SIZE], int attrtype[]) {
	Attribute attrval;
	strcpy(attrval.sval, relname);

	// prev_recid of starting
	recId prev_recid;
	prev_recid.block = -1;
	prev_recid.slot = -1;

	int flag;
	// TODO: flag = ba_search(0,relcatrec,"RelName", attrval, EQ, &prev_recid);
	if (flag == SUCCESS) {
		return E_RELEXIST; // Relation name already exists.
	}

	if (check_duplicate_attributes(nAttrs, attrs) == E_DUPLICATEATTR) {
		return E_DUPLICATEATTR;
	}

	Attribute *relcatrec = make_relcatrec(relname, nAttrs, 0, -1,
	                                      -1);   // Relcat Entry: relname, #attrs, #records, first_blk, #slots_per_blk

	// TODO: flag = ba_insert(0, relcatrec);

	if (flag != SUCCESS) {
		// TODO: ba_delete(relname);
		return flag;
	}

	for (int iter = 0; iter < nAttrs; iter++) {
		Attribute attrcatrec[6];      // Attrcat Entry : relname, attr_name, attr_type, primaryflag, root_blk, offset
		strcpy(attrcatrec[0].sval, relname);
		strcpy(attrcatrec[1].sval, attrs[iter]);
		attrcatrec[2].nval = attrtype[iter];
		attrcatrec[3].nval = -1;
		attrcatrec[4].nval = -1;
		attrcatrec[5].nval = iter;
		// TODO: flag = ba_insert(1,attrcatrec);
		if (flag != SUCCESS) {
			// TODO: ba_delete(relname);
			return flag;
		}
	}
	return SUCCESS;
}

Attribute *make_relcatrec(char relname[16], int nAttrs, int nRecords, int firstBlock, int lastBlock) {
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

int check_duplicate_attributes(int nAttrs, char attrs[][ATTR_SIZE]) {
	// Given a list of attributes, checks if duplicates present
	for (int i = 0; i < nAttrs; i++) {
		for (int j = i + 1; j < nAttrs; j++) {
			if (strcmp(attrs[i], attrs[j]) == 0) {
				return E_DUPLICATEATTR;  // distinct attributes having same name.
			}
		}
	}
	return 0;
}
