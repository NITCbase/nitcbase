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

		if ((char) slotmap[slotNum] == '0')
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
			if ((char) slotmap[i] == '0') {
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
		if ((char) slotmap[i] == '1')
			std::cout << attr[0].sval << "\n";
	}
	std::cout << "\n";
}

int exportRelation(char *rel_name, char *filename) {
	FILE *fp_export = fopen(filename, "w");

	if (!fp_export) {
		cout << " Invalid file path" << endl;
		return FAILURE;
	}

	struct HeadInfo header;
	int slotNum;
	int first_block;
	union Attribute relationCatalogRecord[6];
	int num_slots, next_block_num, num_attrs, no_attrs;

	for (slotNum = 0; slotNum < SLOTMAP_SIZE_RELCAT_ATTRCAT; slotNum++) {
		getRecord(relationCatalogRecord, 4, slotNum);
		if (strcmp(relationCatalogRecord[0].sval, rel_name) == 0) {
			first_block = relationCatalogRecord[3].nval;
			no_attrs = relationCatalogRecord[1].nval;
			break;
		}
	}

	if (slotNum == 20) {
		cout << "The relation does not exist\n";
		return FAILURE;
	}
	if (first_block == -1) {
		cout << "No records exist\n";
		return FAILURE;
	}

	union Attribute attr[6];
	int attr_blk = 5;
	char Attr_name[no_attrs][16];
	int Attr_type[no_attrs];
	int j = 0;
	while (attr_blk != -1) {
		header = getheader(attr_blk);
		next_block_num = header.rblock;
		for (slotNum = 0; slotNum < 20; slotNum++) {
			getRecord(attr, attr_blk, slotNum);
			if (strcmp(attr[0].sval, rel_name) == 0) {
				strcpy(Attr_name[j], attr[1].sval);
				Attr_type[j++] = attr[2].ival;
			}
		}
		attr_blk = next_block_num;
	}
	for (j = 0; j < no_attrs; j++) {
		fputs(Attr_name[j], fp_export);
		if (j != no_attrs - 1)
			fputs(",", fp_export);
	}
	fputs("\n", fp_export);
	int block_num = first_block;
	while (block_num != -1) {
		header = getheader(block_num);
		num_slots = header.numSlots;
		next_block_num = header.rblock;
		num_attrs = header.numAttrs;
		unsigned char slotmap[num_slots];
		getSlotmap(slotmap, block_num);

		union Attribute A[num_attrs];
		int iter;
		for (iter = 0; iter < num_slots; iter++) {
			if (slotmap[iter] == '1') {
				getRecord(A, block_num, iter);
				char s[16];
				for (int l = 0; l < no_attrs; l++) {
					if (Attr_type[l] == FLOAT) {
						sprintf(s, "%f", A[l].fval);
						//cout<<s<<" "<<strlen(s)<<"\n";
						fputs(s, fp_export);
					}
					if (Attr_type[l] == INT) {
						sprintf(s, "%lld", A[l].ival);
						//cout<<s<<" "<<strlen(s)<<"\n";
						fputs(s, fp_export);
					}
					if (Attr_type[l] == STRING) {
						fputs(A[l].sval, fp_export);
					}
					if (l != no_attrs - 1)
						fputs(",", fp_export);
				}
				fputs("\n", fp_export);
			}
		}
		block_num = next_block_num;
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