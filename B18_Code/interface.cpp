//
// Created by Gokul Sreekumar on 16/08/21.
//
// TODO : rename interface_new.cpp to interface.cpp
// TODO : review if we need to keep ';' at the end of commands
#include <iostream>
#include <fstream>
#include "define/constants.h"
#include "define/errors.h"
#include "interface.h"
#include "schema.h"
#include "Disk.h"
#include "OpenRelTable.h"
#include "block_access.h"
#include "external_fs_commands.h"

using namespace std;

void display_help();

void printErrorMsg(int ret);

vector<string> extract_tokens(string input_command);

void string_to_char_array(string x, char *a, int size);

int executeCommandsFromFile(string fileName);

/* TODO: RETURN 0 here means Success, return -1 (EXIT or FAILURE) means quit XFS,
 * I have done wherever i saw, check all that you added once again Jezzy
 */
int regexMatchAndExecute(const string input_command) {
	smatch m;
	if (regex_match(input_command, help)) {
		display_help();
	} else if (regex_match(input_command, ex)) {
		return EXIT;
	} else if (regex_match(input_command, run)) {
		smatch m;
		regex_search(input_command, m, run);
		string file_name = m[2];
		if (executeCommandsFromFile(file_name) == EXIT) {
			return EXIT;
		};
	} else if (regex_match(input_command, fdisk)) {
		Disk::createDisk();
		Disk::formatDisk();
		add_disk_metainfo();
		cout << "Disk formatted" << endl;
	} else if (regex_match(input_command, dump_rel)) {
		dump_relcat();
		cout << "Dumped relation catalog to $HOME/NITCBase/xfs-interface/relation_catalog" << endl;
	} else if (regex_match(input_command, dump_attr)) {
		dump_attrcat();
		cout << "Dumped attribute catalog to $HOME/NITCBase/xfs-interface/attribute_catalog" << endl;
	} else if (regex_match(input_command, dump_bmap)) {
		dumpBlockAllocationMap();
		cout << "Dumped block allocation map to $HOME/NITCBase/xfs-interface/block_allocation_map" << endl;
	} else if (regex_match(input_command, list_all)) {
		ls();
	} else if (regex_match(input_command, exp)) {
		regex_search(input_command, m, exp);
		string tableName = m[2];

		string filePath = m[3];
		filePath = "../Files/" + filePath;

		char relname[16];
		string_to_char_array(tableName, relname, 15);
		char fileName[filePath.length() + 1];
		string_to_char_array(filePath, fileName, filePath.length() + 1);

		int ret = exportRelation(relname, fileName);

		if (ret == SUCCESS)
			cout << "Exported successfully" << endl;
		else {
			cout << "Export Command Failed" << endl;
			return FAILURE;
		}

	} else if (regex_match(input_command, open_table)) {
		regex_search(input_command, m, open_table);
		string tablename = m[3];
		char relname[16];
		string_to_char_array(tablename, relname, 15);
		int ret = OpenRelations::openRelation(relname);
		if (ret >= 0 && ret <= 11) {
			cout << "Relation opened successfully\n";
		} else {
			printErrorMsg(ret);
			return FAILURE;
		}
	} else if (regex_match(input_command, close_table)) {
		regex_search(input_command, m, close_table);
		string tablename = m[3];
		char relname[16];
		string_to_char_array(tablename, relname, 15);
		int id = OpenRelations::getRelationId(relname);
		if (id == E_RELNOTOPEN) {
			cout << "Relation not open" << endl;
			return FAILURE;
		}
		if (id == 0 || id == 1) {
			cout << "Cannot close Relation/Attribute Catalog" << endl;
			return FAILURE;
		}
		int ret = OpenRelations::closeRelation(id);
		if (ret == SUCCESS) {
			cout << "Relation Closed Successfully\n";
		} else {
			printErrorMsg(ret);
			return FAILURE;
		}
	} else if (regex_match(input_command, create_table)) {
		regex_search(input_command, m, create_table);

		string tablename = m[3];
		char relname[ATTR_SIZE];
		string_to_char_array(tablename, relname, 15);

		regex_search(input_command, m, temp);
		string attrs = m[0];
		vector<string> words = extract_tokens(attrs);

		int no_attrs = words.size() / 2;
		char attribute[no_attrs][ATTR_SIZE];
		int type_attr[no_attrs];

		for (int i = 0, k = 0; i < no_attrs; i++, k += 2) {
			string_to_char_array(words[k], attribute[i], 15);
			if (words[k + 1] == "STR")
				type_attr[i] = STRING;
			else if (words[k + 1] == "NUM")
				type_attr[i] = NUMBER;
		}

		int ret = createRel(relname, no_attrs, attribute, type_attr);
		if (ret == SUCCESS) {
			cout << "Relation created successfully" << endl;
		} else {
			printErrorMsg(ret);
			return FAILURE;
		}

	} else if (regex_match(input_command, drop_table)) {
		regex_search(input_command, m, drop_table);
		string tablename = m[3];
		char relname[16];
		string_to_char_array(tablename, relname, 15);
		if (strcmp(relname, "RELATIONCAT") == 0 || strcmp(relname, "ATTRIBUTECAT") == 0)
			cout << "Cannot delete Relation Catalog or Attribute Catalog" << endl;
		int ret = deleteRel(relname);
		if (ret == SUCCESS)
			cout << "Relation deleted successfully" << endl;
		else {
			printErrorMsg(ret);
			return FAILURE;
		}

	} else if (regex_match(input_command, insert_single)) {

		regex_search(input_command, m, insert_single);
			string table_name = m[3];
			char rel_name[ATTR_SIZE];
			string_to_char_array(table_name, rel_name, 15);
			regex_search(input_command, m, temp);
			string attrs = m[0];
			vector<string> words = extract_tokens(attrs);

			//		int ret = insert_val(words, rel_name);

			//		if (ret == SUCCESS) {
			//			cout << "Inserted successfully" << endl;
			//		} else
			//			printErrorMsg(ret);

		}
	} else {
		cout << "Syntax Error" << endl;
		return FAILURE;
	}
	return SUCCESS;
}

