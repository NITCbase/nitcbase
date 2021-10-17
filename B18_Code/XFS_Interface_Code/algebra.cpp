//
// Created by Jessiya Joy on 12/09/21.
//
#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include <unordered_set>
#include "../define/constants.h"
#include "../define/errors.h"
#include "disk_structures.h"
#include "algebra.h"
#include "block_access.h"
#include "OpenRelTable.h"
#include "schema.h"
#include "external_fs_commands.h"

int checkAttrTypeOfValue(char *data);

int getNumberOfAttrsForRelation(int relationId);

void getAttrTypesForRelation(int relId, int numAttrs, int attrTypes[]);

int constructRecordFromAttrsArray(int numAttrs, Attribute record[], char recordArray[][ATTR_SIZE], int attrTypes[]);


int project(char srcrel[ATTR_SIZE], char targetrel[ATTR_SIZE], int tar_nAttrs, char tar_attrs[][ATTR_SIZE]) {
    int ret;
    /* Check source relation is open */
    int srcrelid = OpenRelTable::getRelationId(srcrel);
    if (srcrelid == E_RELNOTOPEN) {
        // src relation not open
        return E_RELNOTOPEN;
    }

    Attribute srcrelcatEntry[NO_OF_ATTRS_RELCAT_ATTRCAT];
    getRelCatEntry(srcrelid, srcrelcatEntry);
    int nAttrs = (int) srcrelcatEntry[1].nval;

    int attr_offset[tar_nAttrs];
    int attr_type[tar_nAttrs];
    int attr_no;
    /* Check if attributes of target rel are present in source rel. */
    for (attr_no = 0; attr_no < tar_nAttrs; attr_no++) {
        Attribute attrcat[NO_OF_ATTRS_RELCAT_ATTRCAT];
        ret = getAttrCatEntry(srcrelid, tar_attrs[attr_no], attrcat);
        if (ret != SUCCESS) {
            return ret;
        }
        attr_offset[attr_no] = (int) attrcat[5].nval;
        attr_type[attr_no] = (int) attrcat[2].nval;
    }

    ret = createRel(targetrel, tar_nAttrs, tar_attrs, attr_type);
    if (ret != SUCCESS) {
        /* Unsuccessful creation of target relation */
        return ret;
    }

    /* Open the target relation */
    // TODO: MOVE THIS UP - DESIGN CHANGE
    int targetrelid = openRel(targetrel);
    if (targetrelid == E_CACHEFULL) {
        ba_delete(targetrel);
        return E_CACHEFULL;
    }

    /*
     * Get record by record from the source relation
     *  and take the projected attributes alone for the record
     */
    recId prev_recid;
    prev_recid.block = -1;
    prev_recid.slot = -1;
    while (true) {
        Attribute rec[nAttrs];
        char attr[ATTR_SIZE];
        Attribute val;
        strcpy(val.sval, "PRJCT");
        strcpy(attr, "PRJCT");

        ret = ba_search(srcrelid, rec, attr, val, PRJCT, &prev_recid);
        if (ret == SUCCESS) {
            Attribute proj_rec[tar_nAttrs];
            for (attr_no = 0; attr_no < tar_nAttrs; attr_no++) {
                proj_rec[attr_no] = rec[attr_offset[attr_no]];
            }
            ret = ba_insert(targetrelid, proj_rec);
            if (ret != SUCCESS) {
                // unable to insert into target relation
                closeRel(targetrel);
                ba_delete(targetrel);
                return ret;
            }
        } else
            break;

    }

    closeRel(targetrel);
    return SUCCESS;
}


