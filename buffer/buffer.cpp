/* This file contains code for all member functions of 'Buffer' class */
#include <cstdint>
#include "buffer.h"
#include "../define/constants.h"
#include "blockBuffer.h"
#include "../disk/disksimulator.cpp"
#include <cstring>

Buffer::Buffer(){
	int iter;
	for(iter=0;iter<4;iter++){
		readblock(block_alloc_map+iter*BLOCK_SIZE,iter);
	}

	for(iter=0;iter<32;iter++){
		metainfo[iter].dirty=false;
		metainfo[iter].free=true;
	}
}

Buffer::~Buffer(){
	int iter;
	for(iter=0;iter<4;iter++){
		writeblock(iter,(void*)(block_alloc_map+iter*BLOCK_SIZE));
	}
	for(iter=0;iter<32;iter++){
		if(!metainfo[iter].free && metainfo[iter].dirty){
			writeblock(metainfo[iter].block_num,(void*)blocks[iter]);
		}
	}

}

int Buffer::getBlockType(int buffer_index){
	int block_type = *(int32_t *)&blocks[buffer_index][0];
	return block_type;
}

int Buffer::getFreeBuffer(){
	int iter;
	for(iter=0;iter<32;iter++){
		if(metainfo[iter].free){
			return iter;
		}
	}

	if(iter==32){
		//use replacement to get free buffer block
	}
	//return -1; ///no free buffer
}

int Buffer::getbufferblock(int block){
	int iter;
	for(iter=0;iter<32;iter++){
		if(!metainfo[iter].free && metainfo[iter].block_num==block){
			return iter;
		}
	}
	return -1; //disk 'block' is not found in buffer
}

int Buffer::loadBlock(int block_num){
	int free_buffer=getFreeBuffer();// always gives free buffer no. even if buffer is full(replacement)
	
	//////code to copy block from disk to buffer////////////////

	metainfo[free_buffer].free=false;
	metainfo[free_buffer].dirty=false;
	metainfo[free_buffer].block_num=block_num;

	int block_type = *(int32_t *)(&blocks[free_buffer][0]);
	return free_buffer;
}

void Buffer::releaseBufferBlock(int buffer_index){
	metainfo[buffer_index].free=true;
	metainfo[buffer_index].dirty=false;
	/// also free metainfo[i].blk;/// also free metainfo[i].blk;metainfo[i].blk=NULL;
	metainfo[buffer_index].block_num=-1;
	return;
}

unsigned char* Buffer::get_buf_dataptr(int block_num){
	int buffer_block=getbufferblock(block_num);
	if(buffer_block==-1){
		buffer_block=loadBlock(block_num);

	}
	
	return &(blocks[buffer_block][0]);
}

//Public functions

class RecBuffer * Buffer::getFreeRecBlock(){
	int iter;
	for(iter=4;iter<DISK_BLOCKS;iter++){
		if(block_alloc_map[iter]==0){
			break;
		}
	}

	if(iter == DISK_BLOCKS){
		return NULL;
	}else{
		int FreeBuffer=getFreeBuffer();
		if(FreeBuffer==-1){ //To be changed -> replaacement is done
			return NULL;
		}
		*(int32_t *)&blocks[FreeBuffer][0]=REC;
		*(int32_t *)&blocks[FreeBuffer][16]=0; //#entries=0
		
		metainfo[FreeBuffer].free=false;
		metainfo[FreeBuffer].dirty=true;
		metainfo[FreeBuffer].block_num=iter;
		class RecBuffer* newRecBuffer= new RecBuffer(iter);
		//metainfo[FreeBuffer].blk=newRecBuffer;

		block_alloc_map[iter]=1;

		return newRecBuffer;
	}
}

class IndBuffer * Buffer::getFreeIndInternal(){
	int iter;
	for(iter=4;iter<DISK_BLOCKS;iter++){
		if(block_alloc_map[iter]==0){
			break;
		}
	}

