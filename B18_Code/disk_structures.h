//
// Created by Gokul Sreekumar on 16/08/21.
//

#ifndef B18_CODE_DISK_STRUCTURES_H
#define B18_CODE_DISK_STRUCTURES_H
#include "define/constants.h"
typedef int relId;
typedef struct recId {
	int block;
	int slot;
} recId;

typedef struct RelCatEntry {
	char rel_name[ATTR_SIZE];
	int num_attr;
	int num_rec;
	int first_blk;
	int last_blk;           //Head of linked list of blocks
	int num_slots_blk;      //Number of slots in a block
} RelCatEntry;

typedef struct AttrCatEntry {
	char rel_name[ATTR_SIZE];
	char attr_name[ATTR_SIZE];
	int attr_type;          // can be INT/FLOAT/STR
	bool primary_flag;      // Currently unused
	int root_block;         // root block# of the B+ tree if exists, else -1
	int offset;             // offset of the attribute in the relation
} AttrCatEntry;

typedef struct RecBlock {
	int32_t blockType;
	int32_t pblock;
	int32_t lblock;
	int32_t rblock;
	int32_t numEntries;
	int32_t numAttrs;
	int32_t numSlots;
	unsigned char reserved[4];
	unsigned char slotMap_Records[BLOCK_SIZE-104];
	unsigned char unused[72];
} RecBlock;

typedef struct HeadInfo {
	int32_t blockType;
	int32_t pblock;
	int32_t lblock;
	int32_t rblock;
	int32_t numEntries;
	int32_t numAttrs;
	int32_t numSlots;
	unsigned char reserved[4];
} HeadInfo;

typedef union Attribute {
	double nval;
	char sval[ATTR_SIZE];
} Attribute;


#endif //B18_CODE_DISK_STRUCTURES_H
