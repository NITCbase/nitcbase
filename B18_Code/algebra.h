//
// Created by Jessiya Joy on 12/09/21.
//

#ifndef B18_CODE_ALGEBRA_H
#define B18_CODE_ALGEBRA_H

int insert(std::vector<std::string> attributeTokens, char *table_name);
int insert(char relName[16], char *filename);
int checkAttrTypeOfValue(char *data);
int constructRecordFromAttrsArray(int numAttrs, Attribute record[numAttrs], char recordArray[numAttrs][ATTR_SIZE], int attrTypes[numAttrs]);

#endif //B18_CODE_ALGEBRA_H
