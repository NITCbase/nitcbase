//
// Created by Jessiya Joy on 17/08/21.
//
#include <stdio.h>
#include <string>
#include "define/constants.h"
#include "define/errors.h"
#include "disk_structures.h"

HeadInfo getHeader(int blockNum);
void setHeader(struct HeadInfo *header, int blockNum);
void getSlotmap(unsigned char * SlotMap,int blockNum);
void setSlotmap(unsigned char * SlotMap,int no_of_slots,int blockNum);
int getFreeRecBlock();
recId getFreeSlot(int block_num);
int getRecord(Attribute *rec, int blockNum, int slotNum);
int setRecord(union Attribute *rec,int blockNum,int slotNum);
int getRelCatEntry(int rel_id, Attribute *relcat_entry);
int setRelCatEntry(int rel_id,union Attribute * relcat_entry);

/* Jess
 *
 */
int ba_insert(int relid, Attribute *rec)
{
	Attribute relcat_entry[6];
	getRelCatEntry(relid, relcat_entry);

	int num_attrs = relcat_entry[1].nval;
	int first_block = relcat_entry[3].nval;
	int num_slots = relcat_entry[5].nval;

	Attribute attrcat_entry[6];
	HeadInfo header;

	unsigned char slotmap[num_slots];
	int blkno = first_block;

	if(first_block == -1)
	{
		blkno = getFreeRecBlock();
		relcat_entry[3].nval = blkno;
		HeadInfo *H = (HeadInfo*)malloc(sizeof(HeadInfo));
		H->blockType = REC;
		H->pblock = -1;
		H->lblock = -1;
		H->rblock = -1;
		H->numEntries = 0;
		H->numAttrs = num_attrs;
		H->numSlots = num_slots;
		setHeader(H, blkno);
		getSlotmap(slotmap, blkno);
		memset(slotmap, '0', sizeof(slotmap)); //all slots are free
		setSlotmap(slotmap, num_slots, blkno);
	}

	recId recid = getFreeSlot(blkno);

	relcat_entry[4].nval = recid.block;
	//cout<<"blk no: "<<relcat_entry[4].nval<<" ";
	setRelCatEntry(relid, relcat_entry);
	getRelCatEntry(relid, relcat_entry);
	//cout<<relcat_entry[3].nval<<"\n";
	if(recid.block == -1 && recid.slot == -1)
	{         //free slot can not be found
		return FAILURE;
	}
	setRecord(rec,recid.block, recid.slot);

	//since record is inserted number of entries is increased by 1
	header = getHeader(recid.block);  //arg
	header.numEntries = header.numEntries + 1; // increased number of entires in block
	setHeader(&header, recid.block); //arg

	//increasing number of entries in relation catalog entry
	relcat_entry[2].nval = relcat_entry[2].nval + 1;
	setRelCatEntry(relid, relcat_entry);

	return SUCCESS;
}

/* Jess
 * Reads header for 'blockNum'th block from disk
 */
HeadInfo getHeader(int blockNum)
{
	HeadInfo header;
	FILE *disk = fopen("disk", "rb");
	fseek(disk, blockNum*BLOCK_SIZE, SEEK_SET);
	fread(&header, 32, 1, disk);
	fclose(disk);
	return header;
}

/* Jess
 * Writes header for 'blockNum'th block into disk
 */
void setHeader(struct HeadInfo *header, int blockNum)
{
	FILE *disk = fopen("disk", "rb+");
	fseek(disk, blockNum*BLOCK_SIZE, SEEK_SET);
	fwrite(header, 32, 1, disk);
	fclose(disk);
}

/* Jess
 *
 */
void getSlotmap(unsigned char * SlotMap,int blockNum)
{
	FILE *disk = fopen("disk", "rb+");
	fseek(disk, blockNum*BLOCK_SIZE, SEEK_SET);
	RecBlock R;
	fread(&R, BLOCK_SIZE, 1, disk);
	int numSlots = R.numSlots;
	memcpy(SlotMap, R.slotMap_Records, numSlots);
	fclose(disk);
}

/* Jess
 *
 */
