//
// Created by Gokul Sreekumar on 31/10/21.
//

#include <iostream>
#include <fstream>
#include "../define/constants.h"
#include "../define/errors.h"
#include "Disk.h"

using namespace std;

/*
 * Used to make a temporary copy of the disk contents before the starting of a new session.
 * This ensures that if the system has a forced shutdown during the course of the session,
 * the previous state of the disk is not lost.
 */
Disk::Disk() {
    FILE *disk = fopen(DISK_PATH, "rb");
    FILE *disk_run_copy = fopen(DISK_RUN_COPY_PATH, "wb");

    /* An efficient method to copy files */
    std::ifstream  src(DISK_PATH, std::ios::binary);
    std::ofstream  dst(DISK_RUN_COPY_PATH,   std::ios::binary);

    dst << src.rdbuf();
    src.close();
    dst.close();
}

/*
 * Used to Read a specified block from disk
 * block - Memory pointer of the buffer to which the block contents is to be loaded/read.
 *         (MUST be Allocated by caller)
 * blockNum - Block number of the disk block to be read.
 */
int Disk::readBlock(unsigned char *block, int blockNum) {
    FILE *disk = fopen(DISK_RUN_COPY_PATH, "rb");
    if (blockNum < 0 || blockNum > 8191) {
        return E_OUTOFBOUND;
    }
    const int offset = blockNum * BLOCK_SIZE;
    fseek(disk, offset, SEEK_SET);
    fread(block, BLOCK_SIZE, 1, disk);
    fclose(disk);
    return SUCCESS;
}

/*
 * Used to Write a specified block from disk
 * block - Memory pointer of the buffer to which contain the contents to be written.
 *         (MUST be Allocated by caller)
 * blockNum - Block number of the disk block to be written into.
 */
int Disk::writeBlock(unsigned char *block, int blockNum) {
    FILE *disk = fopen(DISK_RUN_COPY_PATH, "wb");
    if (blockNum < 0 || blockNum > 8191) {
        return E_OUTOFBOUND;
    }
    const int offset = blockNum * BLOCK_SIZE;
    fseek(disk, offset, SEEK_SET);
    fwrite(block, BLOCK_SIZE, 1, disk);
    fclose(disk);
    return SUCCESS;
}