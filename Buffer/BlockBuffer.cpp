#include "BlockBuffer.h"

#include "StaticBuffer.h"
#include "../define/constants.h"
#include "../define/errors.h"

using namespace std;

// Constructor 1
BlockBuffer::BlockBuffer(char blockType){
    // allocate a block on the disk and a buffer in memory to hold the new block of given type using getFreeBlock function and get the return error codes if any.

    // set the blockNum field of the object to that of the allocated block number if the method returned a valid block number,
    // otherwise set the error code returned as the block number.
        // The caller must check if the constructor allocatted block successfully by checking the value of block number field.
}

// Constructor 2
BlockBuffer::BlockBuffer(int blockNum){
    // set the blockNum field of the object to input argument.
}

int BlockBuffer::getBlockNum() {
    // return corresponding block number

}

int BlockBuffer::getBlockType() {

    unsigned char *bufferPtr;
    // get the starting address of the buffer containing the block using loadBlockAndGetBufferPtr(&bufferPtr).

    // if the call to loadBlockAndGetBufferPtr(&bufferPtr) return SUCCESS
        // return the first 4 bytes of the buffer that stores the block type. (Hint: cast using int32_t)

    // else load failed due to E_OUTOFBOUND, invalid block number, return the value returned by the call.
}

int BlockBuffer::setBlockType(int blockType){

    unsigned char *bufferPtr;
    // get the starting address of the buffer containing the block using loadBlockAndGetBufferPtr(&bufferPtr).

    // if loadBlockAndGetBufferPtr(&bufferPtr) != SUCCESS
        // return the value returned by the call.

    // store the input block type in the first 4 bytes of the buffer.

    // update the StaticBuffer::blockAllocMap entry corresponding to the object's block number.

    // update dirty bit by calling appropriate method of StaticBuffer class.
    // if setDirtyBit() failed
        // return the returned value from the call

    // return SUCCESS
}

int BlockBuffer::getHeader(struct HeadInfo *head) {
    // get the starting address of the buffer containing the block using loadBlockAndGetBufferPtr(&bufferPtr).

    // if loadBlockAndGetBufferPtr(&bufferPtr) != SUCCESS
        // return the value returned by the call.

    // Use type casting here to cast the returned pointer type to the appropriate struct pointer to get the headInfo

    // copy the header of block to the memory location pointed to by the argument head.
        // not copying reserved

    // return SUCCESS
}

int BlockBuffer::setHeader(struct HeadInfo *head) {

    unsigned char *bufferPtr;
    // get the starting address of the buffer containing the block using loadBlockAndGetBufferPtr(&bufferPtr).

    // if loadBlockAndGetBufferPtr(&bufferPtr) != SUCCESS
        // return the value returned by the call.

    // Use type casting here to cast the returned pointer type to the appropriate struct pointer to get the headInfo

    //copy the contents of the memory location pointed to by head to the header of block using appropriate.
        // not copying reserved

    //update dirty bit by calling appropriate method of StaticBuffer class.
    // if setDirtyBit() failed, return the error code

    // return SUCCESS;
}

void BlockBuffer::releaseBlock(){
    // if blockNum is INVALID_BLOCK (-1), or it is invalidated already, do nothing

    // else
        // get the buffer number of the buffer assigned to the block using StaticBuffer::getBufferNum().

        // if the buffer number is valid (!=E_BLOCKNOTINBUFFER), free the buffer by setting the free flag of its metaInfo entry to true. 

        // free the block in disk by setting the data type of the entry corresponding to the block number in StaticBuffer::blockAllocMap to UNUSED_BLK.

        // set the object's blockNum to INVALID_BLOCK (-1)
}

/* NOTE: This function will NOT check if the block already exists in disk or not,
   rather will copy whatever content is there in that disk block to the buffer.
   Only call this if the Block exists in disk already, otherwise call constructor 1 to allocate space for a new block.
   Also ensure that all getter and setter methods accessing the block's data should call the loadBlockAndGetBufferPtr().
 */
int BlockBuffer::loadBlockAndGetBufferPtr(unsigned char **buffPtr) {
    // check whether the block is already present in the buffer using StaticBuffer.getBufferNum()
    int bufferNum = StaticBuffer::getBufferNum(this->blockNum);

    // if present (!=E_BLOCKNOTINBUFFER), set the timestamp of the corresponding buffer to 0 and increment the timestamps of all other occupied buffers in the BufferMetaInfo.

    // else
        // if not present, get a free buffer using StaticBuffer.getFreeBuffer()

        // if the call returns E_OUTOFBOUND, return E_OUTOFBOUND here as the blockNum is invalid

        // Read the block into the free buffer using readBlock()

        // If the read failed, the block number is invalid return E_OUTOFBOUND;

    // store the pointer to this buffer (blocks[bufferNum]) in *buffPtr

    // return SUCCESS;
}

int BlockBuffer::getFreeBlock(int blockType) {
    //iterate through the StaticBuffer::blockAllocMap and find the block number of a free block in the disk.

    //if no block is free, return E_DISKFULL.

    //set the object's blockNum to the block number of the free block.

    //find a free buffer using StaticBuffer::getFreeBuffer() .

    //initialize the header of the block with pblock: -1, lblock: -1, rblock: -1, numEntries: 0, numAttrs: 0 and numSlots: 0 using setHeader().

    //update the block type of the block to the input block type using setBlockType().

    //return block number of the free block.


}


RecBuffer::RecBuffer(int blockNum) : BlockBuffer(blockNum) {}

RecBuffer::RecBuffer() : BlockBuffer('R') {}

