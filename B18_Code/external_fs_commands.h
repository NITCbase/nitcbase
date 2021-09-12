//
// Created by Jessiya Joy on 19/08/21.
//

#ifndef B18_CODE_EXTERNAL_FS_COMMANDS_H
#define B18_CODE_EXTERNAL_FS_COMMANDS_H

void dump_relcat();
void dump_attrcat();
void dumpBlockAllocationMap();
void ls();
int exportRelation(char *relname, char *filename);

#endif //B18_CODE_EXTERNAL_FS_COMMANDS_H
