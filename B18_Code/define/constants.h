#ifndef B18_CODE_CONSTANTS_H
#define B18_CODE_CONSTANTS_H

#define DISK_PATH "../Disk/disk"
#define DISK_RUN_COPY_PATH "../Disk/disk_run_copy"
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
#define INTERNAL_ENTRY_SIZE 20
#define LEAF_ENTRY_SIZE 32

#define SUCCESS 0
#define FAILURE -1
#define EXIT -2

#define SLOT_OCCUPIED '1'
#define SLOT_UNOCCUPIED '0'

#define OCCUPIED 1
#define FREE 0

#define INT_MAX 2147483647

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

#define NUMBER 0
#define STRING 1

#define RELCAT_RELID 0
#define ATTRCAT_RELID 1

#define TEMP "temp"

#define RELCAT_REL_NAME_INDEX 0
#define	RELCAT_NO_ATTRIBUTES_INDEX 1
#define	RELCAT_NO_RECORDS_INDEX 2
#define	RELCAT_FIRST_BLOCK_INDEX 3
#define	RELCAT_LAST_BLOCK_INDEX 4
#define	RELCAT_NO_SLOTS_PER_BLOCK_INDEX 5

#define ATTRCAT_REL_NAME_INDEX 0
#define	ATTRCAT_ATTR_NAME_INDEX 1
#define	ATTRCAT_ATTR_TYPE_INDEX 2
#define	ATTRCAT_PRIMARY_FLAG_INDEX 3
#define	ATTRCAT_ROOT_BLOCK_INDEX 4
#define	ATTRCAT_OFFSET_INDEX 5

#define MAX_KEYS_INTERNAL 100
#define MIDDLE_INDEX_INTERNAL 50
#define MAX_KEYS_LEAF 63
#define MIDDLE_INDEX_LEAF 31

//#define MAX_KEYS_INTERNAL 4
//#define MIDDLE_INDEX_INTERNAL 2
//#define MAX_KEYS_LEAF 3
//#define MIDDLE_INDEX_LEAF 1

//#define MAX_KEYS_INTERNAL 12
//#define MIDDLE_INDEX_INTERNAL 6
//#define MAX_KEYS_LEAF 12
//#define MIDDLE_INDEX_LEAF 6

#endif //B18_CODE_CONSTANTS_H