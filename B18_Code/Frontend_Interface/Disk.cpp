//
// Created by Gokul Sreekumar on 31/10/21.
//

#include <iostream>
#include "../define/constants.h"
#include "../define/errors.h"
#include "Disk.h"

int Disk::readBlock(unsigned char *block, int blockNum) {
    FILE *disk = fopen(DISK_PATH, "rb");
    if (blockNum < 0 || blockNum > 8191) {
        return E_OUTOFBOUND;
    }
    const int offset = blockNum * BLOCK_SIZE;
    fseek(disk, offset, SEEK_SET);
    fread(block, BLOCK_SIZE, 1, disk);
    fclose(disk);
    return SUCCESS;
}

int Disk::writeBlock(unsigned char *block, int blockNum) {
    FILE *disk = fopen(DISK_PATH, "wb");
    int offset = blockNum * BLOCK_SIZE;
    fseek(disk, offset, SEEK_SET);
    fwrite(block, BLOCK_SIZE, 1, disk);
    fclose(disk);
}