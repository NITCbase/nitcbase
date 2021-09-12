//
// Created by Jessiya Joy on 19/08/21.
//

#include <iostream>
#include <cstdio>
#include <cstring>
#include "external_fs_commands.h"
#include "disk_structures.h"
#include "block_access.h"

using namespace std;

void writeHeaderFieldToFile(FILE *fp, int32_t headerField);

void writeHeaderToFile(FILE *fp_export, HeadInfo h);

void writeAttributeToFile(FILE *fp, Attribute attribute, int type, int lastLineFlag);


void dump_relcat() {
	FILE *fp_export = fopen("relation_catalog", "w");
	Attribute relcat_rec[6];

	HeadInfo headInfo;

	// write header of block 4 to disk
	headInfo = getHeader(RELCAT_BLOCK);
	writeHeaderToFile(fp_export, headInfo);

	unsigned char slotmap[headInfo.numSlots];
	getSlotmap(slotmap,  RELCAT_BLOCK);

	for (int slotNum = 0; slotNum < 20; slotNum++) {
		unsigned char ch = slotmap[slotNum];
		fputc(ch, fp_export);
	}
	fputs("\n", fp_export);

	for (int slotNum = 0; slotNum < SLOTMAP_SIZE_RELCAT_ATTRCAT; slotNum++) {
		getRecord(relcat_rec, RELCAT_BLOCK, slotNum);

		if ((char) slotmap[slotNum] == SLOT_UNOCCUPIED)
			strcpy(relcat_rec[0].sval, "NULL");

		// RelationName
		writeAttributeToFile(fp_export, relcat_rec[0], STRING, 0);
		// #Attributes
		writeAttributeToFile(fp_export, relcat_rec[1], NUMBER, 0);
		// #Records
		writeAttributeToFile(fp_export, relcat_rec[2], NUMBER, 0);
		// FirstBlock
		writeAttributeToFile(fp_export, relcat_rec[3], NUMBER, 0);
		// LastBlock
		writeAttributeToFile(fp_export, relcat_rec[4], NUMBER, 0);
		// #SlotsPerBlock
		writeAttributeToFile(fp_export, relcat_rec[5], NUMBER, 1);
	}

	fclose(fp_export);
}

void dump_attrcat() {
	FILE *fp_export = fopen("attribute_catalog", "w");
	Attribute attr[6];
	int attr_blk = 5;

	HeadInfo h;

	while (attr_blk != -1) {
		h = getHeader(attr_blk);
		writeHeaderToFile(fp_export, h);

		unsigned char slotmap[h.numSlots];
		getSlotmap(slotmap, attr_blk);
		for (int k = 0; k < 20; k++) {
			unsigned char ch = slotmap[k];
			fputc(ch, fp_export);
		}
		fputs("\n", fp_export);

		for (int i = 0; i < 20; i++) {

			getRecord(attr, attr_blk, i);
			if ((char) slotmap[i] == SLOT_UNOCCUPIED) {
				strcpy(attr[0].sval, "NULL");
				strcpy(attr[1].sval, "NULL");
			}

			// RelationName
			writeAttributeToFile(fp_export, attr[0], STRING, 0);
			// AttributeName
			writeAttributeToFile(fp_export, attr[1], STRING, 0);
			// AttributeType
			writeAttributeToFile(fp_export, attr[2], NUMBER, 0);
			// PrimaryFlag
			writeAttributeToFile(fp_export, attr[3], NUMBER, 0);
			// RootBlock
			writeAttributeToFile(fp_export, attr[4], NUMBER, 0);
			// Offset
			writeAttributeToFile(fp_export, attr[5], NUMBER, 1);
		}
		attr_blk = h.rblock;
	}
	fclose(fp_export);
}

void dumpBlockAllocationMap() {
	FILE *disk = fopen("disk", "rb+");
	fseek(disk, 0, SEEK_SET);
	unsigned char blockAllocationMap[4 * BLOCK_SIZE];
	fread(blockAllocationMap, 4 * BLOCK_SIZE, 1, disk);
	fclose(disk);

	int iter;
	char s[16];
	FILE *fp_export = fopen("block_allocation_map", "w");
	for (iter = 0; iter < 4; iter++) {
		fputs("Block ", fp_export);
		sprintf(s, "%d", iter);
		fputs(s, fp_export);
		fputs(": Block Allocation Map\n", fp_export);
	}
	for (iter = 4; iter < 8192; iter++) {
		fputs("Block ", fp_export);
		sprintf(s, "%d", iter);
		fputs(s, fp_export);
		if ((int32_t) (blockAllocationMap[iter]) == UNUSED_BLK) {
			fputs(": Unused Block\n", fp_export);
		}
		if ((int32_t) (blockAllocationMap[iter]) == REC) {
			fputs(": Record Block\n", fp_export);
		}
		if ((int32_t) (blockAllocationMap[iter]) == IND_INTERNAL) {
			fputs(": Internal Index Block\n", fp_export);
		}
		if ((int32_t) (blockAllocationMap[iter]) == IND_LEAF) {
			fputs(": Leaf Index Block\n", fp_export);
		}
	}

	fclose(fp_export);
}

void ls() {
	union Attribute attr[6];
	int attr_blk = 4;
	struct HeadInfo h;
	h = getHeader(attr_blk);
	unsigned char slotmap[h.numSlots];
	getSlotmap(slotmap, attr_blk);
	for (int i = 0; i < 20; i++) {
		getRecord(attr, attr_blk, i);
		if ((char) slotmap[i] == SLOT_OCCUPIED)
			std::cout << attr[0].sval << "\n";
	}
	std::cout << "\n";
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

	int recBlock = ATTRCAT_BLOCK;
	int nextRecBlock;

	// Array for attribute names and types
	int attrNo = 0;
	char attrName[numOfAttrs][ATTR_SIZE];
	int attrType[numOfAttrs];

	while (recBlock != -1) {
		headInfo = getHeader(recBlock);
		nextRecBlock = headInfo.rblock;
		for (slotNum = 0; slotNum < SLOTMAP_SIZE_RELCAT_ATTRCAT; slotNum++) {
			getRecord(rec, recBlock, slotNum);
			if (strcmp(rec[0].sval, relname) == 0) {
				// Attribute belongs to this Relation - add info to array
				strcpy(attrName[attrNo], rec[1].sval);
				attrType[attrNo] = (int) rec[2].nval;
				attrNo++;
			}
		}
		recBlock = nextRecBlock;
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
		nextRecBlock = headInfo.rblock;

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

		block_num = nextRecBlock;
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