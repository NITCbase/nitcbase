//
// Created by Jessiya Joy on 17/10/21.
//

#include <cstring>
#include <cstdio>
#include <queue>
#include <iostream>
#include "BPlusTree.h"
#include "../define/constants.h"
#include "../define/errors.h"
#include "disk_structures.h"
#include "block_access.h"

using namespace std;

void BFS(int rootBlock) {
	queue<int> blocks;
	blocks.push(rootBlock);
	blocks.push(INT_MAX);
	while (!blocks.empty()) {
		int current_block = blocks.front();

		if (current_block == INT_MAX) {
			// keys.push(current_block);
//			cout << endl;
			cout << " - ";
			blocks.pop();
			continue;
		}
		blocks.pop();

		HeadInfo header = getHeader(current_block);
		int block_type = getBlockType(current_block);
		int num_entries = header.numEntries;

		if (block_type == IND_INTERNAL) {
			InternalEntry internal_entry;
			for (int iter = 0; iter < num_entries; iter++) {
				internal_entry = getInternalEntry(current_block, iter);
				// keys.push((int) internal_entry.attrVal.nval);
				cout << (int) internal_entry.attrVal.nval << ",";
			}
			cout  << " - ";
		} else if (block_type == IND_LEAF) {
			for (int iter = 0; iter < num_entries; iter++) {
				Index index = getLeafEntry(current_block, iter);
				// keys.push((int) index.attrVal.nval);
				cout << (int) index.attrVal.nval << ",";
			}
			cout  << " - ";
		}

		if (block_type == IND_INTERNAL) {
			InternalEntry internal_entry;

			int entry_num = 0;
			internal_entry = getInternalEntry(current_block, entry_num);
			blocks.push(internal_entry.lChild);
//			blocks.push(INT_MIN);

			for (entry_num = 0; entry_num < num_entries; entry_num++) {
				internal_entry = getInternalEntry(current_block, entry_num);
				blocks.push(internal_entry.rChild);
//				blocks.push(INT_MIN);
			}
			blocks.push(INT_MAX);
		}
	}
}


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
	headInfo.lblock = -1;
	headInfo.rblock = -1;
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

		int num_records = header.numEntries;
		int num_slots = header.numSlots;
		unsigned char slotmap[num_slots];
		getSlotmap(slotmap, dataBlock);

		int iter;
		for (iter = 0; iter < num_records; iter++) {

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

			/*
			 * DEBUGING----------------
			 */
//			BFS(this->rootBlock);
//			cout << endl;
			// -----------------
//			break;
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
			internalEntry = getInternalEntry(blockNum, current_entryNumber);
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
	if (num_of_entries != MAX_KEYS_LEAF) {

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
			//failed to obtain an empty leaf index because the disk is full

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
		leftBlkHeader.numEntries = MIDDLE_INDEX_LEAF + 1;
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
		newRightBlkHeader.blockType = IND_LEAF;
		newRightBlkHeader.numEntries = MIDDLE_INDEX_LEAF + 1;
		newRightBlkHeader.lblock = leftBlkNum;
		newRightBlkHeader.pblock = leftBlkHeader.pblock;
		newRightBlkHeader.rblock = prevRblock;
		setHeader(&newRightBlkHeader, newRightBlkNum);

		//store pblock of leftBlk in parBlkNum.
		int parentBlock = leftBlkHeader.pblock;

		// set the first 32 entries of leftBlk as the first 32 entries of indices array
		int indices_iter;
		for (indices_iter = 0; indices_iter <= MIDDLE_INDEX_LEAF; indices_iter++) {
			setLeafEntry(indices[indices_iter], leftBlkNum, indices_iter);
		}
		// set the first 32 entries of newRightBlk as the next 32 entries of indices array
		for (int rBlockIndexIter = 0; rBlockIndexIter <= MIDDLE_INDEX_LEAF; rBlockIndexIter++) {
			setLeafEntry(indices[indices_iter], newRightBlkNum, rBlockIndexIter);
			indices_iter++;
		}
		//********REPLACE
		indices[0].attrVal.nval = 0;
		indices[0].block = 0;
		indices[0].slot = 0;
		for (indices_iter = MIDDLE_INDEX_LEAF + 1; indices_iter < MAX_KEYS_LEAF; indices_iter++) {
			setLeafEntry(indices[0], leftBlkNum, indices_iter);
		}

		/*
		 * store the attribute value of indices[31] in newAttrVal;
		 * this is attribute value which needs to be inserted in the parent block
		 */
		Index leafentry;
		leafentry = getLeafEntry(leftBlkNum, MIDDLE_INDEX_LEAF);
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
					internalEntry = getInternalEntry(parentBlock, indices_iter);

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
					if (attrType == NUMBER) {
						internal_entries[current_entryNumber].attrVal.nval = newAttrVal.nval;
					} else if (attrType == STRING) {
						strcpy(internal_entries[current_entryNumber].attrVal.sval, newAttrVal.sval);
					}
					internal_entries[current_entryNumber].lChild = leftBlkNum;
					internal_entries[current_entryNumber].rChild = newRightBlkNum;
				}

				// parentBlock has not reached max limit.
				if (parentHeader.numEntries != MAX_KEYS_INTERNAL) {
					// increment parheader.numEntries and update it as header of parblk
					parentHeader.numEntries = parentHeader.numEntries + 1;
					setHeader(&parentHeader, parentBlock);

					// iterate through all entries in internalEntries array and populate the entries of parentBlock with them
					for (indices_iter = 0; indices_iter < parentHeader.numEntries; ++indices_iter) {
						setInternalEntry(internal_entries[indices_iter], parentBlock, indices_iter);
					}

					done = true;
				} else //if parent block is full
				{
					int newBlock = getFreeBlock(IND_INTERNAL); // to get a new internalblock

					// disk full
					if (newBlock == FAILURE) {
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
					leftBlkHeader.numEntries = MIDDLE_INDEX_INTERNAL;
					setHeader(&leftBlkHeader, leftBlkNum);

					//load newRightBlkHeader
					newRightBlkHeader = getHeader(newRightBlkNum);

					/* Update right block header
					   * - number of entries = 50
					   * - parent block = parent block of leftBlkNum
					   */
					newRightBlkHeader.numEntries = MIDDLE_INDEX_INTERNAL;
					newRightBlkHeader.pblock = leftBlkHeader.pblock;
					setHeader(&newRightBlkHeader, newRightBlkNum);

					// set the first 50 entries of leftBlk as the first 50 entries of internalEntries array
					for (indices_iter = 0; indices_iter < MIDDLE_INDEX_INTERNAL; ++indices_iter) {
						// TODO ::: REVIEW ::::
//						if ((internal_entries[indices_iter].lChild == parentBlock) ||
//						    (internal_entries[indices_iter].rChild == parentBlock)) {
//							InternalEntry entry;
//							entry = getInternalEntry(parentBlock, indices_iter);
//							if (internalEntry.attrVal.nval == entry.attrVal.nval)
//								continue;
//							if (internal_entries[indices_iter].lChild == parentBlock)
//								internal_entries[indices_iter].lChild = entry.lChild;
//							else
//								internal_entries[indices_iter].rChild = entry.rChild;
//						}
						// ---------
						setInternalEntry(internal_entries[indices_iter], leftBlkNum, indices_iter);
					}

					indices_iter = MIDDLE_INDEX_INTERNAL;
					// set the first 50 entries of newRightBlk as the entries from 51 to 100 of internalEntries array
					for (int j = 0; j < MIDDLE_INDEX_INTERNAL; ++j) {
						setInternalEntry(internal_entries[indices_iter], newRightBlkNum, j);
						indices_iter++;
					}

					int childNum;
					//iterate from 50 to 100:
					for (int k = MIDDLE_INDEX_INTERNAL; k < MAX_KEYS_INTERNAL; ++k) {
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
					newAttrVal = internal_entries[MIDDLE_INDEX_INTERNAL - 1].attrVal;

				}
			} else //if headparblock == -1 i.e root is split now
			{
				int new_root_block = getFreeBlock(IND_INTERNAL);//get new internal block
				// disk full
				if (new_root_block == FAILURE) {
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
				rootEntry.lChild = leftBlkNum;
				rootEntry.rChild = newRightBlkNum;
				setInternalEntry(rootEntry, new_root_block, 0);

				//update number of entries in newRootBlk as 1
				HeadInfo newRootHeader;
				HeadInfo header1 = getHeader(leftBlkNum);
				HeadInfo header2 = getHeader(newRightBlkNum);
				newRootHeader.numEntries = 1;
				newRootHeader.blockType = IND_INTERNAL;
				newRootHeader.pblock = -1;
				header1.pblock = new_root_block;
				header2.pblock = new_root_block;
				setHeader(&newRootHeader, new_root_block);
				setHeader(&header1, leftBlkNum);
				setHeader(&header2, newRightBlkNum);

				attrCatEntry[ATTRCAT_ROOT_BLOCK_INDEX].nval = new_root_block;
				setAttrCatEntry(relId, attrName, attrCatEntry);

				this->rootBlock = new_root_block;
				done = true;
			}
		}
	}
	return SUCCESS;
}