int select(char srcrel[ATTR_SIZE], char targetrel[ATTR_SIZE], char attr[ATTR_SIZE], int op, char val_str[ATTR_SIZE]) {
    /* Check source relation is open */
    int srcrelid = OpenRelTable::getRelationId(srcrel);
    if (srcrelid == E_RELNOTOPEN) {
        // src relation not open
        return E_RELNOTOPEN;
    }

    /* Get the attribute catalog entry for the input attribute on which condition is applied*/
    Attribute attrcat_entry[NO_OF_ATTRS_RELCAT_ATTRCAT];
    int flag = getAttrCatEntry(srcrelid, attr, attrcat_entry);
    if (flag != SUCCESS)
        return flag;

    /* Convert value c-string to actual NUMBER or STRING attribute */
    int type = (int) attrcat_entry[2].nval;
    Attribute val;
    if (type == NUMBER) {
        try {
            val.nval = std::stof(val_str);
        } catch(std::invalid_argument& e) {
            return E_ATTRTYPEMISMATCH;
        }
    } else if (type == STRING) {
        strcpy(val.sval, val_str);
    }

    Attribute src_relcat_entry[NO_OF_ATTRS_RELCAT_ATTRCAT];
    getRelCatEntry(srcrelid, src_relcat_entry);

    int nAttrs = (int) src_relcat_entry[1].nval;
    char attr_names[nAttrs][ATTR_SIZE];
    int attr_types[nAttrs];

    /*
     * Find the names and types of all attributes of source relation and storing them in attr_names and attr_types
     * Linear search the Attribute Catalog, with :
     *      - RelName = source relation name
     *  Repeat the iteration nAttrs times to collect all attributes of source
     */
    recId prev_recid;
    prev_recid.block = -1;
    prev_recid.slot = -1;
    recId recid;
    for (int attr_no = 0; attr_no < nAttrs; attr_no++) {
        Attribute record[NO_OF_ATTRS_RELCAT_ATTRCAT];
        Attribute srcrelname_attr;
        strcpy(srcrelname_attr.sval, srcrel);
        /* Linear search the Attribute Catalog for attributes of source relation */
        recid = linear_search(ATTRCAT_RELID, "RelName", srcrelname_attr, EQ, &prev_recid);
        if (!((recid.block == -1) && (recid.slot == -1))) {
            getRecord(record, recid.block, recid.slot);
            strcpy(attr_names[attr_no], record[1].sval);
            attr_types[attr_no] = (int) record[2].nval;
        }
        if ((recid.block == -1) && (recid.slot == -1))
            return E_ATTRNOTEXIST;
    }

    /* Create the target relation */
    int retval = createRel(targetrel, nAttrs, attr_names, attr_types);
    if (retval != SUCCESS)
        return retval;

    /* Open the target relation */
    int target_relid = openRel(targetrel);
    if (target_relid == -1) {
        ba_delete(targetrel);
        return E_CACHEFULL;
    }

    // TODO: Already present here:
    //  Call ba_search of block access layer with op=RST for having {-1, -1}
    prev_recid.block = -1;
    prev_recid.slot = -1;
    while (true) {
        Attribute record[nAttrs];
        retval = ba_search(srcrelid, record, attr, val, op, &prev_recid);
        if (retval == SUCCESS) {
            int ret = ba_insert(target_relid, record);
            if (ret != SUCCESS) {
                closeRel(targetrel);
                ba_delete(targetrel);
                return ret;
            }
        } else
            break;
    }
    closeRel(targetrel);
    return SUCCESS;
}

int insert(std::vector<std::string> attributeTokens, char *table_name) {

    // check if relation is open
    int relId = OpenRelTable::getRelationId(table_name);
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
		std::string attrValue = attributeTokens[i];
		char tempAttribute[ATTR_SIZE];
		int j;
		for (j = 0; j < 15 && j < attrValue.size(); j++) {
			tempAttribute[j] = attrValue[j];
		}
		tempAttribute[j] = '\0';
		strcpy(recordArray[i], tempAttribute);
	}

	// Construct a record ( array of type Attribute ) from previous character array
	// Perform type checking for number types
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

