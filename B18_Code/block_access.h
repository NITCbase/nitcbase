//
// Created by Gokul Sreekumar on 17/08/21.
//

#ifndef B18_CODE_BLOCK_ACCESS_H
#define B18_CODE_BLOCK_ACCESS_H

int ba_insert(int relid, Attribute *rec);
int ba_search(relId relid, union Attribute *record, char attrName[ATTR_SIZE], union Attribute attrval, int op, recId *prev_recid);
int ba_delete(char relName[ATTR_SIZE]);

void add_disk_metainfo();
HeadInfo getHeader(int blockNum);
void getSlotmap(unsigned char *SlotMap, int blockNum);
int getRecord(Attribute *rec, int blockNum, int slotNum);
int getRelCatEntry(int relationId, Attribute *relcat_entry);
int getAttrCatEntry(int relationId, int offset, Attribute *attrcat_entry);
int setRelCatEntry(int relationId, Attribute *relcat_entry);

#endif //B18_CODE_BLOCK_ACCESS_H