void setSlotmap(unsigned char * SlotMap,int no_of_slots,int blockNum)
{
	FILE *disk = fopen("disk", "rb+");
	fseek(disk, blockNum*BLOCK_SIZE + 32, SEEK_SET);
	fwrite(SlotMap, no_of_slots, 1, disk);
	fclose(disk);
}

/* Jess
 *
 */
int getFreeRecBlock()
{
	FILE *disk = fopen("disk","rb+");
	fseek(disk, 0, SEEK_SET);
	unsigned char blockAllocationMap[4*BLOCK_SIZE];
	fread(blockAllocationMap, 4*BLOCK_SIZE, 1, disk);

	int iter;
	for(iter = 0; iter < 4*BLOCK_SIZE; iter++)
	{
		if((int32_t)(blockAllocationMap[iter]) == UNUSED_BLK)
		{
			//cout<<"in getfreerecblk: "<<"\n";
			blockAllocationMap[iter] = (unsigned char)REC;
			fseek(disk, 0, SEEK_SET);
			fwrite(blockAllocationMap,BLOCK_SIZE*4, 1, disk);
			fclose(disk);
			return iter;
		}
	}

	return FAILURE;
}

/* Jess
 *
 */
recId getFreeSlot(int block_num)
{
	int arg_blk=block_num;
	recId recid = {-1, -1};
	int prev_block_num, next_block_num;
	int num_slots;
	int iter,num_attrs;
	struct HeadInfo header;
	// finding free slot
	while(block_num != -1)
	{
		//rec_buffer = Buffer::getRecBuffer(block_num);
		//header = rec_buffer->getheader();
		header=getHeader(block_num);
		num_slots = header.numSlots;
		next_block_num = header.rblock;
		num_attrs=header.numAttrs;
		// next block
		//getting slotmap entries
		unsigned char slotmap[num_slots];
		getSlotmap(slotmap,block_num);
		//searching for free slot in block (block_num)
		int iter;
		for(iter = 0; iter < num_slots; iter++)
		{
			if(slotmap[iter] == '0')
			{
				break;
			}
		}
		if(iter < num_slots)
		{
			slotmap[iter] = '1';
			setSlotmap(slotmap, num_slots, block_num);
			recid = {block_num, iter};
			return recid;
		}
		prev_block_num = block_num;
		block_num = next_block_num;
	}

	//no free slot is found in existing records if block_num = -1
	block_num =getFreeRecBlock();
	if(block_num ==-1)
	{
		// no blocks are available in disk
		return recid; // no free slot can be found
	}
	//block_num = rec_buffer->getBlockNum();
	//setting header values
	header = getHeader(block_num);
	header.lblock = prev_block_num;
	header.numSlots = num_slots;
	header.rblock=-1;
	//was not there before: Athira
	header.numAttrs=num_attrs;
	setHeader(&header,block_num);
	//setting slotmap
	unsigned char slotmap[num_slots];
	getSlotmap(slotmap,block_num);
	memset(slotmap,'0', sizeof(slotmap)); //all slots are free
	slotmap[0]='1';
	setSlotmap(slotmap,num_slots,block_num);
	//delete rec_buffer;
	//recid
	recid = {block_num, 0};
	//setting prev_block_num rblock to new block
	header=getHeader(prev_block_num);
	header.rblock = block_num;
	setHeader(&header,prev_block_num);
	return recid;
}

/* Jess
 * Stores the record given by blockNum & slotNum in the memory location pointed by rec
 */
int getRecord(Attribute *rec, int blockNum, int slotNum)
{
	HeadInfo Header;
	Header = getHeader(blockNum);
	int numOfSlots = Header.numSlots;

	if(slotNum < 0 || slotNum > (numOfSlots - 1))
		return E_OUTOFBOUND;

	FILE *disk=fopen("disk", "rb+");
	int BlockType = getBlockType(blockNum);

	if(BlockType == REC)
	{
		RecBlock R;
		fseek(disk, blockNum*BLOCK_SIZE, SEEK_SET);
		fread(&R, BLOCK_SIZE, 1, disk);
		int numSlots=R.numSlots;
		unsigned char slotMap[numSlots];

		if(*((int32_t*)(R.slotMap_Records+ slotNum)) == 0)
			return E_FREESLOT;
		int numAttrs = R.numAttrs;
		// it should be R.slotmprecords+32+...???
		memcpy(rec, (R.slotMap_Records+numSlots+(slotNum*numAttrs*ATTR_SIZE)), numAttrs*16);
		fclose(disk);
		return SUCCESS;
	}
	else if(BlockType == IND_INTERNAL)
	{
		//TODO

	}
	else if(BlockType == IND_LEAF)
	{
		//TODO
	}
	else
	{
		fclose(disk);
		return FAILURE;
	}
}


