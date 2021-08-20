//
// Created by Jessiya Joy on 19/08/21.
//

#include <iostream>
#include <cstdio>
#include <cstring>
#include "external_fs_commands.h"
#include "disk_structures.h"
#include "block_access.h"

void writeHeaderFieldToFile(FILE *fp, int32_t headerField);

void writeHeaderToFile(FILE *fp_export, HeadInfo h);

void writeAttributeToFile(FILE *fp, Attribute attribute, int type, int lastLineFlag);


void dump_relcat() {
	FILE *fp_export = fopen("relation_catalog", "w");
	Attribute attr[6];
	int attr_blk = 4;

	HeadInfo h;

	// write header of block 4 to disk
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

		if ((char) slotmap[i] == '0')
			strcpy(attr[0].sval, "NULL");

		// RelationName
		writeAttributeToFile(fp_export, attr[0], STRING, 0);
		// #Attributes
		writeAttributeToFile(fp_export, attr[1], NUMBER, 0);
		// #Records
		writeAttributeToFile(fp_export, attr[2], NUMBER, 0);
		// FirstBlock
		writeAttributeToFile(fp_export, attr[3], NUMBER, 0);
		// LastBlock
		writeAttributeToFile(fp_export, attr[4], NUMBER, 0);
		// #SlotsPerBlock
		writeAttributeToFile(fp_export, attr[5], NUMBER, 1);
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
		sprintf(tmp, "%d", (int)attribute.nval);
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