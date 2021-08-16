//
// Created by Gokul Sreekumar on 06/06/21.
//

#ifndef B18_CODE_DISK_H
#define B18_CODE_DISK_H


class Disk {
public:
	Disk(); // createdisk() will be this function -> remove this comment later on
	~Disk();
	static int readBlock(unsigned char *block, int blockNum); // Use this wherever a block is being written (eg. ba_insert)
	static int writeBlock(unsigned char *block, int blockNum); // Use this wherever a block is being read
	static void formatDisk();
};


#endif //B18_CODE_DISK_H
