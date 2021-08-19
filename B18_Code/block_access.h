//
// Created by Gokul Sreekumar on 17/08/21.
//

#ifndef B18_CODE_BLOCK_ACCESS_H
#define B18_CODE_BLOCK_ACCESS_H

int ba_insert(int relid, Attribute *rec);
int ba_search(relId relid, union Attribute *record, char attrName[ATTR_SIZE], union Attribute attrval, int op, recId *prev_recid);
void add_disk_metainfo();

#endif //B18_CODE_BLOCK_ACCESS_H