int RecBuffer::getSlotMap(unsigned char *slotMap) {
    unsigned char *bufferPtr;
    // get the starting address of the buffer containing the block using loadBlockAndGetBufferPtr(&bufferPtr).

    // if loadBlockAndGetBufferPtr(&bufferPtr) != SUCCESS
        // return the value returned by the call.

    // Use type casting here to cast the returned pointer type to the appropriate struct pointer

    // get the number of slots in the block.

    // using offset range copy the slotmap of the block to the memory pointed by the argument.

    // return SUCCESS
}

int RecBuffer::setSlotMap(unsigned char *slotMap) {
    unsigned char *bufferPtr;
    // get the starting address of the buffer containing the block using loadBlockAndGetBufferPtr(&bufferPtr).

    // if loadBlockAndGetBufferPtr(&bufferPtr) != SUCCESS
        // return the value returned by the call.

    // Use type casting here to cast the returned pointer type to the appropriate struct pointer to access headInfo

    // get the number of slots in the block.

    // using offset range copy the slotmap from the memory pointed by the argument to that of the block.

    // update dirty bit.
    // if setDirtyBit failed, return the value returned by the call

    // return SUCCESS
}

int RecBuffer::getRecord(union Attribute *rec, int slotNum) {
    unsigned char *bufferPtr;
    // get the starting address of the buffer containing the block using loadBlockAndGetBufferPtr(&bufferPtr).

    // if loadBlockAndGetBufferPtr(&bufferPtr) != SUCCESS
        // return the value returned by the call.

    // Use type casting here to cast the returned pointer type to the appropriate struct pointer to access headInfo

    // get number of attributes in the block.

    // get the number of slots in the block.

    // if input slotNum is not in the permitted range return E_OUTOFBOUND

    // if slot corresponding to input slotNum is free return E_FREESLOT

    // using offset range copy slotNumth record to the memory pointed by rec.

    // return SUCCESS

}

int RecBuffer::setRecord(union Attribute *rec, int slotNum) {
    unsigned char *bufferPtr;
    // get the starting address of the buffer containing the block using loadBlockAndGetBufferPtr(&bufferPtr).

    // if loadBlockAndGetBufferPtr(&bufferPtr) != SUCCESS
        // return the value returned by the call.

    // Use type casting here to cast the returned pointer type to the appropriate struct pointer to access headInfo

    // get number of attributes in the block.

    // get the number of slots in the block.

    // if input slotNum is not in the permitted range return E_OUTOFBOUND.

    // using offset range copy contents of the memory pointed by rec to slotNumth record.

    // update dirty bit.
    // if setDirtyBit failed, return the value returned by the call

    // return SUCCESS

}

IndBuffer::IndBuffer(int blockNum) : BlockBuffer(blockNum) {}

IndBuffer::IndBuffer(char blockType) : BlockBuffer(blockType) {}

IndInternal::IndInternal() : IndBuffer('I') {}

IndInternal::IndInternal(int blockNum) : IndBuffer(blockNum) {}

int IndInternal::getEntry(void *ptr, int indexNum) {
    unsigned char *bufferPtr;
    // get the starting address of the buffer containing the block using loadBlockAndGetBufferPtr(&bufferPtr).

    // if loadBlockAndGetBufferPtr(&bufferPtr) != SUCCESS
        // return the value returned by the call.

    // if the indexNum is not in the valid range of 0-(MAX_KEYS_INTERNAL-1), return E_OUTOFBOUND.

    // copy the indexNum'th Internalentry in block to memory ptr(ptr can be type casted appropriately if needed).

    // return SUCCESS.
}

int IndInternal::setEntry(void *ptr, int indexNum) {
    unsigned char *bufferPtr;
    // get the starting address of the buffer containing the block using loadBlockAndGetBufferPtr(&bufferPtr).

    // if loadBlockAndGetBufferPtr(&bufferPtr) != SUCCESS
        // return the value returned by the call.

    // if the indexNum is not in the valid range of 0-(MAX_KEYS_INTERNAL-1), return E_OUTOFBOUND.

    // copy the struct InternalEntry pointed by ptr to indexNum'th entry in block.

    // update dirty bit.
    // if setDirtyBit failed, return the value returned by the call

    // return SUCCESS
}

IndLeaf::IndLeaf() : IndBuffer('L') {}

IndLeaf::IndLeaf(int blockNum) : IndBuffer(blockNum) {}

int IndLeaf::getEntry(void *ptr, int indexNum) {
    unsigned char *bufferPtr;
    // get the starting address of the buffer containing the block using loadBlockAndGetBufferPtr(&bufferPtr).

    // if loadBlockAndGetBufferPtr(&bufferPtr) != SUCCESS
        // return the value returned by the call.

    // if the indexNum is not in the valid range of 0-(MAX_KEYS_LEAF-1), return E_OUTOFBOUND.

    // copy the indexNum'th Index entry in block to memory ptr(ptr can be type casted appropriately if needed).

    // return SUCCESS.

}

int IndLeaf::setEntry(void *ptr, int indexNum) {
unsigned char *bufferPtr;
    // get the starting address of the buffer containing the block using loadBlockAndGetBufferPtr(&bufferPtr).

    // if loadBlockAndGetBufferPtr(&bufferPtr) != SUCCESS
        // return the value returned by the call.

    // if the indexNum is not in the valid range of 0-(MAX_KEYS_LEAF-1), return E_OUTOFBOUND.

    // copy the struct Index pointed by ptr to indexNum'th entry in block.

    // update dirty bit.
    // if setDirtyBit failed, return the value returned by the call

    //return SUCCESS

}