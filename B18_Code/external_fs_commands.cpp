//
// Created by Jessiya Joy on 19/08/21.
//

#include <iostream>
#include <cstdio>
#include <cstring>
#include "external_fs_commands.h"
#include "disk_structures.h"
#include "block_access.h"
#include "OpenRelTable.h"
#include "algebra.h"
#include "schema.h"

using namespace std;

void writeHeaderFieldToFile(FILE *fp, int32_t headerField);

void writeHeaderToFile(FILE *fp_export, HeadInfo h);

void writeAttributeToFile(FILE *fp, Attribute attribute, int type, int lastLineFlag);


void dump_relcat() {
	FILE *fp_export = fopen("relation_catalog", "w");
	Attribute relCatRecord[ATTR_SIZE];

	HeadInfo headInfo;

	// write header of block 4 to disk
	headInfo = getHeader(RELCAT_BLOCK);
	writeHeaderToFile(fp_export, headInfo);

	unsigned char slotmap[headInfo.numSlots];
	getSlotmap(slotmap, RELCAT_BLOCK);

	for (int slotNum = 0; slotNum < SLOTMAP_SIZE_RELCAT_ATTRCAT; slotNum++) {
		unsigned char ch = slotmap[slotNum];
		fputc(ch, fp_export);
	}
	fputs("\n", fp_export);

	for (int slotNum = 0; slotNum < SLOTMAP_SIZE_RELCAT_ATTRCAT; slotNum++) {
		getRecord(relCatRecord, RELCAT_BLOCK, slotNum);

		if ((char) slotmap[slotNum] == SLOT_UNOCCUPIED)
			strcpy(relCatRecord[0].sval, "NULL");

		// RelationName
		writeAttributeToFile(fp_export, relCatRecord[0], STRING, 0);
		// #Attributes
		writeAttributeToFile(fp_export, relCatRecord[1], NUMBER, 0);
		// #Records
		writeAttributeToFile(fp_export, relCatRecord[2], NUMBER, 0);
		// FirstBlock
		writeAttributeToFile(fp_export, relCatRecord[3], NUMBER, 0);
		// LastBlock
		writeAttributeToFile(fp_export, relCatRecord[4], NUMBER, 0);
		// #SlotsPerBlock
		writeAttributeToFile(fp_export, relCatRecord[5], NUMBER, 1);
	}

	fclose(fp_export);
}

void dump_attrcat() {
	FILE *fp_export = fopen("attribute_catalog", "w");
	Attribute attrCatRecord[ATTR_SIZE];
	int attrCatBlock = ATTRCAT_BLOCK;

	HeadInfo headInfo;

	while (attrCatBlock != -1) {
		headInfo = getHeader(attrCatBlock);
		writeHeaderToFile(fp_export, headInfo);

		unsigned char slotmap[headInfo.numSlots];
		getSlotmap(slotmap, attrCatBlock);
		for (int slotNum = 0; slotNum < SLOTMAP_SIZE_RELCAT_ATTRCAT; slotNum++) {
			unsigned char ch = slotmap[slotNum];
			fputc(ch, fp_export);
		}
		fputs("\n", fp_export);

		for (int slotNum = 0; slotNum < SLOTMAP_SIZE_RELCAT_ATTRCAT; slotNum++) {

			getRecord(attrCatRecord, attrCatBlock, slotNum);
			if ((char) slotmap[slotNum] == SLOT_UNOCCUPIED) {
				strcpy(attrCatRecord[0].sval, "NULL");
				strcpy(attrCatRecord[1].sval, "NULL");
			}

			// RelationName
			writeAttributeToFile(fp_export, attrCatRecord[0], STRING, 0);
			// AttributeName
			writeAttributeToFile(fp_export, attrCatRecord[1], STRING, 0);
			// AttributeType
			writeAttributeToFile(fp_export, attrCatRecord[2], NUMBER, 0);
			// PrimaryFlag
			writeAttributeToFile(fp_export, attrCatRecord[3], NUMBER, 0);
			// RootBlock
			writeAttributeToFile(fp_export, attrCatRecord[4], NUMBER, 0);
			// Offset
			writeAttributeToFile(fp_export, attrCatRecord[5], NUMBER, 1);
		}
		attrCatBlock = headInfo.rblock;
	}
	fclose(fp_export);
}

