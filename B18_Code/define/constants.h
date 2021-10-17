#ifndef B18_CODE_CONSTANTS_H
#define B18_CODE_CONSTANTS_H

#define DISK_PATH "../Disk/disk"
#define FILES_PATH "../Files/"

#define BLOCK_SIZE 2048
#define ATTR_SIZE 16
#define DISK_SIZE 16 * 1024 * 1024
#define HEADER_SIZE 32
#define DISK_BLOCKS 8192
#define MAX_OPEN 12
#define BLOCK_ALLOCATION_MAP_SIZE 4

#define RELCAT_BLOCK 4
#define ATTRCAT_BLOCK 5

#define NO_OF_ATTRS_RELCAT_ATTRCAT 6
#define SLOTMAP_SIZE_RELCAT_ATTRCAT 20

#define SUCCESS 0
#define FAILURE -1
#define EXIT -2

#define SLOT_OCCUPIED '1'
#define SLOT_UNOCCUPIED '0'

#define OCCUPIED 1
#define FREE 0


// TODO : use enum here

// TODO : add type for slot map blocks
#define REC 0
#define IND_INTERNAL 1
#define IND_LEAF 2
#define UNUSED_BLK 3

#define EQ 101
#define LE 102
#define LT 103
#define GE 104
#define GT 105
#define NE 106 //if considered
#define RST 100 //reset op.
#define PRJCT 107

#define INT 0
#define NUMBER 3
#define STRING 1
#define FLOAT 2

#define RELCAT_RELID 0
#define ATTRCAT_RELID 1

#define TEMP "temp"

#endif //B18_CODE_CONSTANTS_H
