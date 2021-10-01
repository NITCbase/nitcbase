//
// Created by Gokul Sreekumar on 06/06/21.
//
#include <cstdio>
#include <cstring>
#include <string>
#include "../define/constants.h"
#include "Disk.h"
#include "disk_structures.h"
#include "block_access.h"

int Disk::createDisk() {
	FILE *disk = fopen(DISK_PATH,"wb+");
	if(disk == nullptr)
		return FAILURE;
	fseek(disk, 0, SEEK_SET);

	// 16 MB
	for(int i=0; i < DISK_SIZE; i++){
		fputc(0,disk);
	}

	fclose(disk);
	return SUCCESS;
}

Disk::~Disk() {

}

int Disk::readBlock(unsigned char *block, int blockNum) {
	FILE *disk = fopen(DISK_PATH,"rb");
	const int offset = blockNum * BLOCK_SIZE;
	fseek(disk, offset, SEEK_SET);
	fread(block, BLOCK_SIZE, 1, disk);
	fclose(disk);
}

int Disk::writeBlock(unsigned char *block, int blockNum) {
	FILE *disk = fopen(DISK_PATH,"wb");
	int offset = blockNum * BLOCK_SIZE;
	fseek(disk, offset, SEEK_SET);
	fwrite(block, BLOCK_SIZE, 1, disk);
	fclose(disk);
}

/*
 * Formats the disk
 * Set the reserved_blocks entries in block allocation map
 * Set Relcat and Attrcat
 */
void Disk::formatDisk() {
	FILE *disk = fopen(DISK_PATH, "wb+");
	const int reserved_blocks = 6;
	const int offset = DISK_SIZE;

	fseek(disk, 0, SEEK_SET);
	unsigned char blockAllocationMap[BLOCK_SIZE * BLOCK_ALLOCATION_MAP_SIZE];

	// reserved_blocks Entries in Block Allocation Map (Used)
    // TODO: Type for BLOCK ALLOCATION MAP block required and replace 1 with it.
	for (int i = 0; i < reserved_blocks; i++) {
		if (i >= 0 && i <= 3)
            blockAllocationMap[i] = (unsigned char) 1;
		else
            blockAllocationMap[i] = (unsigned char) REC;
	}

	// Remaining Entries in Block Allocation Map are marked Unused
	for (int i = reserved_blocks; i < BLOCK_SIZE * BLOCK_ALLOCATION_MAP_SIZE; i++)
        blockAllocationMap[i] = (unsigned char) UNUSED_BLK;
	fwrite(blockAllocationMap, BLOCK_SIZE * BLOCK_ALLOCATION_MAP_SIZE, 1, disk);

	// Remaining Locations of Disk initialised to 0
	for (int i = BLOCK_SIZE * BLOCK_ALLOCATION_MAP_SIZE; i < offset; i++) {
		fputc(0, disk);
	}
	fclose(disk);

    Disk::add_disk_metainfo();
}

