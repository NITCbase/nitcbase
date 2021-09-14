//
// Created by Jessiya Joy on 12/09/21.
//
#include <iostream>
#include <string>
#include <vector>
#include "define/constants.h"
#include "define/errors.h"
#include "disk_structures.h"
#include "algebra.h"
#include "block_access.h"
#include "OpenRelTable.h"

using namespace std;

int check_type(char *data);

int getNumberOfAttrsForRelation(int relationId);

void getAttrTypesForRelation(int relId, int numAttrs, int attrTypes[numAttrs]);

int constructRecordFromAttrsArray(int numAttrs, Attribute record[numAttrs], char recordArray[numAttrs][ATTR_SIZE],
                                  int attrTypes[numAttrs]);

int insert(vector<string> attributeTokens, char *table_name) {

	// check if relation is open
	int relId = OpenRelations::getRelationId(table_name);
	if (relId == E_RELNOTOPEN) {
		return relId;
	}

	// get #attributes from relation catalog entry
	int numAttrs = getNumberOfAttrsForRelation(relId);
	if (numAttrs != attributeTokens.size())
		return E_NATTRMISMATCH;

	// get attribute types from attribute catalog entry
	int attrTypes[numAttrs];
	getAttrTypesForRelation(relId, numAttrs, attrTypes);

	// for each attribute, convert string vector to char array
	char recordArray[numAttrs][ATTR_SIZE];
	for (int i = 0; i < numAttrs; i++) {
		string attrValue = attributeTokens[i];
		char tempAttribute[ATTR_SIZE];
		int j;
		for (j = 0; j < 15 && j < attrValue.size(); j++) {
			tempAttribute[j] = attrValue[j];
		}
		tempAttribute[j] = '\0';
		strcpy(recordArray[i], tempAttribute);
	}

	// Construct a record ( array of type Attribute ) from previous character array
	// Perform type checking for integer types
	Attribute record[numAttrs];
	int retValue = constructRecordFromAttrsArray(numAttrs, record, recordArray, attrTypes);
	if (retValue == E_ATTRTYPEMISMATCH)
		return E_ATTRTYPEMISMATCH;

	retValue = ba_insert(relId, record);
	if (retValue == SUCCESS) {
		return SUCCESS;
	} else {
		return FAILURE;
	}
}

int insert(char relName[16], char *filename) {
	FILE *file = fopen(filename, "r");
	char ch;

	// check if relation is open
	int relId = OpenRelations::getRelationId(relName);
	if (relId == E_RELNOTOPEN) {
		return relId;
	}

	// get #attributes from relation catalog entry
	int numAttrs = getNumberOfAttrsForRelation(relId);

	// get attribute types from attribute catalog entry
	int attrTypes[numAttrs];
	getAttrTypesForRelation(relId, numAttrs, attrTypes);


	char *record = (char *) malloc(sizeof(char));
	int len = 1;
	while (1) {
		ch = fgetc(file);
		if (ch == EOF)
			break;
		while (ch == ' ' || ch == '\t' || ch == '\n') {
			ch = fgetc(file);
		}
		if (ch == EOF)
			break;
		len = 1;
		int fieldsCount = 0;
		char oldch = ',';
		while ((ch != '\n') && (ch != EOF)) {
			if (ch == ',')
				fieldsCount++;

			if (oldch == ch && ch == ',') {
				cout << "Null values not allowed\n";
				return FAILURE;
			}

			record[len - 1] = ch;
			len++;
			record = (char *) realloc(record, (len) * sizeof(char));
			oldch = ch;
			ch = fgetc(file);

		}

		if (oldch == ',' && ch != '\n') {
			cout << "Null values not allowed in attribute values\n";
			return FAILURE;
		}

		if (numAttrs != fieldsCount + 1 && ch != '\n') {
			std::cout << "Mismatch in number of attributes\n";
			return FAILURE;
		}
		record[len - 1] = '\0';
		int i = 0;
		//record contains each record in the file (seperated by commas)
		char recordArray[numAttrs][ATTR_SIZE];
		int j = 0;

		while (j < numAttrs) {
			int k = 0;

			while (((record[i] != ',') && (record[i] != '\0')) && (k < 15)) {
				recordArray[j][k++] = record[i++];
			}
			if (k == 15) {
				while (record[i] != ',')
					i++;
			}
			i++;
			recordArray[j][k] = '\0';
			j++;
		}

		// Construct a record ( array of type Attribute ) from previous character array
		// Perform type checking for integer types
		Attribute record[numAttrs];
		int retValue = constructRecordFromAttrsArray(numAttrs, record, recordArray, attrTypes);
		if (retValue == E_ATTRTYPEMISMATCH)
			return E_ATTRTYPEMISMATCH;

		retValue = ba_insert(relId, record);
		if (retValue != SUCCESS) {
			return FAILURE;
		}

		if (ch == EOF)
			break;
	}

	fclose(file);
	return SUCCESS;
}

// TODO : Find library functions for this?
int check_type(char *data) {
	int count_int = 0, count_dot = 0, count_string = 0, i;
	for (i = 0; data[i] != '\0'; i++) {

		if (data[i] >= '0' && data[i] <= '9')
			count_int++;
		if (data[i] == '.')
			count_dot++;
		else
			count_string++;
	}

	if (count_dot == 1 && count_int == (strlen(data) - 1))
		return NUMBER;
	if (count_int == strlen(data)) {
		return NUMBER;
	} else
		return STRING;
}

int getNumberOfAttrsForRelation(int relationId) {
	Attribute relCatEntry[NO_OF_ATTRS_RELCAT_ATTRCAT];
	int retValue;
	retValue = getRelCatEntry(relationId, relCatEntry);
	if (retValue != SUCCESS) {
		return retValue;
	}
	int numAttrs = static_cast<int>(relCatEntry[1].nval);
	return numAttrs;
}

void getAttrTypesForRelation(int relId, int numAttrs, int attrTypes[numAttrs]) {

	Attribute attrCatEntry[NO_OF_ATTRS_RELCAT_ATTRCAT];

	for (int offsetIter = 0; offsetIter < numAttrs; ++offsetIter) {
		getAttrCatEntry(relId, offsetIter, attrCatEntry);
		attrTypes[static_cast<int>(attrCatEntry[5].nval)] = static_cast<int>(attrCatEntry[2].nval);
	}
}

int constructRecordFromAttrsArray(int numAttrs, Attribute record[numAttrs], char recordArray[numAttrs][ATTR_SIZE],
                                  int attrTypes[numAttrs]) {
	for (int l = 0; l < numAttrs; l++) {

		if (attrTypes[l] == NUMBER) {
			if (check_type(recordArray[l]) == NUMBER)
				record[l].nval = atof(recordArray[l]);
			else
				return E_ATTRTYPEMISMATCH;
		}

		if (attrTypes[l] == STRING) {
			strcpy(record[l].sval, recordArray[l]);
		}
	}
	return SUCCESS;
}