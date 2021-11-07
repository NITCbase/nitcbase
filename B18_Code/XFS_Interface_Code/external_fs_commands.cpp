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

void string_to_char_array(string x, char *a, int size);

void writeHeaderFieldToFile(FILE *fp, int32_t headerField);

void writeHeaderToFile(FILE *fp_export, HeadInfo h);

void writeAttributeToFile(FILE *fp, Attribute attribute, int type, int lastLineFlag);


void dump_relcat() {
	string relation_catalog = "relation_catalog";
	string filePath = FILES_PATH + relation_catalog;
	char fileName[filePath.length() + 1];
	string_to_char_array(filePath, fileName, filePath.length() + 1);

	FILE *fp_export = fopen(fileName, "w");
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
	string attribute_catalog = "attribute_catalog";
	string filePath = FILES_PATH + attribute_catalog;
	char fileName[filePath.length() + 1];
	string_to_char_array(filePath, fileName, filePath.length() + 1);

	FILE *fp_export = fopen(fileName, "w");

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
	FILE *disk = fopen(DISK_PATH, "rb+");
	fseek(disk, 0, SEEK_SET);
	unsigned char blockAllocationMap[4 * BLOCK_SIZE];
	fread(blockAllocationMap, 4 * BLOCK_SIZE, 1, disk);
	fclose(disk);

	int blockNum;
	char s[ATTR_SIZE];

	string block_allocation_map = "block_allocation_map";
	string filePath = FILES_PATH + block_allocation_map;

	char fileName[filePath.length() + 1];
	string_to_char_array(filePath, fileName, filePath.length() + 1);

	FILE *fp_export = fopen(fileName, "w");

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
	HeadInfo headInfo;
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

int importRelation(char *fileName) {

	FILE *file = fopen(fileName, "r");

	/*
	 *  GET ATTRIBUTE NAMES FROM FIRST LINE OF FILE
	 */
	char *firstLine = (char *) malloc(sizeof(char));
	int numOfCharactersInLine = 1;
	char currentCharacter, previousCharacter;
	int numOfAttributes = 1;
	previousCharacter = ',';
	while ((currentCharacter = fgetc(file)) != '\n') {
		if (currentCharacter == EOF)
			break;
		while (currentCharacter == ' ' || currentCharacter == '\t' || currentCharacter == '\n') {
			currentCharacter = fgetc(file);
		}
		if (currentCharacter == EOF)
			break;
		if (currentCharacter == ',') {
			numOfAttributes++;
			if (previousCharacter == currentCharacter) {
				cout << "Null values are not allowed in attribute names\n";
				return FAILURE;
			}
		}
		firstLine[numOfCharactersInLine - 1] = currentCharacter;
		numOfCharactersInLine++;
		firstLine = (char *) realloc(firstLine, (numOfCharactersInLine) * sizeof(char));
		previousCharacter = currentCharacter;
	}

	if (previousCharacter == ',') {
		cout << "Null values are not allowed in attribute names\n";
		return FAILURE;
	}

	firstLine[numOfCharactersInLine - 1] = '\0';
	int currentCharIndexInLine = 0, attrOffsetIterator, attributeIndexIterator;
	char attributeNames[numOfAttributes][ATTR_SIZE];
	attrOffsetIterator = 0;
	while (attrOffsetIterator < numOfAttributes) {
		attributeIndexIterator = 0;
		while (((firstLine[currentCharIndexInLine] != ',') && (firstLine[currentCharIndexInLine] != '\0')) &&
		       (attributeIndexIterator < ATTR_SIZE - 1)) {
			if (checkIfInvalidCharacter(firstLine[currentCharIndexInLine])) {
				cout << "Invalid character : '" << firstLine[currentCharIndexInLine] << "' in attribute name\n";
				return FAILURE;
			}
			attributeNames[attrOffsetIterator][attributeIndexIterator++] = firstLine[currentCharIndexInLine++];
		}
		if (attributeIndexIterator == ATTR_SIZE - 1) {
			while (firstLine[currentCharIndexInLine] != ',')
				currentCharIndexInLine++;
		}
		attributeNames[attrOffsetIterator][attributeIndexIterator] = '\0';
		attrOffsetIterator++;
		currentCharIndexInLine++;
	}
	currentCharIndexInLine = 0;

	/*
	 *  INFER ATTRIBUTE TYPES FROM SECOND LINE OF FILE
	 */
	char *secondLine = (char *) malloc(sizeof(char));
	numOfCharactersInLine = 1;
	while ((currentCharacter = fgetc(file)) != '\n') {
		if (currentCharacter == EOF)
			break;
		secondLine[numOfCharactersInLine - 1] = currentCharacter;
		numOfCharactersInLine++;
		secondLine = (char *) realloc(secondLine, (numOfCharactersInLine) * sizeof(char));
	}
	secondLine[numOfCharactersInLine - 1] = '\0';
	currentCharIndexInLine = 0;
	char secondLineFields[numOfAttributes][ATTR_SIZE];
	int attrTypes[numOfAttributes];
	attrOffsetIterator = 0;
	while (attrOffsetIterator < numOfAttributes) {
		attributeIndexIterator = 0;
		while (((secondLine[currentCharIndexInLine] != ',') && (secondLine[currentCharIndexInLine] != '\0')) &&
		       (attributeIndexIterator < ATTR_SIZE - 1)) {
			secondLineFields[attrOffsetIterator][attributeIndexIterator++] = secondLine[currentCharIndexInLine++];
		}
		secondLineFields[attrOffsetIterator][attributeIndexIterator] = '\0';
		attrTypes[attrOffsetIterator] = checkAttrTypeOfValue(secondLineFields[attrOffsetIterator]);

		attrOffsetIterator++;
		currentCharIndexInLine++;
	}

	// EXTRACT RELATION NAME FROM FILE PATH
	currentCharIndexInLine = 0;
	char relationName[ATTR_SIZE];
	int fileNameIterator = strlen(fileName) - 1;
	while (fileName[fileNameIterator] != '.') {
		fileNameIterator--;
	}
	fileNameIterator--;
	int end = fileNameIterator;
	while (fileName[fileNameIterator] != '/') {
		fileNameIterator--;
	}
	int start;
	int relname_iter;
	for (start = fileNameIterator + 1, relname_iter = 0;
	     start <= end && relname_iter < ATTR_SIZE - 1; start++, relname_iter++) {
		relationName[relname_iter] = fileName[start];
	}
	if (relname_iter == ATTR_SIZE - 1) {
		cout << "File name is more than 15 characters, trimming to get relation name\n";
	}
	relationName[relname_iter] = '\0';

	// CREATE RELATION
	int ret;
	ret = createRel(relationName, numOfAttributes, attributeNames, attrTypes);
	if (ret != SUCCESS) {
		cout << "Import not possible as createRel failed\n";
		return ret;
	}

	// OPEN RELATION
	int relId = OpenRelTable::openRelation(relationName);
	if (relId == E_CACHEFULL) {
		cout << "Import not possible as openRel failed\n";
		return FAILURE;
	}

	// Skip first line containing attribute names
	file = fopen(fileName, "r");
	while ((currentCharacter = fgetc(file)) != '\n')
		continue;

	char *currentLineAsCharArray = (char *) malloc(sizeof(char));
	numOfCharactersInLine = 1;

	int lineNumber = 2;
	while (true) {
		currentCharacter = fgetc(file);
		if (currentCharacter == EOF)
			break;
		while (currentCharacter == ' ' || currentCharacter == '\t' || currentCharacter == '\n') {
			currentCharacter = fgetc(file);
		}
		if (currentCharacter == EOF)
			break;
		numOfCharactersInLine = 1;
		int numOfFieldsInLine = 0;
		previousCharacter = ',';


		while ((currentCharacter != '\n') && (currentCharacter != EOF)) {

			if (currentCharacter == ',')
				numOfFieldsInLine++;
			if (currentCharacter == previousCharacter && currentCharacter == ',') {
				ba_delete(relId);
				cout << "Null values are not allowed in attribute fields\n";
				return FAILURE;
			}
			currentLineAsCharArray[numOfCharactersInLine - 1] = currentCharacter;
			numOfCharactersInLine++;
			currentLineAsCharArray = (char *) realloc(currentLineAsCharArray, (numOfCharactersInLine) * sizeof(char));
			previousCharacter = currentCharacter;

			currentCharacter = fgetc(file);

		}

		if (previousCharacter == ',') {
			ba_delete(relId);

			cout << "Null values are not allowed in attribute fields\n";
			return FAILURE;
		}
		if (numOfAttributes != numOfFieldsInLine + 1) {
			ba_delete(relId);
			cout << "Mismatch in number of attributes\n";
			return FAILURE;
		}
		currentLineAsCharArray[numOfCharactersInLine - 1] = '\0';
		currentCharIndexInLine = 0;

		char attributesCharArray[numOfAttributes][ATTR_SIZE];
		attrOffsetIterator = 0;
		while (attrOffsetIterator < numOfAttributes) {
			attributeIndexIterator = 0;

			while (((currentLineAsCharArray[currentCharIndexInLine] != ',') &&
			        (currentLineAsCharArray[currentCharIndexInLine] != '\0')) &&
			       (attributeIndexIterator < ATTR_SIZE - 1)) {
				attributesCharArray[attrOffsetIterator][attributeIndexIterator++] = currentLineAsCharArray[currentCharIndexInLine++];
			}
			if (attributeIndexIterator == ATTR_SIZE - 1) {
				while (currentLineAsCharArray[currentCharIndexInLine] != ',')
					currentCharIndexInLine++;
			}
			currentCharIndexInLine++;
			attributesCharArray[attrOffsetIterator][attributeIndexIterator] = '\0';

			attrOffsetIterator++;
		}

		Attribute record[numOfAttributes];
		int retValue = constructRecordFromAttrsArray(numOfAttributes, record, attributesCharArray, attrTypes);

		if (retValue == E_ATTRTYPEMISMATCH) {
			ba_delete(relId);
			return E_ATTRTYPEMISMATCH;
		} else if (retValue == E_INVALID) {
			ba_delete(relId);
			cout << "Invalid character at line " << lineNumber << " in file \n";
			return FAILURE;
		}

		int retVal = ba_insert(relId, record);

		if (retVal != SUCCESS) {
			ba_delete(relId);
			cout << "Insert failed" << endl;
			return retVal;
		}
		if (currentCharacter == EOF)
			break;

		lineNumber++;
	}
	OpenRelTable::closeRelation(relId);
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
			fputs(",", fp_export);
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
				char s[ATTR_SIZE];
				for (int l = 0; l < numOfAttrs; l++) {
					if (attrType[l] == NUMBER) {
						sprintf(s, "%f", A[l].nval);
						fputs(s, fp_export);
					}
					if (attrType[l] == STRING) {
						fputs(A[l].sval, fp_export);
					}
					if (l != numOfAttrs - 1)
						fputs(",", fp_export);
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
	char tmp[ATTR_SIZE];
	sprintf(tmp, "%d", headerField);
	fputs(tmp, fp);
	fputs(",", fp);
}

void writeAttributeToFile(FILE *fp, Attribute attribute, int type, int lastLineFlag) {
	char tmp[ATTR_SIZE];
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

bool checkIfInvalidCharacter(char character) {
	if (character >= 48 && character <= 57 || character >= 65 && character <= 90 ||
	    character >= 97 && character <= 122 || character == '-' || character == '_') {
		return false;
	}
	return true;
}