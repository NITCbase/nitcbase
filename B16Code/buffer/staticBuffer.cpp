#include <cstdint>
#include "staticBuffer.h"
#include "../define/constants.h"
#include "../disk/disksimulator.cpp"

StaticBuffer::StaticBuffer(){
    // copy blockAllocMap blocks from disk to buffer (using readblock() of disk)
    for(int it = 0;it<4;it++)
    {
      readblock(blockAllocMap+it*BLOCK_SIZE, it);
    }

    //initialise metainfo of all the buffer blocks with dirty:false, free:true, timestamp:-1 and blockNum:-1
    for(int it = 0;it<32;it++)
    {
      metainfo[it].dirty = false;
      metainfo[it].free = true;
      metainfo[it].timeStamp = -1;
      metainfo[it].blockNum = -1;
    }
}

StaticBuffer::~StaticBuffer(){
    // copy blockAllocMap blocks from buffer to disk(using writeblock() of disk)
    //NOTE : 2nd param should be explicitly type casted to void*
    for(int it = 0;it<4;it++)
    {
      writeblock(it, (void*)(blockAllocMap+it*BLOCK_SIZE));
    }

    /*iterate through all the buffer blocks, 
    	write back blocks with meta info as free:false,dirty:true (using writeblock() of disk)*/
    for(int it = 0;it<32;it++)
    {
      if(metainfo[it].free == false && metainfo[it].dirty == true)
          writeblock(metainfo[it].blockNum, (void*)blocks[it]);
    }
}

int StaticBuffer::getFreeBuffer(int blockNum){
    // increase the time stamps in metainfo of all the occupied buffers.
    for(int it = 0;it<32;it++)
    {
      if(metainfo[it].free == false)
        metainfo[it].timeStamp += 1;
    }
    
    // if free buffer is available, bufferIndex is the index of the free buffer.
    int bufferIndex = -1;
    for(int it = 0;it<32;it++)
    {
      if(metainfo[it].free == true){
          bufferIndex = it;
          break;
      }
    }
    
    // if free buffer is not available, replace the buffer with the largest timestamp and set it as bufferIndex.
    if(bufferIndex == -1){
        largest_timestamp = -1;
        largest_index = -1;
        for(int it = 0;it < 32;it++)
        {
            if(metainfo[it].timeStamp > largest_timestamp)
            {
              largest_timestamp =  metainfo[it].timeStamp;
              largest_index = it;
            }
        }
        writeblock(metainfo[largest_index].blockNum, (void*)blocks[largest_index]);
        bufferIndex = largest_index;
    }
  
    
    // update the metainfo array corresponding to the buffer index. 
    metainfo[bufferIndex].dirty = false;
    metainfo[bufferIndex].free = false;
    metainfo[bufferIndex].timeStamp = 0;
    metainfo[bufferIndex].blockNum = blockNum;

    
    // return the buffer index
    return bufferIndex;
}


  int StaticBuffer::getBufferNum(int blockNum){
    //traverse through the metainfo array, find the buffer index of the buffer to which the block is loaded.
     int bufferIndex = -1;
     for(int it = 0;it < 32;it++)
     {
        if(metainfo[it].blockNum == blockNum)
        {
            bufferIndex = it;
            break;
        }
     }
    
    //if found return buffer index, else indicate failure.
    if(bufferIndex == -1)
         return FAILURE;
     
    return bufferIndex;
}


int StaticBuffer::getStaticBlockType(int blockNum){
    //traverse the blockAllocMap to find the type corresponding to blockNum.
    //doubt
    
    //return the blockType obtained(REC/IND_INTERNAL/IND_LEAF/UNUSED)
    return (int)blockAllocMap[blockNum];
}

void StaticBuffer::setDirtyBit(int blockNum){
    int bufferNum = getBufferNum(blockNum);
    metainfo[bufferNum].dirty = true;
}
