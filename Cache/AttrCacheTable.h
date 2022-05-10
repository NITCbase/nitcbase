#ifndef NITCBASE_ATTRCACHETABLE_H
#define NITCBASE_ATTRCACHETABLE_H

#include "../define/constants.h"
#include "../define/errors.h"
#include "../Buffer/BlockBuffer.h"
#include "../define/id.h"
#include <cstring>

typedef struct AttrCatEntry {

	char relName[ATTR_SIZE];
	char attrName[ATTR_SIZE];
	int attrType;
	bool primaryFlag;
	int rootBlock;
	int offset;

} AttrCatEntry;

typedef struct AttrCacheEntry {

	AttrCatEntry attrCatEntry;
	bool dirty;
	RecId recId;
	IndexId searchIndex;
	struct AttrCacheEntry *next;

} AttrCacheEntry;

class AttrCacheTable {

	friend class OpenRelTable;

public:
	//methods
	static int getAttrCatEntry(int relId, char attrName[ATTR_SIZE], AttrCatEntry *attrCatBuf);
	static int getAttrCatEntry(int relId, int attrOffset, AttrCatEntry *attrCatBuf);
	static int setAttrCatEntry(int relId, char attrName[ATTR_SIZE], AttrCatEntry *attrCatBuf);
	static int setAttrCatEntry(int relId, int attrOffset, AttrCatEntry *attrCatBuf);
	static int getSearchIndex(int relId, char attrName[ATTR_SIZE], IndexId *searchIndex);
	static int getSearchIndex(int relId, int attrOffset, IndexId *searchIndex);
	static int setSearchIndex(int relId, char attrName[ATTR_SIZE], IndexId *searchIndex);
	static int setSearchIndex(int relId, int attrOffset, IndexId *searchIndex);

private:
	//field
	static AttrCacheEntry* attrCache[MAX_OPEN];

	//methods
	static void recordToAttrCacheEntry(union Attribute record[ATTRCAT_NO_ATTRS], AttrCacheEntry *attrCacheEntry);
	static void attrCacheEntryToRecord(union Attribute record[ATTRCAT_NO_ATTRS], AttrCacheEntry *attrCacheEntry);

};

#endif //NITCBASE_ATTRCACHETABLE_H