int main() {
	// Initializing Open Relation Table
	OpenRelations::initializeOpenRelationTable();

	while (true) {
		cout << "# ";
		string input_command;
		smatch m;
		getline(cin, input_command);

		int ret = regexMatchAndExecute(input_command);
		if (ret == EXIT) {
			return 0;
		}
	}
}

// TODO: What to do when one line Fails - EXIT?
int executeCommandsFromFile(const string fileName) {
	const string filePath = "./Files/";
	fstream commandsFile;
	commandsFile.open(filePath + fileName, ios::in);
	string command;
	vector<string> commands;
	if (commandsFile.is_open()) {
		while (getline(commandsFile, command)) {
			commands.push_back(command);
		}
	} else {
		cout << "The file " << fileName << " does not exist\n";
	}
	int lineNumber = 1;
	for (auto command: commands) {
		int ret = regexMatchAndExecute(command);
		if (ret == EXIT) {
			return EXIT;
		} else if (ret == FAILURE) {
			cout << "At line number " << lineNumber << endl;
			break;
		}
		lineNumber++;
	}
	return SUCCESS;
}

void display_help() {
	printf("fdisk \n\t -Format disk \n\n");
	printf("import <filename> \n\t -loads relations from the UNIX filesystem to the XFS disk. \n\n");
	printf("export <tablename> <filename> \n\t -export a relation from XFS disk to UNIX file system. \n\n");
	printf("ls \n\t  -list the names of all relations in the xfs disk. \n\n");
	printf("dump bmap \n\t-dump the contents of the block allocation map.\n\n");
	printf("dump relcat \n\t-copy the contents of relation catalog to relationcatalog.txt\n \n");
	printf("dump attrcat \n\t-copy the contents of attribute catalog to an attributecatalog.txt. \n\n");
	printf("CREATE TABLE tablename(attr1_name attr1_type ,attr2_name attr2_type....); \n\t -create a relation with given attribute names\n \n");
	printf("DROP TABLE tablename ;\n\t-delete the relation\n  \n");
	printf("OPEN TABLE tablename ;\n\t-open the relation \n\n");
	printf("CLOSE TABLE tablename ;\n\t-close the relation \n \n");
	printf("CREATE INDEX ON tablename.attributename;\n\t-create an index on a given attribute. \n\n");
	printf(" DROP INDEX ON tablename.attributename ; \n\t-delete the index. \n\n");
	printf("ALTER TABLE RENAME tablename TO new_tablename ;\n\t-rename an existing relation to a given new name. \n\n");
	printf("ALTER TABLE RENAME tablename COLUMN column_name TO new_column_name ;\n\t-rename an attribute of an existing relation.\n\n");
	printf("INSERT INTO tablename VALUES ( value1,value2,value3,... );\n\t-insert a single record into the given relation. \n\n");
	printf("INSERT INTO tablename VALUES FROM filepath; \n\t-insert multiple records from a csv file \n\n");
	printf("SELECT * FROM source_relation INTO target_relation ; \n\t-creates a relation with the same attributes and records as of source relation\n\n");
	printf("SELECT Attribute1,Attribute2,....FROM source_relation INTO target_relation ; \n\t-creates a relation with attributes specified and all records\n\n");
	printf("SELECT * FROM source_relation INTO target_relation WHERE attrname OP value; \n\t-retrieve records based on a condition and insert them into a target relation\n\n");
	printf("SELECT Attribute1,Attribute2,....FROM source_relation INTO target_relation ;\n\t-creates a relation with the attributes specified and inserts those records which satisfy the given condition.\n\n");
	printf("SELECT * FROM source_relation1 JOIN source_relation2 INTO target_relation WHERE source_relation1.attribute1 = source_relation2.attribute2 ; \n\t-creates a new relation with by equi-join of both the source relations\n\n");
	printf("SELECT Attribute1,Attribute2,.. FROM source_relation1 JOIN source_relation2 INTO target_relation WHERE source_relation1.attribute1 = source_relation2.attribute2 ; \n\t-creates a new relation by equi-join of both the source relations with the attributes specified \n\n");
	printf("exit \n\t-Exit the interface\n");
	return;
}

