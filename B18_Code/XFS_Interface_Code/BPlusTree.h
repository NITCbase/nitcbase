//
// Created by Jessiya Joy on 17/10/21.
//

#ifndef B18_CODE_BPLUSTREE_H
#define B18_CODE_BPLUSTREE_H

#include "../define/constants.h"
#include "../define/errors.h"

class BPlusTree{
private :
	int rootBlock, relId;
	char attrName[ATTR_SIZE];

public:
	BPlusTree(int relid, char attrName[ATTR_SIZE]);
	int getRootBlock() const;
	int bPlusInsert(union Attribute val, struct recId recordId);
	struct recId BPlusSearch(union Attribute attrVal, int op);
	int bPlusDestroy(int blockNum);
};

#endif //B18_CODE_BPLUSTREE_H
