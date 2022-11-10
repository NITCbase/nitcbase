#ifndef NITCBASE_BPLUSTREE_H
#define NITCBASE_BPLUSTREE_H

#include "../Buffer/BlockBuffer.h"
#include "../Buffer/StaticBuffer.h"
#include "../Cache/OpenRelTable.h"
#include "../define/constants.h"
#include "../define/id.h"

class BPlusTree {
 public:
  static int bPlusCreate(int relId, char attrName[ATTR_SIZE]);
  static int bPlusInsert(int relId, char attrName[ATTR_SIZE], union Attribute attrVal, RecId recordId);
  static RecId bPlusSearch(int relId, char attrName[ATTR_SIZE], union Attribute attrVal, int op);
  static int bPlusDestroy(int rootBlockNum);
};

#endif  // NITCBASE_BPLUSTREE_H
