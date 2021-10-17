//
// Created by Jessiya Joy on 17/10/21.
//

#include <cstring>
#include "BPlusTree.h"
#include "../define/constants.h"
#include "../define/enum_constants.h"
#include "../define/errors.h"
#include "disk_structures.h"
#include "block_access.h"

BPlusTree::BPlusTree(int relId, char attrName[ATTR_SIZE]) {
	// initialise object instance member fields
	this->relId = relId;
	strcpy(this->attrName, attrName);

	// get the attribute catalog entry of target attribute
	union Attribute attrCatEntry[6];
	int flag = getAttrCatEntry(relId, attrName, attrCatEntry);
	// in case attribute does not exist
	if (flag != SUCCESS) {
		this->rootBlock = flag;
		return;
	}

	// check if an index already exists for the attribute or not
	if (attrCatEntry[ATTRCAT_ROOT_BLOCK_INDEX].nval != -1) {
		this->rootBlock = attrCatEntry[ATTRCAT_ROOT_BLOCK_INDEX].nval;
		return;
	}

	// CREATING A NEW B+ TREE

	// getting free block as leaf index/root block
	int root_block = getFreeBlock(IND_LEAF);
	if (root_block == FAILURE) {   //Problem in getting free leaf index block
		this->rootBlock = E_DISKFULL;
		return;
	}

	this->rootBlock = root_block;

	// set header for root block
	HeadInfo headInfo;
	headInfo.blockType = IND_LEAF;
	headInfo.pblock = -1;
	headInfo.lblock = 0;
	headInfo.rblock = 0;
	headInfo.numEntries = 0;
	setHeader(&headInfo, root_block);

	// update AttrCatEntry with root block
	attrCatEntry[ATTRCAT_ROOT_BLOCK_INDEX].nval = root_block;
	if (setAttrCatEntry(relId, attrName, attrCatEntry) != SUCCESS) {
		this->rootBlock = FAILURE;
		return;
	}

	// get num of attrs, record block number from relCatEntry
	Attribute relCatEntry[6];
	if (getRelCatEntry(relId, relCatEntry) != SUCCESS) {
		this->rootBlock = FAILURE;
		return;
	}
	int data_block, num_attrs;
	data_block = (int) relCatEntry[RELCAT_FIRST_BLOCK_INDEX].nval; //first record block for the relation
	num_attrs = (int) relCatEntry[RELCAT_NO_ATTRIBUTES_INDEX].nval; //num of attributes for the relation

	int attroffset, attrtype;
	attroffset = (int) attrCatEntry[ATTRCAT_OFFSET_INDEX].nval;
	attrtype = (int) attrCatEntry[ATTRCAT_ATTR_TYPE_INDEX].nval;

	Attribute record[num_attrs];

	// inserting index entries for each record in bplus tree
	while (data_block != -1) {
		// get header of record block
		HeadInfo header;
		header = getHeader(data_block);

		int num_slots;
		num_slots = header.numSlots;
		unsigned char slotmap[num_slots];
		getSlotmap(slotmap, data_block);

		int iter;
		for (iter = 0; iter < num_slots; iter++) {
			if (slotmap[iter] == SLOT_UNOCCUPIED)
				continue;

			// get iter th number record from data block
			getRecord(record, data_block, iter);

			// get attribute value
			Attribute attrval;
			if (attrtype == NUMBER) {
				attrval.nval = record[attroffset].nval;
			} else if (attrtype == STRING) {
				strcpy(attrval.sval, record[attroffset].sval);
			}

			recId recid;
			recid.block = data_block;
			recid.slot = iter;

			int res;
			// TODO : int res = bplus_insert(relId, attrName, attrval, recid);

			if (res != SUCCESS) {
				// TODO : bplus_destroy(root_block);
				this->rootBlock = FAILURE;
				return;
			}
		}
		data_block = header.rblock; //next data block for the relation
	}
}

int BPlusTree::bPlusInsert(union Attribute val, struct recId recordId) {

}

struct recId BPlusTree::BPlusSearch(Attribute attrVal, int op) {
	// TODO
}

int BPlusTree::getRootBlock() {

}

int BPlusTree::bPlusDestroy(int blockNum) {
	// TODO
}