// TODO : review in which file this function should be
void Disk::add_disk_metainfo() {
    Attribute rec[6];
    HeadInfo *H = (struct HeadInfo *) malloc(sizeof(struct HeadInfo));

    // TODO: use the set_headerInfo, make_relcatrec and make_attrcatrec function in schema.cpp
    /*
     * Set the header for Block 4 - First Block of Relation Catalog
     */
    H->blockType = REC;
    H->pblock = -1;
    H->lblock = -1;
    H->rblock = -1;
    H->numEntries = 2;
    H->numAttrs = NO_OF_ATTRS_RELCAT_ATTRCAT;
    H->numSlots = SLOTMAP_SIZE_RELCAT_ATTRCAT;
    setHeader(H, BLOCK_ALLOCATION_MAP_SIZE);

    /*
     * Set the slot allocation map for Block 4
     */
    unsigned char slot_map[SLOTMAP_SIZE_RELCAT_ATTRCAT];
    for (int slotNum = 0; slotNum < SLOTMAP_SIZE_RELCAT_ATTRCAT; slotNum++) {
        if (slotNum == 0 || slotNum == 1)
            slot_map[slotNum] = SLOT_OCCUPIED;
        else
            slot_map[slotNum] = SLOT_UNOCCUPIED;
    }
    setSlotmap(slot_map, SLOTMAP_SIZE_RELCAT_ATTRCAT, BLOCK_ALLOCATION_MAP_SIZE);

    /*
     * Create and Add 2 Records into Block 4 (Relation Catalog)
     *  - First for Relation Catalog Relation (Block 4 itself is used for this relation)
     *  - Second for Attribute Catalog Relation (Block 5 is used for this relation)
     */
    strcpy(rec[0].sval, "RELATIONCAT");
    rec[1].nval = 6;
    rec[2].nval = 2;
    rec[3].nval = 4;
    rec[4].nval = 4;
    rec[5].nval = 20;
    setRecord(rec, 4, 0);

    strcpy(rec[0].sval, "ATTRIBUTECAT");
    rec[1].nval = 6;
    rec[2].nval = 12;
    rec[3].nval = 5;
    rec[4].nval = 5;
    rec[5].nval = 20;
    setRecord(rec, 4, 1);

    /*
     * Set the header for Block 5 - First Block of Attribute Catalog
     */
    H->blockType = REC;
    H->pblock = -1;
    H->lblock = -1;
    H->rblock = -1;
    H->numEntries = 12;
    H->numAttrs = 6;
    H->numSlots = 20;
    setHeader(H, 5);

    /*
     * Set the slot allocation map for Block 5
     */
    for (int i = 0; i < 20; i++) {
        if (i >= 0 && i <= 11)
            slot_map[i] = SLOT_OCCUPIED;
        else
            slot_map[i] = SLOT_UNOCCUPIED;
    }
    setSlotmap(slot_map, 20, 5);

    /*
     *  Create Entries for every attribute for Relation Catalog and Attribute Catalog
     */
    strcpy(rec[0].sval, "RELATIONCAT");
    strcpy(rec[1].sval, "RelName");
    rec[2].nval = STRING;
    rec[3].nval = -1;
    rec[4].nval = -1;
    rec[5].nval = 0;
    setRecord(rec, 5, 0);

    strcpy(rec[0].sval, "RELATIONCAT");
    strcpy(rec[1].sval, "#Attributes");
    rec[2].nval = NUMBER;
    rec[3].nval = -1;
    rec[4].nval = -1;
    rec[5].nval = 1;
    setRecord(rec, 5, 1);

    strcpy(rec[0].sval, "RELATIONCAT");
    strcpy(rec[1].sval, "#Records");
    rec[2].nval = NUMBER;
    rec[3].nval = -1;
    rec[4].nval = -1;
    rec[5].nval = 2;
    setRecord(rec, 5, 2);

    strcpy(rec[0].sval, "RELATIONCAT");
    strcpy(rec[1].sval, "FirstBlock");
    rec[2].nval = NUMBER;
    rec[3].nval = -1;
    rec[4].nval = -1;
    rec[5].nval = 3;
    setRecord(rec, 5, 3);

    strcpy(rec[0].sval, "RELATIONCAT");
    strcpy(rec[1].sval, "LastBlock");
    rec[2].nval = NUMBER;
    rec[3].nval = -1;
    rec[4].nval = -1;
    rec[5].nval = 4;
    setRecord(rec, 5, 4);

    strcpy(rec[0].sval, "RELATIONCAT");
    strcpy(rec[1].sval, "#Slots");
    rec[2].nval = NUMBER;
    rec[3].nval = -1;
    rec[4].nval = -1;
    rec[5].nval = 5;
    setRecord(rec, 5, 5);

    strcpy(rec[0].sval, "ATTRIBUTECAT");
    strcpy(rec[1].sval, "RelName");
    rec[2].nval = STRING;
    rec[3].nval = -1;
    rec[4].nval = -1;
    rec[5].nval = 0;
    setRecord(rec, 5, 6);

    strcpy(rec[0].sval, "ATTRIBUTECAT");
    strcpy(rec[1].sval, "AttributeName");
    rec[2].nval = STRING;
    rec[3].nval = -1;
    rec[4].nval = -1;
    rec[5].nval = 1;
    setRecord(rec, 5, 7);

    strcpy(rec[0].sval, "ATTRIBUTECAT");
    strcpy(rec[1].sval, "AttributeType");
    rec[2].nval = NUMBER;
    rec[3].nval = -1;
    rec[4].nval = -1;
    rec[5].nval = 2;
    setRecord(rec, 5, 8);

    strcpy(rec[0].sval, "ATTRIBUTECAT");
    strcpy(rec[1].sval, "PrimaryFlag");
    rec[2].nval = NUMBER;
    rec[3].nval = -1;
    rec[4].nval = -1;
    rec[5].nval = 3;
    setRecord(rec, 5, 9);

    strcpy(rec[0].sval, "ATTRIBUTECAT");
    strcpy(rec[1].sval, "RootBlock");
    rec[2].nval = NUMBER;
    rec[3].nval = -1;
    rec[4].nval = -1;
    rec[5].nval = 4;
    setRecord(rec, 5, 10);

    strcpy(rec[0].sval, "ATTRIBUTECAT");
    strcpy(rec[1].sval, "Offset");
    rec[2].nval = NUMBER;
    rec[3].nval = -1;
    rec[4].nval = -1;
    rec[5].nval = 5;
    setRecord(rec, 5, 11);

}