/* Jess
 *
 */
int setRecord(union Attribute *rec,int blockNum,int slotNum)
{
	//cout<<"setrrrr\n";
	struct HeadInfo header=getHeader(blockNum);
	int numOfSlots=header.numSlots;
	int numAttrs=header.numAttrs;
	//cout<<numOfSlots;

	if(slotNum < 0 || slotNum > numOfSlots - 1)
		return E_OUTOFBOUND;
	//cout<<"hereee!!!\n";

	int BlockType=getBlockType(blockNum);
	//cout<<BlockType<<endl;
	//cout<<"BLOCK "<<blockNum<<"\n";
	FILE *disk=fopen("disk","rb+");
	if(BlockType==REC)
	{
		/*struct recBlock R;
		fseek(disk,blockNum*BLOCK_SIZE,SEEK_SET);
		fread(&R,BLOCK_SIZE,1,disk);
		int numAttrs=R.numAttrs;
		int numSlots=R.numSlots;*/
		//if(blockNum==5&&slotNum>11)
		fseek(disk,blockNum*BLOCK_SIZE+32+numOfSlots+slotNum*numAttrs*ATTR_SIZE,SEEK_SET);
		fwrite(rec,numAttrs*ATTR_SIZE,1,disk);
		fclose(disk);
		return SUCCESS;

	}
	else if(BlockType==IND_INTERNAL)
	{
		//TODO

	}
	else if(BlockType==IND_LEAF)
	{
		//TODO
	}
	else
	{
		fclose(disk);
		return FAILURE;
	}
	/*unsigned char* bufferPtr = getBufferPtr();
	int numOfAttrib=((int32_t) (bufferPtr + 5*4));
	int numOfSlots=((int32_t) (bufferPtr + 6*4));
	if(slotNum < 0 || slotNum > numOfSlots - 1)
		return E_OUTOFBOUND;
	unsigned char *slotMap = new unsigned char;
	getSlotMap(slotMap);
	if((int32_t*)(slotMap + slotNum) == 0)
	    return E_FREESLOT;
	memcpy((void*)(bufferPtr + 32 + numOfSlots + (slotNum*numOfAttrib)ATTR_SIZE), (void)rec, numOfAttrib*ATTR_SIZE);
	return SUCCESS;*/
}

/* Jess
 * Stores the relation catalogue entry of the relation with id rel_id
 * in memory location pointed to by relcat_entry
 */
int getRelCatEntry(int rel_id, Attribute *relcat_entry)
{
	if(rel_id< 0 || rel_id >= MAXOPEN)
		return E_OUTOFBOUND;

	if(strcmp(OpenRelTable[rel_id], "NULL") == 0)
		return E_NOTOPEN;

	char rel_name[16];
	strcpy(rel_name, OpenRelTable[rel_id]);

	for(int i = 0; i < 20; i++)
	{
		getRecord(relcat_entry, 4, i);
		if(strcmp(relcat_entry[0].sval, rel_name) == 0)
			return SUCCESS;
	}
	return FAILURE;
}

/* Jess
 *
 */
int setRelCatEntry(int rel_id,union Attribute * relcat_entry)
{
	if(rel_id< 0 || rel_id>= MAXOPEN)
		return E_OUTOFBOUND;
	if(strcmp(OpenRelTable[rel_id],"NULL")==0)
		return E_NOTOPEN;
	char rel_name[16];

	strcpy(rel_name, OpenRelTable[rel_id]);
	union Attribute  relcat_entry1[6];
	for(int i=0;i<20;i++)
	{
		getRecord(relcat_entry1,4,i);
		if(strcmp(relcat_entry1[0].sval,rel_name)==0)
		{
			setRecord(relcat_entry,4,i);
			return SUCCESS;
		}
	}
}
