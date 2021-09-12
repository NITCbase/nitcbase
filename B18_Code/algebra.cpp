//
// Created by Jessiya Joy on 12/09/21.
//
#include <string>
#include <vector>
#include "define/constants.h"
#include "define/errors.h"
#include "disk_structures.h"
#include "algebra.h"
#include "OpenRelTable.h"

using namespace std;

int insert_val(vector<string> attributeTokens, char table_name[16]) {
	char ch;
	int relationId = OpenRelations::getRelationId(table_name);
	if (relationId == E_CACHEFULL || relationId == E_RELNOTEXIST || relationId == E_RELNOTOPEN) {
		//cout<<"Relation not open\n";
		return relationId;
	}
	Attribute relCatEntry[6];
	int y;
	y = getRelCatEntry(relationId, relCatEntry);
	if (y != SUCCESS) {
		//cout<<"Insert Failed\n";
		return y;
	}
	int count;
	count = relCatEntry[1].ival;
	int type[count];
	//getting the types of all attributes
	Attribute attr[6];
	int attr_blk = 5;
	char Attr_name[6][16];
	int j = 0;
	struct HeadInfo h;
	while (attr_blk != -1) {
		h = getheader(attr_blk);
		unsigned char slotmap[h.numSlots];
		getSlotmap(slotmap, attr_blk);
		for (int i = 0; i < 20; i++) {
			getRecord(attr, attr_blk, i);
			if ((char) slotmap[i] == '0') {
				continue;
			}
			if (strcmp(attr[0].sval, table_name) == 0) {
				type[attr[5].ival] = attr[2].ival;
			}
		}
		attr_blk = h.rblock;
	}
	//-----------------------------------------------------

	char record_array[count][16];
	if (attributeTokens.size() != count) {
		//cout<<"Mismatch in no of arguments\n";
		return E_NATTRMISMATCH;
	}
	//cout<<"hi\n";
	//cout<<count<<"\n";
	for (int i = 0; i < count; i++) {
		string x = attributeTokens[i];
		char p[16];
		int j;
		for (j = 0; j < 15 && j < x.size(); j++) {
			p[j] = x[j];
		}
		p[j] = '\0';
		strcpy(record_array[i], p);
	}

	union Attribute rec[count];
	for (int l = 0; l < count; l++) {
		if (type[l] == FLOAT) {
			// cout<<record_array[l]<<"\n";
			if (check_type(record_array[l]) == FLOAT || check_type(record_array[l]) == INT)
				rec[l].fval = atof(record_array[l]);
			else
				return E_ATTRTYPEMISMATCH;


			//cout<<rec[l].fval<<"\n";
		}
		if (type[l] == INT) {
			//cout<<record_array[l]<<"\n";
			if (check_type(record_array[l]) != INT)
				return E_ATTRTYPEMISMATCH;
			rec[l].ival = strtoull(record_array[l], NULL, 10);
			//cout<<rec[l].ival<<"\n";
		}
		if (type[l] == STRING) {
			//cout<<record_array[l]<<"\n";

			strcpy(rec[l].sval, record_array[l]);
			//cout<<rec[l].sval<<"\n";
		}
	}
	int r;
	//cout<<"Calling insert"<<endl;
	r = ba_insert(relationId, rec);
	if (r == SUCCESS) {
		return SUCCESS;
	} else {
		//cout<<"FAILURE\n";
		return FAILURE;
	}


}

int insert(char relname[ATTR_SIZE], int nAttrs, char record[][ATTR_SIZE]) {

	// get the relation's open relation id(let it be rel_id), using getRelId() method of Openreltable
	// if relation is not opened in Openreltable, return E_RELNOTOPEN
	int relid = getRelId(relname);
	if (relid == E_RELNOTOPEN)
		return relid;


	//get the no. of attributes present in relation, from RelcatEntry present in Openreltable(using getRelCatEntry() method).
	//if nAttrs!=no_of_attrs in relation, return E_NATTRMISMATCH
	union Attribute relcat_entry[6];
	getRelCatEntry(relid, relcat_entry);
	int noAttrs = relcat_entry[1].ival;
	if (noAttrs != nAttrs)
		return E_NATTRMISMATCH;

	union Attribute recval[nAttrs];

	/*iterate through 0 to nAttrs-1 :
		get the i'th attribute's AttrCatEntry (using getAttrcatEntry() of Openreltable )
	*/
	// let type=attrcatentry.attr_type;
	union Attribute attrcat_entry[6];
	recId prev_recid;
	prev_recid.block = -1;
	prev_recid.slot = -1;
	recId recid;
	char attrName[ATTR_SIZE];
	strcpy(attrName, "RelName");
	union Attribute a;
	strcpy(a.sval, OpenRelTable[relid]);
	for (int i = 0; i < nAttrs; i++) {

		recid = linear_search(ATTRCAT_RELID, attrName, a, EQ, &prev_recid);
		getRecord(attrcat_entry, recid.block, recid.slot);
		int type = attrcat_entry[2].ival;
		if (type == INT)
			recval[i].ival = stoi(record[i]);
		else if (type == FLOAT)
			recval[i].fval = stof(record[i]);
		else if (type == STRING)
			strcpy(recval[i].sval, record[i]);


	}

	int retval = ba_insert(relid, recval);
	return retval;

	//return retval
}