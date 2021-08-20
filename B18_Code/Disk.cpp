//
// Created by Gokul Sreekumar on 06/06/21.
//
#include <cstdio>
#include "define/constants.h"
#include "Disk.h"

Disk::Disk() {
	const int offset = DISK_SIZE; //16MB
	FILE *disk = fopen("disk","wb+");
	fseek(disk, 0, SEEK_SET);
	// TODO: Use memset here
	for(int i=0; i < offset; i++){
		fputc(0,disk);
	}
	fclose(disk);
}

Disk::~Disk() {

}

int Disk::readBlock(unsigned char *block, int blockNum) {
	FILE *disk = fopen("disk","rb");
	const int offset = blockNum * BLOCK_SIZE;
	fseek(disk, offset, SEEK_SET);
	fread(block, BLOCK_SIZE, 1, disk);
	fclose(disk);
}

int Disk::writeBlock(unsigned char *block, int blockNum) {
	FILE *disk = fopen("disk","wb");
	int offset = blockNum * BLOCK_SIZE;
	fseek(disk, offset, SEEK_SET);
	fwrite(block, BLOCK_SIZE, 1, disk);
	fclose(disk);
}

/*
 * Formats the disk
 * Set the reserved entries in block allocation map
 */
void Disk::formatDisk() {
	FILE *disk = fopen("disk", "wb+");
	const int reserved = 6;
	const int offset = DISK_SIZE;

	fseek(disk, 0, SEEK_SET);
	unsigned char ch[BLOCK_SIZE * 4];

	// Reserved Entries in Block Allocation Map (Used)
	for (int i = 0; i < reserved; i++) {
		if (i >= 0 && i <= 3)
			ch[i] = (unsigned char) 1;
		else
			ch[i] = (unsigned char) REC;
	}

	// Remaining Entries in Block Allocation Map are marked Unused
	for (int i = reserved; i < BLOCK_SIZE * 4; i++)
		ch[i] = (unsigned char) UNUSED_BLK;
	fwrite(ch, BLOCK_SIZE * 4, 1, disk);

	// Remaining Locations of Disk initialised to 0
	for (int i = BLOCK_SIZE * 4; i < offset; i++) {
		fputc(0, disk);
	}
	fclose(disk);
}