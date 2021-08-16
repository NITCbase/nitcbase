#include "define/constants.h"
#include "define/errors.h"
#include "disk_structures.h"
#include "schema.h"
#include <string>

int createRel(char relname[16], int nAttrs, char attrs[][ATTR_SIZE], int attrtype[]) {
	union Attribute attrval;
	union Attribute relcat_rec[6];
	strcpy(attrval.sval, relname);

	union Attribute relcatrec[6];   // Relcat Entry: relname, #attrs, #records, first_blk, #slots_per_blk

	recId prev_recid;
	prev_recid.block = -1;
	prev_recid.slot = -1;

	int flag;
	// TODO: flag = ba_search(0,relcatrec,"RelName", attrval, EQ, &prev_recid);

	if (flag == SUCCESS) {
		return E_RELEXIST; //Relation name already exists.
	}

	for (int iter = 0; iter < nAttrs; iter++) {
		int iter2;
		for (iter2 = iter + 1; iter2 < nAttrs; iter2++) {
			if (strcmp(attrs[iter], attrs[iter2]) == 0) {
				return E_DUPLICATEATTR;  //distinct attributes having same name.
			}
		}
	}
	strcpy(relcatrec[0].sval, relname);
	relcatrec[1].nval = nAttrs;
	relcatrec[2].nval = 0;
	relcatrec[3].nval = -1;//first block=-1 ,earlier it was 0
	relcatrec[4].nval = -1;
	relcatrec[5].nval = (2016 / (16 * nAttrs + 1));

	// TODO: flag = ba_insert(0,relcatrec);

	if (flag != SUCCESS) {
		// TODO: ba_delete(relname);
		return flag;
	}

	for (int iter = 0; iter < nAttrs; iter++) {
		union Attribute attrcatrec[6];      // Attrcat Entry : relname, attr_name, attr_type, primaryflag, root_blk, offset
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