void dumpBlockAllocationMap() {
	FILE *disk = fopen("disk", "rb+");
	fseek(disk, 0, SEEK_SET);
	unsigned char blockAllocationMap[4 * BLOCK_SIZE];
	fread(blockAllocationMap, 4 * BLOCK_SIZE, 1, disk);
	fclose(disk);

	int blockNum;
	char s[16];
	FILE *fp_export = fopen("block_allocation_map", "w");
	for (blockNum = 0; blockNum < 4; blockNum++) {
		fputs("Block ", fp_export);
		sprintf(s, "%d", blockNum);
		fputs(s, fp_export);
		fputs(": Block Allocation Map\n", fp_export);
	}
	for (blockNum = 4; blockNum < 8192; blockNum++) {
		fputs("Block ", fp_export);
		sprintf(s, "%d", blockNum);
		fputs(s, fp_export);
		if ((int32_t) (blockAllocationMap[blockNum]) == UNUSED_BLK) {
			fputs(": Unused Block\n", fp_export);
		}
		if ((int32_t) (blockAllocationMap[blockNum]) == REC) {
			fputs(": Record Block\n", fp_export);
		}
		if ((int32_t) (blockAllocationMap[blockNum]) == IND_INTERNAL) {
			fputs(": Internal Index Block\n", fp_export);
		}
		if ((int32_t) (blockAllocationMap[blockNum]) == IND_LEAF) {
			fputs(": Leaf Index Block\n", fp_export);
		}
	}

	fclose(fp_export);
}

void ls() {
	Attribute relCatRecord[6];
	int attr_blk = 4;
	struct HeadInfo headInfo;
	headInfo = getHeader(attr_blk);
	unsigned char slotmap[headInfo.numSlots];
	getSlotmap(slotmap, attr_blk);
	for (int i = 0; i < 20; i++) {
		getRecord(relCatRecord, attr_blk, i);
		if ((char) slotmap[i] == SLOT_OCCUPIED)
			std::cout << relCatRecord[0].sval << "\n";
	}
	std::cout << "\n";
}

