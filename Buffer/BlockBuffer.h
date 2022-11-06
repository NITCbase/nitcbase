#ifndef NITCBASE_BLOCKBUFFER_H
#define NITCBASE_BLOCKBUFFER_H

#include "StaticBuffer.h"
#include "../define/constants.h"
#include "../define/errors.h"

struct HeadInfo {
    int32_t blockType;
    int32_t pblock;
    int32_t lblock;
    int32_t rblock;
    int32_t numEntries;
    int32_t numAttrs;
    int32_t numSlots;
    unsigned char reserved[4];
};

typedef union Attribute {
    double nVal;
    char sVal[ATTR_SIZE];
} Attribute;

int compareAttrs(Attribute attr1, Attribute attr2, int attrType);

struct InternalEntry {
    int32_t lChild;
    union Attribute attrVal;
    int32_t rChild;
};

struct Index {
    union Attribute attrVal;
    int32_t block;
    int32_t slot;
    unsigned char unused[8];
};

class BlockBuffer {
protected:
    // field
    int blockNum;
    // methods
    int loadBlockAndGetBufferPtr(unsigned char **buffPtr);
    int getFreeBlock(int BlockType);

 public:
    // methods
    BlockBuffer(char blockType);
    BlockBuffer(int blockNum);
    int getBlockNum();
    int getBlockType();
    int setBlockType(int blockType);
    int getHeader(struct HeadInfo *head);
    int setHeader(struct HeadInfo *head);
    void releaseBlock();
};

class RecBuffer : public BlockBuffer {
public:
    // methods
    RecBuffer();
    RecBuffer(int blockNum);
    int getSlotMap(unsigned char *slotMap);
    int setSlotMap(unsigned char *slotMap);
    int getRecord(union Attribute *rec, int slotNum);
    int setRecord(union Attribute *rec, int slotNum);
};

class IndBuffer : public BlockBuffer {
public:
    IndBuffer(int blockNum);
    IndBuffer(char blockType);
    virtual int getEntry(void *ptr, int indexNum) = 0;
    virtual int setEntry(void *ptr, int indexNum) = 0;
};

class IndInternal : public IndBuffer {
public:
    IndInternal();  // update in documentation
    IndInternal(int blockNum);
    int getEntry(void *ptr, int indexNum);
    int setEntry(void *ptr, int indexNum);
};

class IndLeaf : public IndBuffer {
public:
    IndLeaf();
    IndLeaf(int blockNum);
    int getEntry(void *ptr, int indexNum);
    int setEntry(void *ptr, int indexNum);
};

#endif  // NITCBASE_BLOCKBUFFER_H