	if(iter == DISK_BLOCKS){
		return NULL;
	}else{
		int FreeBuffer=getFreeBuffer();
		if(FreeBuffer==-1){///no free buffer found -- Replacement must be done
			return NULL;
		}
		*(int32_t *)&blocks[FreeBuffer][0]=IND_INTERNAL;
		*(int32_t *)&blocks[FreeBuffer][16]=0; //#entries=0
		*(int32_t *)&blocks[FreeBuffer][4]=-1; //pblock=-1

		metainfo[FreeBuffer].free=false;
		metainfo[FreeBuffer].dirty=true;
		metainfo[FreeBuffer].block_num=iter;

		class IndBuffer* newIndBuffer= new IndInternal(iter);
		//metainfo[FreeBuffer].blk=newIndBuffer;

		block_alloc_map[iter]=1;

		return newIndBuffer;
	}
}

class IndBuffer * Buffer::getFreeIndLeaf(){
	int iter;
	for(iter=4;iter<DISK_BLOCKS;iter++){
		if(block_alloc_map[iter]==0){
			break;
		}
	}

	if(iter == DISK_BLOCKS){
		return NULL;
	}else{
		int FreeBuffer=getFreeBuffer();
		if(FreeBuffer==-1){///no free buffer found -- Replacement must be done
			return NULL;
		}
		*(int32_t *)&blocks[FreeBuffer][0]=IND_LEAF;
		*(int32_t *)&blocks[FreeBuffer][16]=0; //#entries=0
		*(int32_t *)&blocks[FreeBuffer][4]=-1; //pblock=-1
		
		metainfo[FreeBuffer].free=false;
		metainfo[FreeBuffer].dirty=true;
		metainfo[FreeBuffer].block_num=iter;
		class IndBuffer* newIndBuffer= new IndLeaf(iter);
		//metainfo[FreeBuffer].blk=newIndBuffer;

		block_alloc_map[iter]=1;

		return newIndBuffer;
	}
}

class RecBuffer * Buffer::getRecBlock(int block_num){
	int buffer_block=getbufferblock(block_num); //if block is already in buffer

	if(buffer_block==-1){
		buffer_block=loadBlock(block_num); //replacement will be done in this
	}

	if(buffer_block!=-1){
		int blk_type=getBlockType(buffer_block);
		if(blk_type==REC){
			class RecBuffer* newRecBuffer= new RecBuffer(block_num);
			return newRecBuffer;
		}else{
		return NULL;
		}
	}else{ //will not be required if replacement is done.
		return NULL;
	}
}

class IndBuffer * Buffer::getIndBlock(int block_num){
	int buffer_block=getbufferblock(block_num);

	if(buffer_block==-1){
		buffer_block=loadBlock(block_num);
	}

	if(buffer_block!=-1){
		int blk_type=getBlockType(buffer_block);
		if(blk_type==IND_INTERNAL){
			class IndBuffer* newIndBuffer= new IndInternal(block_num);
			return newIndBuffer;
		}else if(blk_type==IND_LEAF){
			class IndBuffer* newIndBuffer= new IndLeaf(block_num);
			return newIndBuffer;
		}else{
			return NULL; //returns NULL if block found to be other than internal or leaf block(i.e recblock)
		}
		
	}else{ //will not be required if replacement is done.
		return NULL;
	}
}


void Buffer::releaseBlock(int block_num){ //To release and commit to disk.
	if(block_num < 0 || block_num >= DISK_BLOCKS){
		return;  
	}

	int buffer_block=getbufferblock(block_num);

	if(buffer_block!=-1){
		if(metainfo[buffer_block].dirty){
		 int write_to_disk=writeblock(block_num, (void*) (blocks[buffer_block]));
		}
		releaseBufferBlock(buffer_block);
	}

}

void Buffer::deleteBlock(int block_num){ //To completely free any block.
	if(block_num < 0 || block_num >= DISK_BLOCKS){
		return;  
	}

	int buffer_block=getbufferblock(block_num);

	if(buffer_block!=-1){
		releaseBufferBlock(buffer_block);
	}

	block_alloc_map[block_num]=UNUSED;
}