recId BPlusTree::BPlusSearch(Attribute attrVal, int op, indexId *prev_indexId) {
	// Used to store search index for attrName
	indexId searchIndex;
	searchIndex.block = prev_indexId->block;
	searchIndex.index = prev_indexId->index;

	// Get attribute catalog entry of target attribute
	Attribute attrCatEntry[6];
	int flag = getAttrCatEntry(relId, attrName, attrCatEntry);

	if (flag != SUCCESS) {
		return {-1, -1};
	}

	// Store root block num and attribute type of the B+ Tree in rootBlock and attrType respectively.
	int rootBlock = this->rootBlock;
	int attrType = (int) attrCatEntry[2].nval;

	// Block and index variables are used to locate the leaf index to be searched.
	int block, index;
	if (searchIndex.block == -1 && searchIndex.index == -1) {
		// Search is done for the first time
		block = rootBlock;      // start from root
		index = 0;              // start from the first index when searching

		if (block == -1) {
			// B+ tree has not yet been created
			return {-1, -1};
		}
	} else {
		// Search starts from record next to the previous hit
		block = searchIndex.block;
		index = searchIndex.index + 1;

		// Load the header of leaf block
		HeadInfo leafHead;
		leafHead = getHeader(block);

		// Load the leaf block
		int numEntries = leafHead.numEntries;

		// Check if index exceeds maximum number of entries in the current block
		if (index >= numEntries) {
			// All the entries in the block has been searched
			// search from the beginning of the next leaf index block
			block = leafHead.rblock;
			index = 0;

			if (block == -1) {
				// End of Linked list of Leafs
				return {-1, -1};
			}
		}
	}

	/* Incase of Search for first time, traverse the B+ tree and reach appropriate leaf entry */
	// Used to store the header of the internal block
	HeadInfo intHead;
	// Used to store an internal entry of the internal block
	InternalEntry internalEntry;
	/* cond =>
	 * if 1 move to left child
	 * else move to right child after going through all the internal entries
	 */
	int cond;
	while (getBlockType(block) == IND_INTERNAL) {
		intHead = getHeader(block);
		int numOfEntries = intHead.numEntries;
		int currEntryNum;
		cond = 0;
		// Iterate over all
		for (currEntryNum = 0; currEntryNum < numOfEntries; currEntryNum++) {
			internalEntry = getInternalEntry(block, currEntryNum);
			int flag = compareAttributes(attrVal, internalEntry.attrVal, attrType);
			switch (op) {
				case EQ:
					if (flag >= 0) {
						// move to the left child of the first entry that is greater than or equal to attrVal.
						cond = 1;
					}
					break;
				case LE:
					// Since indexing is in ascending order, for lesser values always move left
					cond = 1;
					break;
				case LT:
					// Since indexing is in ascending order, for lesser values always move left
					cond = 1;
					break;
				case GE:
					if (flag >= 0) {
						// move to the left child of the first entry that is greater than or equal to attrVal.
						cond = 1;
					}
					break;
				case GT:
					if (flag >= 0) {
						// move to the left child of the first entry that is greater than or equal to attrVal.
						cond = 1;
					}
					break;
				case NE:
					// Need to search the entire linked list of index
					// So go to the leftmost entry (move left always)
					cond = 1;
					break;
			}

			if (cond == 1) {
				// Condition Met in this Internal Block
				// Now, Search in the Left Child
				block = intHead.lblock;
				break;
			} else {
				// Continue iterating this Internal index block
				continue;
			}
		}
		if (cond == 0) {
			// traversed all the entries of internalBlk without satisfying op condition
			// proceed to search the right child
			block = intHead.rblock;
		}
	}
	/* Traversing of B+ tree has been done and Appropriate Leaf Block has been reached */

	// Used to store the header of the leaf block.
	HeadInfo leafHead;
	// Used to store an index entry of the leaf block.
	Index leafEntry;
	/* cond =>
	 * cond = 0: not found but need to search more
	 * cond = 1: found a record satisfying the search condition
	 * cond = -1 : stop searching
	 */
	cond = 0;
	/* Traverse through index entries in the leaf index block starting from the index entry - index */
	while (block != -1) {
		leafHead = getHeader(block);
		while (index < leafHead.numEntries) {
			leafEntry = getLeafEntry(block, index);
			int flag = compareAttributes(attrVal, leafEntry.attrVal, attrType);
			switch (op) {
				case EQ:
					if (flag == 0) {
						// Entry satisfies EQ condition.
						cond = 1;
					} else if (flag > 0) {
						// Indexes are in Ascending order, so further traversal will NOT give EQ condition
						cond = -1;
					}
					break;
				case LE:
					if (flag <= 0) {
						// Entry satisfies LE condition.
						cond = 1;
					} else {
						// Indexes are in Ascending order, so further traversal will NOT give LE condition
						cond = -1;
					}
					break;
				case LT:
					if (flag < 0) {
						// Entry satisfies LT condition
						cond = 1;
					} else {
						// Indexes are in Ascending order, so further traversal will NOT give LT condition
						cond = -1;
					}
					break;
				case GE:
					if (flag >= 0) {
						// Entry satisfies GE condition
						cond = 1;
					}
					break;
				case GT:
					if (flag > 0) {
						// Entry satisfies GT condition
						cond = 1;
					}
					break;
				case NE:
					if (flag != 0) {
						// Entry satisfies NE condition
						cond = 1;
					}
					break;
			}
			if (cond == 1) {
				// Update prev_indexId to reflect this new search hit
				(*prev_indexId).block = block;
				(*prev_indexId).index = index;

				return {leafEntry.block, leafEntry.slot};
			} else if (cond == -1) {
				// No Record matches the given search condition
				return {-1, -1};
			} else {
				// Keep on searching
				index++;
			}
		}
		if (op != NE) {
			/* For all ops other than NE,
			 * it is gauranteed that
			 *  - the block being searched will have the required record, if it exists
			 *  - And that next blocks will NOT have the required records matching the search condition
			 *  So exit from search loop
			 */
			break;
		} else {
			// Case for NE
			block = leafHead.rblock;
			index = 0; // reset
		}
	}
	return {-1, -1};
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
		InternalEntry internal_entry = getInternalEntry(blockNum, iter);
		bPlusDestroy(internal_entry.lChild);
		for (iter = 0; iter < num_entries; iter++) {
			// get the internal index block entries
			internal_entry = getInternalEntry(blockNum, iter);
			bPlusDestroy(internal_entry.rChild);
		}
		deleteBlock(blockNum);
	} else if (block_type == IND_LEAF) {
		deleteBlock(blockNum);
	} else {
		//if the block is not index block
		return E_INVALIDBLOCK;
	}
	return SUCCESS;
}