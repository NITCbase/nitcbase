#ifndef B18_CODE_STATICBUFFER_H
#define B18_CODE_STATICBUFFER_H

#include "../define/constants.h"

struct BufferMetaInfo {
	bool free;
	bool dirty;
	int blockNum;
	int timeStamp;
};

class StaticBuffer {

	friend class BlockBuffer;

private:
	//fields
	static unsigned char blocks[BUFFER_CAPACITY][BLOCK_SIZE];
	static struct BufferMetaInfo metainfo[BUFFER_CAPACITY];
	static unsigned char blockAllocMap[DISK_BLOCKS];

	//methods
	static int getFreeBuffer(int blockNum);
	static int getBufferNum(int blockNum);

public:
	//methods
	static int getStaticBlockType(int blockNum);
	static void setDirtyBit(int blockNum);
	StaticBuffer();
	~StaticBuffer();

};

#endif //B18_CODE_STATICBUFFER_H