int insert(char relName[ATTR_SIZE], char *fileName) {

	char currentCharacter;

	// check if relation is open
	int relId = OpenRelTable::getRelationId(relName);
	if (relId == E_RELNOTOPEN) {
		return relId;
	}

	// get #attributes from relation catalog entry
	int numOfAttributes = getNumberOfAttrsForRelation(relId);

	// get attribute types from attribute catalog entry
	int attrTypes[numOfAttributes];
	getAttrTypesForRelation(relId, numOfAttributes, attrTypes);


	char *currentLineAsCharArray = (char *) malloc(sizeof(char));
	int numOfCharactersInLine = 1;

	FILE *file = fopen(fileName, "r");
	int lineNumber = 1;
	while (true) {
		currentCharacter = fgetc(file);
		if (currentCharacter == EOF)
			break;
		while (currentCharacter == ' ' || currentCharacter == '\t' || currentCharacter == '\n') {
			currentCharacter = fgetc(file);
		}
		if (currentCharacter == EOF)
			break;

		numOfCharactersInLine = 1;
		int numOfFieldsInLine = 0;

		char previousCharacter = ',';
		while ((currentCharacter != '\n') && (currentCharacter != EOF)) {
			if (currentCharacter == ',')
				numOfFieldsInLine++;

			if (previousCharacter == currentCharacter && currentCharacter == ',') {
				std::cout << "Null values not allowed\n";
				return FAILURE;
			}

			currentLineAsCharArray[numOfCharactersInLine - 1] = currentCharacter;
			numOfCharactersInLine++;
			currentLineAsCharArray = (char *) realloc(currentLineAsCharArray, (numOfCharactersInLine) * sizeof(char));
			previousCharacter = currentCharacter;
			currentCharacter = fgetc(file);

		}

		if (previousCharacter == ',') {
			std::cout << "Null values not allowed in attribute values\n";
			return FAILURE;
		}

		if (numOfAttributes != numOfFieldsInLine + 1) {
			std::cout << "Mismatch in number of attributes\n";
			return FAILURE;
		}
		currentLineAsCharArray[numOfCharactersInLine - 1] = '\0';
		int currentCharIndexInLine = 0;

		char attributesCharArray[numOfAttributes][ATTR_SIZE];
		int attrOffsetIterator = 0;

		while (attrOffsetIterator < numOfAttributes) {
			int attributeIndexIterator = 0;

			while (((currentLineAsCharArray[currentCharIndexInLine] != ',') && (currentLineAsCharArray[currentCharIndexInLine] != '\0')) && (attributeIndexIterator < 15)) {
				attributesCharArray[attrOffsetIterator][attributeIndexIterator++] = currentLineAsCharArray[currentCharIndexInLine++];
			}
			if (attributeIndexIterator == 15) {
				while (currentLineAsCharArray[currentCharIndexInLine] != ',')
					currentCharIndexInLine++;
			}
			currentCharIndexInLine++;
			attributesCharArray[attrOffsetIterator][attributeIndexIterator] = '\0';
			attrOffsetIterator++;
		}

		Attribute record[numOfAttributes];
		int retValue = constructRecordFromAttrsArray(numOfAttributes, record, attributesCharArray, attrTypes);
		if (retValue == E_ATTRTYPEMISMATCH)
			return E_ATTRTYPEMISMATCH;
		else if (retValue == E_INVALID) {
			if(lineNumber > 1) {
				std::cout << "Rows till line " << lineNumber - 1 << " successfully inserted\n";
			}
			std::cout << "Invalid character at line " << lineNumber << " in file \n";
			std::cout << "Subsequent lines will be skipped\n";
			return FAILURE;
		}

		retValue = ba_insert(relId, record);
		if (retValue != SUCCESS) {
			return FAILURE;
		}

		if (currentCharacter == EOF)
			break;

		lineNumber++;
	}

	fclose(file);
	return SUCCESS;
}