int importRelation(char *filename) {
	FILE *file = fopen(filename, "r");
	//attr : first line in the file
	//numAttrs : no of attributes in the file
	char *attr = (char *) malloc(sizeof(char));
	int len = 1;
	char ch, oldch;
	int numAttrs = 1;
	oldch = ',';
	while ((ch = fgetc(file)) != '\n') {

		if (ch == EOF)
			break;
		while (ch == ' ' || ch == '\t' || ch == '\n') {
			ch = fgetc(file);
		}
		if (ch == EOF)
			break;
		if (ch == ',') {
			numAttrs++;
			if (oldch == ch) {
				cout << "Null values are not allowed in attribute names\n";
				return FAILURE;
			}
		}
		attr[len - 1] = ch;
		len++;
		attr = (char *) realloc(attr, (len) * sizeof(char));
		oldch = ch;
	}

	if (oldch == ',') {
		cout << "Null values are not allowed in attribute names\n";
		return FAILURE;
	}

	attr[len - 1] = '\0';
	int i = 0, j, k;
	char attribute[numAttrs][ATTR_SIZE];
	j = 0;
	while (j < numAttrs) {
		k = 0;
		while (((attr[i] != ',') && (attr[i] != '\0')) && (k < 15)) {
			attribute[j][k++] = attr[i++];
		}
		if (k == 15) {
			while (attr[i] != ',')
				i++;
		}
		attribute[j][k] = '\0';
		j++;
		i++;
	}
	i = 0;
	//attribute array contains the names of all attributes


	char *attr_type = (char *) malloc(sizeof(char));
	len = 1;
	while ((ch = fgetc(file)) != '\n') {
		attr_type[len - 1] = ch;
		len++;
		attr_type = (char *) realloc(attr_type, (len) * sizeof(char));
	}
	attr_type[len - 1] = '\0';
	i = 0;
	//attr_type contains second line in the file (to know the attrTypes)
	char attribute_type[numAttrs][ATTR_SIZE];
	int attrTypes[numAttrs];
	j = 0;
	while (j < numAttrs) {
		k = 0;
		while (((attr_type[i] != ',') && (attr_type[i] != '\0')) && (k < 15)) {
			attribute_type[j][k++] = attr_type[i++];
		}
		attribute_type[j][k] = '\0';
		attrTypes[j] = checkAttrTypeOfValue(attribute_type[j]);

		j++;
		i++;
	}

	i = 0;
	//attrTypes array contains the types of all attributes
	char newfilename[16];
	int loopv = strlen(filename) - 1;
	while (filename[loopv] != '.') {loopv--;
	}
	loopv--;
	int end = loopv;
	while (filename[loopv] != '/') {
		loopv--;
	}
	int start = loopv + 1;
	int f = 0;
	for (; start <= end; start++) {
		newfilename[f] = filename[start];
		f++;
	}
	newfilename[f] = '\0';
	int ret;
	ret = createRel(newfilename, numAttrs, attribute, attrTypes);
	if (ret != SUCCESS) {
		cout << "Import not possible as createRel failed\n";
		return FAILURE;
	}
	int relId = OpenRelations::openRelation(newfilename);
	if (relId == E_CACHEFULL || relId == E_RELNOTEXIST) {
		cout << "Import not possible as openRel failed\n";
		return FAILURE;
	}
	file = fopen(filename, "r");

	while ((ch = fgetc(file)) != '\n')
		continue;

	char *record = (char *) malloc(sizeof(char));
	len = 1;

	while (1) {
		ch = fgetc(file);
		if (ch == EOF)
			break;
		while (ch == ' ' || ch == '\t' || ch == '\n') {
			ch = fgetc(file);
		}
		if (ch == EOF)
			break;
		len = 1;
		int c_c = 0;
		oldch = ',';
		while ((ch != '\n') && (ch != EOF)) {
			if (ch == ',')
				c_c++;
			if (ch == oldch && ch == ',') {

				OpenRelations::closeRelation(relId);
				ba_delete(newfilename);
				//cout<<ch;
				cout << "Null values are not allowed in attribute names\n";
				return FAILURE;
			}
			record[len - 1] = ch;
			len++;
			record = (char *) realloc(record, (len) * sizeof(char));
			oldch = ch;

			ch = fgetc(file);

		}

		if (oldch == ',' && ch != '\n') {
			OpenRelations::closeRelation(relId);
			ba_delete(newfilename);

			cout << "Null values are not allowed in attribute names\n";
			return FAILURE;
		}
		if (numAttrs != c_c + 1 && ch != '\n') {
			OpenRelations::closeRelation(relId);
			ba_delete(newfilename);
			cout << "Mismatch in number of attributes\n";
			return FAILURE;
		}
		record[len - 1] = '\0';
		i = 0;

		char recordArray[numAttrs][ATTR_SIZE];
		j = 0;
		while (j < numAttrs) {
			k = 0;

			while (((record[i] != ',') && (record[i] != '\0')) && (k < 15)) {
				recordArray[j][k++] = record[i++];
			}
			if (k == 15) {
				while (record[i] != ',')
					i++;
			}
			i++;
			recordArray[j][k] = '\0';

			j++;
		}

		// Construct a record ( array of attrTypes Attribute ) from previous character array
		// Perform attrTypes checking for number types
		Attribute record[numAttrs];
		int retValue = constructRecordFromAttrsArray(numAttrs, record, recordArray, attrTypes);
		if (retValue == E_ATTRTYPEMISMATCH)
			return E_ATTRTYPEMISMATCH;


		int retVal = ba_insert(relId, record);

		if (retVal != SUCCESS) {
			OpenRelations::closeRelation(relId);
			ba_delete(newfilename);
			cout << "Insert failed" << endl;
			return FAILURE;
		}
		if (ch == EOF)
			break;

	}
	OpenRelations::closeRelation(relId);
	fclose(file);
	return SUCCESS;
}

