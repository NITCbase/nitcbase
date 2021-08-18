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

void Disk::formatDisk() {
	// Formats the disk
	// Set the reserved entries in block allocation map

	FILE *disk = fopen("disk","wb+");
	const int reserved = 6;
	const int offset = DISK_SIZE;
	fseek(disk, 0, SEEK_SET);

	// First Five Entries set to 1
	for(int i=0; i<reserved; i++){
		fputc(1, disk);
	}
	// All other entries to zero
	for(int i=reserved; i<offset; i++){
		fputc(0, disk);
	}
	fclose(disk);
}