int join(char srcrel1[ATTR_SIZE], char srcrel2[ATTR_SIZE], char targetRelation[ATTR_SIZE], char attr1[ATTR_SIZE],
         char attr2[ATTR_SIZE]) {

    // IF ANY SOURCE RELATION IS NOT OPEN, return E_RELNOTOPEN
    int srcRelId1 = OpenRelTable::getRelationId(srcrel1);
    if (srcRelId1 == E_RELNOTOPEN)
        return srcRelId1;
    int srcRelId2 = OpenRelTable::getRelationId(srcrel2);
    if (srcRelId2 == E_RELNOTOPEN)
        return srcRelId2;

    // GET RELATION CATALOG ENTRIES OF JOIN ATTRIBUTES OF SRC RELATIONS
    Attribute attrcat_entry1[NO_OF_ATTRS_RELCAT_ATTRCAT];
    int flag1 = getAttrCatEntry(srcRelId1, attr1, attrcat_entry1);
    Attribute attrcat_entry2[NO_OF_ATTRS_RELCAT_ATTRCAT];
    int flag2 = getAttrCatEntry(srcRelId2, attr2, attrcat_entry2);

    // if attr1 is not present in rel1 or attr2 not present in rel2 (failure of call to Openreltable) return E_ATTRNOTEXIST.
    if (flag1 != SUCCESS || flag2 != SUCCESS)
        return E_ATTRNOTEXIST;

    // if attr1 and attr2 are of different types return E_ATTRTYPEMISMATCH
    if (attrcat_entry1[2].nval != attrcat_entry2[2].nval)
        return E_ATTRTYPEMISMATCH;

    // GET RELATION CATALOG ENTRIES OF SRC RELATIONS
    union Attribute relcat_entry1[NO_OF_ATTRS_RELCAT_ATTRCAT];
    getRelCatEntry(srcRelId1, relcat_entry1);
    int nAttrs1 = static_cast<int>(relcat_entry1[1].nval);

    union Attribute relcat_entry2[NO_OF_ATTRS_RELCAT_ATTRCAT];
    getRelCatEntry(srcRelId2, relcat_entry2);
    int nAttrs2 = static_cast<int>(relcat_entry2[1].nval);

    /*
     * TODO :
     * Once B+ tree layer is implemented, ensure index exists for at least one attribute
     */

    /* TARGET RELATION -
     * targetRelAttrNames : array of attribute names
     * targetRelAttrTypes : array of attribute types
     */

    // GET THE NAMES AND TYPES OF ATTRIBUTES IN TARGET RELATION

    char targetRelAttrNames[nAttrs1 + nAttrs2 - 1][ATTR_SIZE];
    int targetRelAttrTypes[nAttrs1 + nAttrs2 - 1];
    std::unordered_set<std::string> targetRelAttributesSet;

    char srcRelation1[ATTR_SIZE];
    OpenRelTable::getRelationName(srcRelId1, srcRelation1);
    for (int iter = 0; iter < nAttrs1; iter++) {
        Attribute attrCatalogEntry[NO_OF_ATTRS_RELCAT_ATTRCAT];
        getAttrCatEntry(srcRelId1, iter, attrCatalogEntry);
        strncpy(targetRelAttrNames[iter], attrCatalogEntry[1].sval, ATTR_SIZE);
        targetRelAttributesSet.insert(attrCatalogEntry[1].sval);
        targetRelAttrTypes[iter] = attrCatalogEntry[2].nval;
    }
    int attrIndex = nAttrs1;

    char srcRelation2[ATTR_SIZE];
    OpenRelTable::getRelationName(srcRelId2, srcRelation2);
    for (int iter = 0; iter < nAttrs2; iter++) {
        Attribute attrCatalogEntry[NO_OF_ATTRS_RELCAT_ATTRCAT];
        getAttrCatEntry(srcRelId2, iter, attrCatalogEntry);

        if (strcmp(attr2, attrCatalogEntry[1].sval) != 0) {

	        // check if any 2 attributes in source relations have same name
	        if(targetRelAttributesSet.find(attrCatalogEntry[1].sval) != targetRelAttributesSet.end()) {
		        std::cout << "source relations have at least one attribute (other than join attributes) with same name\n";
		        return FAILURE;
	        }

            targetRelAttrTypes[attrIndex] = attrCatalogEntry[2].nval;
            strncpy(targetRelAttrNames[attrIndex++], attrCatalogEntry[1].sval, ATTR_SIZE);
        }
    }

    // CREATE TARGET RELATION AND OPEN IT OPEN REL TABLE
    int flag = createRel(targetRelation, nAttrs1 + nAttrs2 - 1, targetRelAttrNames, targetRelAttrTypes);
    if (flag != SUCCESS) {
        return flag;
    }
    int targetRelId = OpenRelTable::openRelation(targetRelation);
    if (targetRelId == E_CACHEFULL) {
        deleteRel(targetRelation);
        return E_CACHEFULL;
    }

    Attribute record1[nAttrs1];
    recId prev_recid1;
    prev_recid1.block = -1;
    prev_recid1.slot = -1;
    Attribute dummy;
    strcpy(dummy.sval, " ");

    // FORMING TARGET RELATION
    /*
     *
     */
    while (ba_search(srcRelId1, record1, "PROJECT", dummy, PRJCT, &prev_recid1) == SUCCESS) {

        Attribute record2[nAttrs2];
        Attribute targetRecord[nAttrs1 + nAttrs2 - 1];
        recId prev_recid2;
        prev_recid2.block = -1;
        prev_recid2.slot = -1;

        while (ba_search(srcRelId2, record2, attr2, record1[static_cast<int>(attrcat_entry1[5].nval)], EQ,
                         &prev_recid2) == SUCCESS) {

            for (int iter = 0; iter < nAttrs1; iter++)
                targetRecord[iter] = record1[iter];
            int targetIndex = nAttrs1;
            for (int iter = 0; iter < nAttrs2; iter++) {
                if (iter != static_cast<int>(attrcat_entry2[5].nval))
                    targetRecord[targetIndex++] = record2[iter];
            }

            flag = ba_insert(targetRelId, targetRecord);

            if (flag != SUCCESS) {
                OpenRelTable::closeRelation(targetRelId);
                deleteRel(targetRelation);
                return flag;
            }

        }
    }

    OpenRelTable::closeRelation(targetRelId);
    return SUCCESS;
}


