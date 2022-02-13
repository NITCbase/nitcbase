//
// Created by Jessiya Joy on 12/02/22.
//

#ifndef B18_CODE_BUFFER_H
#define B18_CODE_BUFFER_H

#include "../define/constants.h"

class StaticBuffer{

	friend class BlockBuffer;

public:
	//methods
	StaticBuffer();
	~StaticBuffer();
	static int getStaticBlockType(int blockNum);
	static void setDirtyBit(int blockNum);

private:
	//fields
	static unsigned char blocks[HEADER_SIZE][BLOCK_SIZE];
	static struct BufferMetaInfo metaInfo[HEADER_SIZE];
	static unsigned char blockAllocMap[DISK_BLOCKS];

	//methods
	static int getBufferNum(int blockNum);
	static int getFreeBuffer(int blockNum);
};

class BlockBuffer{
public:
	//methods
	BlockBuffer(char blockType);
	BlockBuffer(int blockNum);
	int getBlockNum();
	int getBlockType();
	void setBlockType(int blockType);
	void getHeader(struct HeadInfo* head);
	void setHeader(struct HeadInfo* head);
	void releaseBlock();

protected:
	//field
	int blockNum;

	//methods
	unsigned char *getBufferPtr();
	int getBlock();
	int getFreeBlock(int BlockType);

};

class RecBuffer : public BlockBuffer{

public:

	//methods
	RecBuffer();
	RecBuffer(int blockNum);
	void getSlotmap(unsigned char *slotMap);
	void setSlotmap(unsigned char *slotMap);
	int getRecord(union Attribute *rec,int slotNum);
	int setRecord(union Attribute *rec,int slotNum);

};

class IndBuffer : public BlockBuffer{

public:
	//methods
	IndBuffer(char blockType);
	IndBuffer(int blockNum);
	virtual int getEntry(void *ptr, int indexNum) = 0;
	virtual int setEntry(void *ptr, int indexNum) = 0;

};

class IndInternal : public IndBuffer{

public:
	//methods
	IndInternal();
	IndInternal(int blockNum);
	int getEntry(void *ptr, int indexNum);
	int setEntry(void *ptr, int indexNum);

};

class IndLeaf : public IndBuffer{

public:
	//methods
	IndLeaf();
	IndLeaf(int blockNum);
	int getEntry(void *ptr, int indexNum) ;
	int setEntry(void *ptr, int indexNum) ;

};

#endif //B18_CODE_BUFFER_H