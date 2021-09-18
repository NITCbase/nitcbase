//
// Created by Jessiya Joy on 12/09/21.
//

#ifndef B18_CODE_ALGEBRA_H
#define B18_CODE_ALGEBRA_H
#include <vector>
#include <string>

int project(char srcrel[ATTR_SIZE], char targetrel[ATTR_SIZE], int tar_nAttrs, char tar_attrs[][ATTR_SIZE]);
int select(char srcrel[ATTR_SIZE], char targetrel[ATTR_SIZE], char attr[ATTR_SIZE], int op, char val_str[ATTR_SIZE]);
int insert(std::vector<std::string> attributeTokens, char *table_name);
int insert(char relName[ATTR_SIZE], char *fileName);
int checkAttrTypeOfValue(char *data);
int constructRecordFromAttrsArray(int numAttrs, Attribute record[], char recordArray[][ATTR_SIZE], int attrTypes[]);
int join(char srcrel1[ATTR_SIZE], char srcrel2[ATTR_SIZE], char targetRelation[ATTR_SIZE], char attr1[ATTR_SIZE], char attr2[ATTR_SIZE]);

#endif //B18_CODE_ALGEBRA_H
