//
// Created by Gokul Sreekumar on 17/08/21.
//

#ifndef B18_CODE_BLOCK_ACCESS_H
#define B18_CODE_BLOCK_ACCESS_H

int ba_insert(int relid, Attribute *rec);
int ba_search(relId relid, union Attribute *record, char attrName[ATTR_SIZE], union Attribute attrval, int op, recId *prev_recid);
recId linear_search(relId relid, char attrName[ATTR_SIZE], union Attribute attrval, int op, recId *prev_recid);
int ba_renamerel(char oldName[ATTR_SIZE], char newName[ATTR_SIZE]);
int ba_renameattr(char relName[ATTR_SIZE], char oldName[ATTR_SIZE], char newName[ATTR_SIZE]);
int ba_delete(char relName[ATTR_SIZE]);

void add_disk_metainfo();
HeadInfo getHeader(int blockNum);
void setHeader(struct HeadInfo *header, int blockNum);
void getSlotmap(unsigned char *SlotMap, int blockNum);
void setSlotmap(unsigned char *SlotMap, int no_of_slots, int blockNum);
int getRecord(Attribute *rec, int blockNum, int slotNum);
int setRecord(Attribute *rec, int blockNum, int slotNum);
int getRelCatEntry(int relationId, Attribute *relcat_entry);
int getAttrCatEntry(int relationId, char attrname[16], Attribute *attrcat_entry);
int getAttrCatEntry(int relationId, int offset, Attribute *attrCatEntry);
int setRelCatEntry(int relationId, Attribute *relcat_entry);
int setAttrCatEntry(int relationId, char attrName[ATTR_SIZE], Attribute *attrCatEntry);

int getFreeBlock(int block_type);
int getBlockType(int blocknum);
//InternalEntry getEntry(int block, int entry_number);
int compareAttributes(union Attribute attr1, union Attribute attr2, int attrType);
int deleteBlock(int blockNum);

#endif //B18_CODE_BLOCK_ACCESS_H