// TODO : Find library functions for this?
int checkAttrTypeOfValue(char *data) {
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

/*
 * Gets #ttribute of relation from Relation Catalog Entry
 */
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

/*
 * Gets attribute types of relation from Attribute Catalog Entry
 */
void getAttrTypesForRelation(int relId, int numAttrs, int attrTypes[]) {

    Attribute attrCatEntry[NO_OF_ATTRS_RELCAT_ATTRCAT];

    for (int offsetIter = 0; offsetIter < numAttrs; ++offsetIter) {
        getAttrCatEntry(relId, offsetIter, attrCatEntry);
        attrTypes[static_cast<int>(attrCatEntry[5].nval)] = static_cast<int>(attrCatEntry[2].nval);
    }
}


/* Construct a record ( array of type Attribute ) from char array of attributes
 * Also performs type checking
 * @param numAttrs : #attributes in the relation
 * @param record : 'Attribute' array, new record is stored here
 * @param recordArray : 2D char array, a single row stores an attribute as a char array
 * @param attrTypes : attribute types of the relation, used for type checking
 * @return :
 *      SUCCESS
 *      E_ATTRTYPEMISMATCH : types dont match
 */
int constructRecordFromAttrsArray(int numAttrs, Attribute record[], char recordArray[][ATTR_SIZE], int attrTypes[]) {
	for (int attributeOffset = 0; attributeOffset < numAttrs; attributeOffset++) {

		if (attrTypes[attributeOffset] == NUMBER) {
			if (checkAttrTypeOfValue(recordArray[attributeOffset]) == NUMBER)
				record[attributeOffset].nval = atof(recordArray[attributeOffset]);
			else
				return E_ATTRTYPEMISMATCH;
		}

		if (attrTypes[attributeOffset] == STRING) {
			for (int charIndex = 0; charIndex < ATTR_SIZE; ++charIndex) {
				char ch = recordArray[attributeOffset][charIndex];
				if (ch == '\0')
					break;
				if (checkIfInvalidCharacter(ch)) {
//					std::cout << "attr : " << recordArray[attributeOffset];
//					std::cout << "  attr letter : " << ch;
					return E_INVALID;
				}
			}
			strcpy(record[attributeOffset].sval, recordArray[attributeOffset]);
		}
	}
	return SUCCESS;
}