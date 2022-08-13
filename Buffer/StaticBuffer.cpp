#include "StaticBuffer.h"

#include "StaticBuffer.h"
#include "../define/constants.h"
//#include "../Disk_Class/Disk.cpp"

StaticBuffer::StaticBuffer(){
// copy blockAllocMap blocks from disk to buffer (using readblock() of disk)

//initialise metainfo of all the buffer blocks with dirty:false, free:true, timestamp:-1 and blockNum:-1
}

StaticBuffer::~StaticBuffer(){
	// copy blockAllocMap blocks from buffer to disk(using writeblock() of disk)

	/*iterate through all the buffer blocks,
		write back blocks with meta info as free:false,dirty:true (using writeblock() of disk)*/
}

int StaticBuffer::getFreeBuffer(int blockNum){
	// increase the time stamps in metainfo of all the occupied buffers.

	// if free buffer is available, bufferIndex is the index of the free buffer.

	// if free buffer is not available, replace the buffer with the largest timestamp and set it as bufferIndex.

	// update the metainfo array corresponding to the buffer index.

	// return the buffer index
}

int StaticBuffer::getBufferNum(int blockNum){
	//traverse through the metainfo array, find the buffer index of the buffer to which the block is loaded.

	//if found return buffer index, else indicate failure.
}

int StaticBuffer::getStaticBlockType(int blockNum){
	//traverse the blockAllocMap to find the type corresponding to blockNum.

	//return the blockType obtained(REC/IND_INTERNAL/IND_LEAF/UNUSED)
}

void StaticBuffer::setDirtyBit(int blockNum){
	//find the buffer index corresponding to the block using the getBufferNum().

	//set the dirty bit of that buffer in the metaInfo to true.
}