int exportRelation(char *relname, char *filename) {
	FILE *fp_export = fopen(filename, "w");

	if (!fp_export) {
		cout << " Invalid file path" << endl;
		return FAILURE;
	}

	HeadInfo headInfo;
	Attribute relcat_rec[6];

	int firstBlock, numOfAttrs;
	int slotNum;
	for (slotNum = 0; slotNum < SLOTMAP_SIZE_RELCAT_ATTRCAT; slotNum++) {
		getRecord(relcat_rec, RELCAT_BLOCK, slotNum);
		if (strcmp(relcat_rec[0].sval, relname) == 0) {
			firstBlock = (int) relcat_rec[3].nval;
			numOfAttrs = (int) relcat_rec[1].nval;
			break;
		}
	}

	if (slotNum == SLOTMAP_SIZE_RELCAT_ATTRCAT) {
		cout << "The relation does not exist\n";
		return FAILURE;
	}
	if (firstBlock == -1) {
		cout << "No records exist for the relation\n";
		return FAILURE;
	}

	Attribute rec[6];

	int recBlock_Attrcat = ATTRCAT_BLOCK;
	int nextRecBlock_Attrcat;

	// Array for attribute names and types
	int attrNo = 0;
	char attrName[numOfAttrs][ATTR_SIZE];
	int attrType[numOfAttrs];

	/*
	 * Searching the Attribute Catalog Disk Blocks
	 * for finding and storing all the attributes of the given relation
	 */
	 while (recBlock_Attrcat != -1) {
		headInfo = getHeader(recBlock_Attrcat);
		 nextRecBlock_Attrcat = headInfo.rblock;
		for (slotNum = 0; slotNum < SLOTMAP_SIZE_RELCAT_ATTRCAT; slotNum++) {
			getRecord(rec, recBlock_Attrcat, slotNum);
			if (strcmp(rec[0].sval, relname) == 0) {
				// Attribute belongs to this Relation - add info to array
				strcpy(attrName[attrNo], rec[1].sval);
				attrType[attrNo] = (int) rec[2].nval;
				attrNo++;
			}
		}
		 recBlock_Attrcat = nextRecBlock_Attrcat;
	}

	// Write the Attribute names to o/p file
	for (attrNo = 0; attrNo < numOfAttrs; attrNo++) {
		fputs(attrName[attrNo], fp_export);
		if (attrNo != numOfAttrs - 1)
			fputs(", ", fp_export);
	}

	fputs("\n", fp_export);

	int block_num = firstBlock;
	int num_slots;
	int num_attrs;

	/*
	 * Iterate over the record blocks of this relation
	 * Linked list traversal
	 */
	while (block_num != -1) {
		headInfo = getHeader(block_num);

		num_slots = headInfo.numSlots;
		num_attrs = headInfo.numAttrs;
		nextRecBlock_Attrcat = headInfo.rblock;

		unsigned char slotmap[num_slots];
		getSlotmap(slotmap, block_num);

		Attribute A[num_attrs];
		slotNum = 0;
		// Go through all slots and write the record entry to file
		for (slotNum = 0; slotNum < num_slots; slotNum++) {
			if (slotmap[slotNum] == SLOT_OCCUPIED) {
				getRecord(A, block_num, slotNum);
				char s[16];
				for (int l = 0; l < numOfAttrs; l++) {
					if (attrType[l] == NUMBER) {
						sprintf(s, "%f ", A[l].nval);
						fputs(s, fp_export);
					}
					if (attrType[l] == STRING) {
						fputs(A[l].sval, fp_export);
					}
					if (l != numOfAttrs - 1)
						fputs(", ", fp_export);
				}
				fputs("\n", fp_export);
			}
		}

		block_num = nextRecBlock_Attrcat;
	}

	fclose(fp_export);
	return SUCCESS;
}


void writeHeaderToFile(FILE *fp_export, HeadInfo h) {
	writeHeaderFieldToFile(fp_export, h.blockType);
	writeHeaderFieldToFile(fp_export, h.pblock);
	writeHeaderFieldToFile(fp_export, h.lblock);
	writeHeaderFieldToFile(fp_export, h.rblock);
	writeHeaderFieldToFile(fp_export, h.numEntries);
	writeHeaderFieldToFile(fp_export, h.numAttrs);
	writeHeaderFieldToFile(fp_export, h.numSlots);
}

void writeHeaderFieldToFile(FILE *fp, int32_t headerField) {
	char tmp[16];
	sprintf(tmp, "%d", headerField);
	fputs(tmp, fp);
	fputs(",", fp);
}

void writeAttributeToFile(FILE *fp, Attribute attribute, int type, int lastLineFlag) {
	char tmp[16];
	if (type == NUMBER) {
		sprintf(tmp, "%d", (int) attribute.nval);
		fputs(tmp, fp);
	} else if (type == STRING) {
		fputs(attribute.sval, fp);
	}

	if (lastLineFlag == 0) {
		fputs(",", fp);
	} else {
		fputs("\n", fp);
	}
}