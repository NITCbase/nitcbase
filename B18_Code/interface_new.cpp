//
// Created by Gokul Sreekumar on 16/08/21.
//

#include <iostream>
#include "interface_new.h"
#include "Disk.h"
using namespace std;

void displayHelp();

int regexMatchAndExecute(const string input_command) {
	if(regex_match(input_command, help)) {
		displayHelp();
	} else if(regex_match(input_command, ex)) {
		return -1;
	} else if(regex_match(input_command,fdisk)) {
		Disk disk; // For Calling the constructor and making a new disk file
		Disk::formatDisk();

		// TODO: Do the meta() function here
		cout<<"Disk formatted"<<endl;
	}
	else {
		cout << "Syntax Error" << endl;
	}
	return 0;
}

int main() {
	while(true) {
		cout << "# ";
		string input_command;
		smatch m;
		getline(cin,input_command);

		int res = regexMatchAndExecute(input_command);
		if(res == -1) {
			return 0;
		}
	}
}

void displayHelp()
{
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