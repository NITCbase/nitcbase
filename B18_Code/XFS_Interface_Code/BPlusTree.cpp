//
// Created by Jessiya Joy on 17/10/21.
//

#include <cstring>
#include <cstdio>
#include "BPlusTree.h"
#include "../define/constants.h"
#include "../define/errors.h"
#include "disk_structures.h"
#include "block_access.h"

BPlusTree::BPlusTree(int relId, char attrName[ATTR_SIZE]) {
	// initialise object instance member fields
	this->relId = relId;
	strcpy(this->attrName, attrName);

	// get the attribute catalog entry of target attribute
	Attribute attrCatEntry[6];
	int flag = getAttrCatEntry(relId, attrName, attrCatEntry);
	// in case attribute does not exist
	if (flag != SUCCESS) {
		this->rootBlock = flag;
		return;
	}

	// check if an index already exists for the attribute or not
	if (attrCatEntry[ATTRCAT_ROOT_BLOCK_INDEX].nval != -1) {
		this->rootBlock = (int) attrCatEntry[ATTRCAT_ROOT_BLOCK_INDEX].nval;
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
	int dataBlock, numAttrs;
	dataBlock = (int) relCatEntry[RELCAT_FIRST_BLOCK_INDEX].nval; //first record block for the relation
	numAttrs = (int) relCatEntry[RELCAT_NO_ATTRIBUTES_INDEX].nval; //num of attributes for the relation

	int attrOffset, attrType;
	attrOffset = (int) attrCatEntry[ATTRCAT_OFFSET_INDEX].nval;
	attrType = (int) attrCatEntry[ATTRCAT_ATTR_TYPE_INDEX].nval;

	Attribute record[numAttrs];

	// inserting index entries for each record in bplus tree
	while (dataBlock != -1) {
		// get header of record block
		HeadInfo header;
		header = getHeader(dataBlock);

		int num_slots;
		num_slots = header.numSlots;
		unsigned char slotmap[num_slots];
		getSlotmap(slotmap, dataBlock);

		int iter;
		for (iter = 0; iter < num_slots; iter++) {
			if (slotmap[iter] == SLOT_UNOCCUPIED)
				continue;

			// get iter th number record from data block
			getRecord(record, dataBlock, iter);

			// get attribute value
			Attribute attrval;
			if (attrType == NUMBER) {
				attrval.nval = record[attrOffset].nval;
			} else if (attrType == STRING) {
				strcpy(attrval.sval, record[attrOffset].sval);
			}

			recId rec_id;
			rec_id.block = dataBlock;
			rec_id.slot = iter;

			int res = bPlusInsert(attrval, rec_id);
			if (res != SUCCESS) {
				bPlusDestroy(root_block);
				this->rootBlock = FAILURE;
				return;
			}
		}
		dataBlock = header.rblock; //next data block for the relation
	}
}

int BPlusTree::bPlusInsert(Attribute val, recId recordId) {

	// get attribute catalog entry of target attribute
	Attribute attrCatEntry[6];
	int flag = getAttrCatEntry(relId, attrName, attrCatEntry);
	if (flag != SUCCESS) {
		this->rootBlock = flag;
		return flag;
	}

	//check if B+ Tree exists for attr
	int blockNum = this->rootBlock;
	if (blockNum == -1) {
		this->rootBlock = E_NOINDEX;
		return E_NOINDEX;
	}

	int attrType = (int) attrCatEntry[ATTRCAT_ATTR_TYPE_INDEX].nval;

	int blockType = getBlockType(blockNum);
	HeadInfo blockHeader;
	int num_of_entries, current_entryNumber;

	/******Traverse the B+ Tree to reach the appropriate leaf where insertion can be done******/
	while (blockType != IND_LEAF) {
		blockHeader = getHeader(blockNum);
		num_of_entries = blockHeader.numEntries;
		InternalEntry internalEntry;
		for (current_entryNumber = 0; current_entryNumber < num_of_entries; ++current_entryNumber) {
			internalEntry = getEntry(blockNum, current_entryNumber);
			if (compareAttributes(val, internalEntry.attrVal, attrType) < 0)
				break;
		}

		if (current_entryNumber == num_of_entries) {
			blockNum = internalEntry.rChild;
		} else {
			blockNum = internalEntry.lChild;
		}
		blockType = getBlockType(blockNum);
	}

	// NOTE : blockNum is the leaf index block to which insertion of val is to be done

	/******Insertion of entry in the appropriate leaf block******/
	blockHeader = getHeader(blockNum);
	num_of_entries = blockHeader.numEntries;

	Index indices[num_of_entries + 1];
	Index current_leafEntry;
	int current_leafEntryIndex = 0;
	flag = 0;
	/* iterate through all the entries in the block and copy them to the array indices
	 * Also insert val at appropriate position in the indices array
	*/
	for (current_entryNumber = 0; current_entryNumber < num_of_entries; ++current_entryNumber) {
		current_leafEntry = getLeafEntry(blockNum, current_entryNumber);

		if (flag == 0) {
			if (compareAttributes(val, current_leafEntry.attrVal, attrType) < 0) {
				if (attrType == NUMBER) {
					indices[current_leafEntryIndex].attrVal.nval = val.nval;
				} else if (attrType == STRING) {
					strcpy(indices[current_leafEntryIndex].attrVal.sval, val.sval);
				}
				indices[current_leafEntryIndex].block = recordId.block;
				indices[current_leafEntryIndex].slot = recordId.slot;
				flag = 1;
				current_leafEntryIndex++;
			}
		}
		if (attrType == NUMBER) {
			indices[current_leafEntryIndex].attrVal.nval = current_leafEntry.attrVal.nval;
		} else if (attrType == STRING) {
			strcpy(indices[current_leafEntryIndex].attrVal.sval, current_leafEntry.attrVal.sval);
		}
		indices[current_leafEntryIndex].block = current_leafEntry.block;
		indices[current_leafEntryIndex].slot = current_leafEntry.slot;
		current_leafEntryIndex++;
	}

	if (num_of_entries == current_leafEntryIndex) {
		if (attrType == NUMBER)
			indices[current_leafEntryIndex].attrVal.nval = val.nval;
		if (attrType == STRING)
			strcpy(indices[current_leafEntryIndex].attrVal.sval, val.sval);
		indices[current_leafEntryIndex].block = recordId.block;
		indices[current_leafEntryIndex].slot = recordId.slot;
		current_leafEntryIndex++;
	}

	//leaf block has not reached max limit
	if (num_of_entries != 63) {

		// increment blockHeader.numEntries and set this as header of block
		blockHeader.numEntries = blockHeader.numEntries + 1;
		setHeader(&blockHeader, blockNum);

		// iterate through all the entries of indices array and populate the entries of block with them
		for (int i = 0; i < current_leafEntryIndex; ++i) {
			setLeafEntry(indices[i], blockNum, i);
		}

		return SUCCESS;
	} else { //leaf block is full- need a new leaf to make the entry; split the entries between the two blocks.

		//assign the existing block as the left block in the splitting.
		int leftBlkNum = blockNum;
		// obtain new leaf index block to be used as the right block in the splitting
		int newRightBlkNum = getFreeBlock(IND_LEAF);

		if (newRightBlkNum == FAILURE) {
			//failed to obtain an empty leaf index becauase the disk is full

			//destroy the existing B+ tree by passing rootBlock member field to bPlusDestroy().
			bPlusDestroy(this->rootBlock);

			//update the rootBlock of attribute catalog entry to -1
			attrCatEntry[ATTRCAT_ROOT_BLOCK_INDEX].nval = -1;
			setAttrCatEntry(relId, attrName, attrCatEntry);

			this->rootBlock = E_DISKFULL;
			return E_DISKFULL;
		}

		// let leftBlkHeader be the header of the left block(which is presently stored in blockHeader)
		struct HeadInfo leftBlkHeader = blockHeader;

		//store the block after leftBlk that appears in the linked list in prevRblock
		int prevRblock = leftBlkHeader.rblock;

		/* Update left block header
		 * - number of entries = 32
		 * - right block = newRightBlkNum
		 */
		leftBlkHeader.numEntries = 32;
		leftBlkHeader.rblock = newRightBlkNum;
		setHeader(&leftBlkHeader, leftBlkNum);

		//load the header of newRightBlk in newRightBlkHeader using BlockBuffer::getHeader()
		HeadInfo newRightBlkHeader = getHeader(newRightBlkNum);
		/* Update right block header
		 * - number of entries = 32
		 * - left block = leftBlkNum
		 * - right block = prevRblock
		 * - parent block = parent block of leftBlkNum
		 */

		newRightBlkHeader.numEntries = 32;
		newRightBlkHeader.lblock = leftBlkNum;
		newRightBlkHeader.pblock = leftBlkHeader.pblock;
		newRightBlkHeader.rblock = prevRblock;
		setHeader(&newRightBlkHeader, newRightBlkNum);

		//store pblock of leftBlk in parBlkNum.
		int parentBlock = leftBlkHeader.pblock;

		// set the first 32 entries of leftBlk as the first 32 entries of indices array
		int indices_iter;
		for (indices_iter = 0; indices_iter < 32; indices_iter++) {
			setLeafEntry(indices[indices_iter], leftBlkNum, indices_iter);
		}
		// set the first 32 entries of newRightBlk as the next 32 entries of indices array
		for (int rBlockIndexIter = 0; rBlockIndexIter < 32; rBlockIndexIter++) {
			setLeafEntry(indices[indices_iter], newRightBlkNum, rBlockIndexIter);
			indices_iter++;
		}

		indices[0].attrVal.nval = 0;
		indices[0].block = 0;
		indices[0].slot = 0;
		for (indices_iter = 32; indices_iter < 63; indices_iter++) {
			setLeafEntry(indices[0], leftBlkNum, indices_iter);
		}

		/*
		 * store the attribute value of indices[31] in newAttrVal;
		 * this is attribute value which needs to be inserted in the parent block
		 */
		Index leafentry;
		leafentry = getLeafEntry(leftBlkNum, 31);
		Attribute newAttrVal;

		if (attrType == NUMBER)
			newAttrVal.nval = leafentry.attrVal.nval;
		if (attrType == STRING)
			strcpy(newAttrVal.sval, leafentry.attrVal.sval);

		bool done = false;

		/******Traverse the internal index blocks of the B+ Tree bottom up making insertions wherever required******/
		//let done indicate whether the insertion is complete or not
		while (!done) {
			if (parentBlock != -1) {
				HeadInfo parentHeader = getHeader(parentBlock);
				InternalEntry internal_entries[parentHeader.numEntries + 1];
				InternalEntry internalEntry;
				flag = 0;
				current_entryNumber = 0;

				/* iterate through all the entries of the parentBlock and copy them to the array internal_entries.
				 * Also insert an InternalEntry entry with attrVal as newAttrval, lChild as leftBlkNum,
				 * and rChild as newRightBlkNum at an appropriate position in the internalEntries array.
				 */
				for (indices_iter = 0; indices_iter < parentHeader.numEntries; ++indices_iter) {
					internalEntry = getEntry(parentBlock, indices_iter);

					// inserting newAttrVal at appropriate place
					if (flag == 0) {
						if (compareAttributes(newAttrVal, internalEntry.attrVal, attrType) < 0) {
							if (attrType == NUMBER) {
								internal_entries[current_entryNumber].attrVal.nval = newAttrVal.nval;
							} else if (attrType == STRING) {
								strcpy(internal_entries[current_entryNumber].attrVal.sval, newAttrVal.sval);
							}
							internal_entries[current_entryNumber].lChild = leftBlkNum;
							internal_entries[current_entryNumber].rChild = newRightBlkNum;
							flag = 1;
							current_entryNumber++;
						}
					}

					// copy entries of the parentBlock to the array internal_entries
					if (attrType == NUMBER) {
						internal_entries[current_entryNumber].attrVal.nval = internalEntry.attrVal.nval;
					} else if (attrType == STRING) {
						strcpy(internal_entries[current_entryNumber].attrVal.sval, internalEntry.attrVal.sval);
					}
					if (current_entryNumber - 1 >= 0) {
						internal_entries[current_entryNumber].lChild = internal_entries[current_entryNumber - 1].rChild;
					} else {
						internal_entries[current_entryNumber].lChild = internalEntry.lChild;
					}
					internal_entries[current_entryNumber].rChild = internalEntry.rChild;
					current_entryNumber++;
				}
				// TODO : review
				if (flag == 0) //when newattrval is greater than all parentblock enries
				{
					internal_entries[current_entryNumber].attrVal = newAttrVal;
					internal_entries[current_entryNumber].lChild = leftBlkNum;
					internal_entries[current_entryNumber].rChild = newRightBlkNum;
					flag = 1;
				}

				// parentBlock has not reached max limit.
				if (parentHeader.numEntries != 100) {
					// increment parheader.numEntries and update it as header of parblk
					parentHeader.numEntries = parentHeader.numEntries + 1;
					setHeader(&parentHeader, parentBlock);

					// iterate through all entries in internalEntries array and populate the entries of parentBlock with them
					for (indices_iter = 0; indices_iter < (parentHeader.numEntries + 1); ++indices_iter) {
						setEntry(internal_entries[indices_iter], parentBlock, indices_iter);
					}

					done = true;
				} else //if parent block is full
				{
					int newBlock = getFreeBlock(IND_INTERNAL); // to get a new internalblock

					// disk full
					if(newBlock == FAILURE) {
						// destroy the right subtree, given by newRightBlkNum, build up till now that has not yet been connected to the existing B+ Tree
						bPlusDestroy(newRightBlkNum);
						// destroy the existing B+ tree by passing rootBlock member field
						bPlusDestroy(this->rootBlock);
						// update the rootBlock of attribute catalog entry to -1
						attrCatEntry[ATTRCAT_ROOT_BLOCK_INDEX].nval = -1;
						setAttrCatEntry(relId, attrName, attrCatEntry);
						this->rootBlock = E_DISKFULL;
						return E_DISKFULL;
					}

					//assign new block as the right block in the splitting.
					newRightBlkNum = newBlock;
					//assign parentBlock as the left block in the splitting.
					leftBlkNum = parentBlock;

					//update leftBlkHeader with parheader.
					leftBlkHeader = parentHeader;

					/* Update left block header
		            * - number of entries = 50
		            */
					leftBlkHeader.numEntries = 50;
					setHeader(&leftBlkHeader, leftBlkNum);

					//load newRightBlkHeader
					newRightBlkHeader = getHeader(newRightBlkNum);

					/* Update right block header
		            * - number of entries = 50
		            * - parent block = parent block of leftBlkNum
		            */
					newRightBlkHeader.numEntries = 50;
					newRightBlkHeader.pblock = leftBlkHeader.pblock;
					setHeader(&newRightBlkHeader, newRightBlkNum);

					// set the first 50 entries of leftBlk as the first 50 entries of internalEntries array
					for (indices_iter = 0; indices_iter < 50; ++indices_iter) {
//						if ((internal_entries[indices_iter].lChild == parentBlock) || (internal_entries[indices_iter].rChild == parentBlock)) {
//							InternalEntry entry;
//							entry = getEntry(parentBlock, indices_iter);
//							if (internalEntry.attrVal.ival == entry.attrVal.ival)
//								continue;
//							if (internal_entries[indices_iter].lChild == parentBlock)
//								internal_entries[indices_iter].lChild = entry.lChild;
//							else
//								internal_entries[indices_iter].rChild = entry.rChild;
//						}
						setEntry(internal_entries[indices_iter], leftBlkNum, indices_iter);
					}

					indices_iter = 51;
					// set the first 50 entries of newRightBlk as the entries from 51 to 100 of internalEntries array
					for (int j = 0; j < 50; ++j) {
						setEntry(internal_entries[indices_iter], newRightBlkNum, j);
						indices_iter++;
					}

					int childNum;
					//iterate from 50 to 100:
					for (int k = 50; k < 100; ++k) {
						//assign the rchild block of ith index in internalEntries to childNum
						childNum = internal_entries[k].rChild;

						// update pblock of the child block to newRightBlkNum
						HeadInfo childHeader = getHeader(childNum);
						childHeader.pblock = newRightBlkNum;
						setHeader(&childHeader, childNum);
					}

					//update parBlkNum as the pblock of leftBlk.
					parentBlock = leftBlkHeader.pblock;

					/* update newAttrval to the attribute value of 50th entry in the internalEntries array;
					 * this is attribute value which needs to be inserted in the parent block.
					 */
					newAttrVal = internal_entries[49].attrVal;

				}
			} else //if headparblock == -1 i.e root is split now
			{
				int new_root_block = getFreeBlock(IND_INTERNAL);//get new internal block
				// disk full
				if(new_root_block == FAILURE) {
					// destroy the right subtree, given by newRightBlkNum, build up till now that has not yet been connected to the existing B+ Tree
					bPlusDestroy(newRightBlkNum);
					// destroy the existing B+ tree by passing rootBlock member field
					bPlusDestroy(this->rootBlock);
					// update the rootBlock of attribute catalog entry to -1
					attrCatEntry[ATTRCAT_ROOT_BLOCK_INDEX].nval = -1;
					setAttrCatEntry(relId, attrName, attrCatEntry);
					this->rootBlock = E_DISKFULL;
					return E_DISKFULL;
				}

				/* add the struct InternalEntry entry with
				 *     lChild as leftBlkNum, attrVal as newAttrval, and rChild as newRightBlkNum
				 * as the first entry to new_root_block
				*/
				InternalEntry rootEntry;
				if (attrType == NUMBER)
					rootEntry.attrVal.nval = newAttrVal.nval;
				else if (attrType == STRING)
					strcpy(rootEntry.attrVal.sval, newAttrVal.sval);
				rootEntry.lChild = blockNum;
				rootEntry.rChild = newchild;
				setEntry(rootEntry, new_root_block, 0);

				//update number of entries in newRootBlk as 1
				HeadInfo newRootHeader;
				HeadInfo header1 = getHeader(blockNum);
				HeadInfo header2 = getHeader(newchild);
				newRootHeader.numEntries = 1;
				newRootHeader.blockType = IND_INTERNAL;
				newRootHeader.pblock = -1;
				header1.pblock = new_root_block;
				header2.pblock = new_root_block;
				setHeader(&newRootHeader, new_root_block);
				setHeader(&header1, blockNum);
				setHeader(&header2, newchild);
				attrCatEntry[ATTRCAT_ROOT_BLOCK_INDEX].nval = new_root_block;

				if (setAttrCatEntry(relId, attrCatEntry[ATTRCAT_ATTR_NAME_INDEX].sval, attrCatEntry) != SUCCESS) {
					return FAILURE;
				}
				done = true;
			}
		}
	}
	return SUCCESS;
}

recId BPlusTree::BPlusSearch(Attribute attrVal, int op) {
	// TODO
}

int BPlusTree::getRootBlock() {
	return this->rootBlock;
}

int BPlusTree::bPlusDestroy(int blockNum) {
	HeadInfo header;

	// if the block_num lies outside valid range
	if (blockNum < 0 || blockNum >= DISK_BLOCKS) {
		return E_OUTOFBOUND;
	}
	header = getHeader(blockNum);
	int block_type = getBlockType(blockNum);

	if (block_type == IND_INTERNAL) {
		// if block is internal node remove all children before removing it
		int num_entries;
		num_entries = header.numEntries;

		/*
		 * iterate through all the entries of the internalBlk and
		 * destroy the lChild of the first entry and
		 * rChild of all entries using BPlusTree::bPlusDestroy().
		 * (take care not to delete overlapping children more than once)
		 */
		int iter = 0;
		InternalEntry internal_entry = getEntry(blockNum, iter);
		bPlusDestroy(internal_entry.lChild);
		for (iter = 0; iter < num_entries; iter++) {
			// get the internal index block entries
			internal_entry = getEntry(blockNum, iter);
			bPlusDestroy(internal_entry.rChild);
		}
		deleteBlock(blockNum);
	} else if (block_type == IND_LEAF) {
		deleteBlock(blockNum);
	}
	else {
		//if the block is not index block
		return E_INVALIDBLOCK;
	}
	return SUCCESS;
}