void printErrorMsg(int ret) {
	if (ret == FAILURE)
		cout << "Error: Command Failed" << endl;
	else if (ret == E_OUTOFBOUND)
		cout << "Error: Out of bound" << endl;
	else if (ret == E_FREESLOT)
		cout << "Error: Free slot" << endl;
	else if (ret == E_NOINDEX)
		cout << "Error: No index" << endl;
	else if (ret == E_DISKFULL)
		cout << "Error: Insufficient space in Disk" << endl;
	else if (ret == E_INVALIDBLOCK)
		cout << "Error: Invalid block" << endl;
	else if (ret == E_RELNOTEXIST)
		cout << "Error: Relation does not exist" << endl;
	else if (ret == E_RELEXIST)
		cout << "Error: Relation already exists" << endl;
	else if (ret == E_ATTRNOTEXIST)
		cout << "Error: Attribute does not exist" << endl;
	else if (ret == E_ATTREXIST)
		cout << "Error: Attribute already exists" << endl;
	else if (ret == E_CACHEFULL)
		cout << "Error: Cache is full" << endl;
	else if (ret == E_RELNOTOPEN)
		cout << "Error: Relation is not open" << endl;
	else if (ret == E_NOTOPEN)
		cout << "Error: Relation is not open" << endl;
	else if (ret == E_NATTRMISMATCH)
		cout << "Error: Mismatch in number of attributes" << endl;
	else if (ret == E_DUPLICATEATTR)
		cout << "Error: Duplicate attributes found" << endl;
	else if (ret == E_RELOPEN)
		cout << "Error: Relation is open" << endl;
	else if (ret == E_ATTRTYPEMISMATCH)
		cout << "Error: Mismatch in attribute type" << endl;
	else if (ret == E_INVALID)
		cout << "Error: Invalid index or argument" << endl;
}

vector<string> extract_tokens(string input_command) {
	// tokenize with whitespace and brackets as delimiter
	vector<string> tokens;
	string token = "";
	for (int i = 0; i < input_command.length(); i++) {
		if (input_command[i] == '(' || input_command[i] == ')') {
			if (token != "") {
				tokens.push_back(token);
			}
			token = "";
		} else if (input_command[i] == ',') {
			if (token != "") {
				tokens.push_back(token);
			}
			token = "";
		} else if (input_command[i] == ' ' || input_command[i] == ';') {
			if (token != "") {
				tokens.push_back(token);
			}
			token = "";
		} else {
			token = token + input_command[i];
		}
	}
	if (token != "")
		tokens.push_back(token);

	return tokens;
}

void string_to_char_array(string x, char *a, int size) {
	// Reducing size of string to the size provided
	int i;
	if (size == 15) {
		for (i = 0; i < x.size() && i < 15; i++)
			a[i] = x[i];
		a[i] = '\0';
	} else {
		for (i = 0; i < size; i++) {
			a[i] = x[i];
		}
		a[i] = '\0';
	}
}