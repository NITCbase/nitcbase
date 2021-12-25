//
// Created by Gokul on 25/12/21.
//

#include <iostream>
#include "Frontend.h"

void print16(char char_string_thing[ATTR_SIZE], bool newline);
void print16(char char_string_thing[ATTR_SIZE]);

// TODO: Complete the DML Commands, Should we make the functions static?
int Frontend::create_table(char relname[ATTR_SIZE], int no_attrs, char attribute[no_attrs][ATTR_SIZE], int type_attr[no_attrs]) {
    cout << "In Create Table\n";
    print16(relname);
    for (auto i = 0; i < no_attrs; i++) {
        print16(attribute[i], false);
        cout << " type = ";
        cout << type_attr[i] << endl;
    }
    return SUCCESS;
}

int Frontend::drop_table(char relname[ATTR_SIZE]) {
    cout << "In Drop Table\n";
    print16(relname);
    return SUCCESS;
}

int Frontend::open_table(char relname[ATTR_SIZE]) {
    cout << "In Open Table\n";
    print16(relname);
    return SUCCESS;
}

int Frontend::close_table(char relname[ATTR_SIZE]) {
    cout << "In Close Table\n";
    print16(relname);
    return SUCCESS;
}

int Frontend::alter_table_rename(char relname_from[ATTR_SIZE], char relname_to[ATTR_SIZE]) {
    cout << "In Alter Table Rename\n";
    print16(relname_from);
    print16(relname_to);
    return SUCCESS;
}

int Frontend::alter_table_rename_column(char relname[ATTR_SIZE], char attrname_from[ATTR_SIZE],
                                        char attrname_to[ATTR_SIZE]) {
    cout << "In Alter Table Rename Column\n";
    print16(relname);
    print16(attrname_from);
    print16(attrname_to);
    return SUCCESS;
}


int Frontend::create_index(char relname[ATTR_SIZE], char attrname[ATTR_SIZE]) {
    cout << "In Create Index\n";
    print16(relname);
    print16(attrname);
    return SUCCESS;
}

int Frontend::drop_index(char relname[ATTR_SIZE], char attrname[ATTR_SIZE]) {
    cout << "In Drop Index\n";
    print16(relname);
    print16(attrname);
    return SUCCESS;
}

int Frontend::insert_into_table_values(char relname[ATTR_SIZE], vector<string> attr_values) {
    cout << "In Insert into table values\n";
    print16(relname);
    for (const auto attr_value: attr_values) {
        cout << attr_value << " ";
    }
    return SUCCESS;
}

int Frontend::insert_into_table_from_file(char relname[ATTR_SIZE], char filepath[ATTR_SIZE]) {
    cout << "In Insert into table from file\n";
    print16(relname);
    print16(filepath);
    return SUCCESS;
}

int Frontend::select_from_table() {
    return SUCCESS;
}

int Frontend::select_attrlist_from_table() {
    return SUCCESS;
}

int Frontend::select_from_table_where() {
    return SUCCESS;
}

int Frontend::select_attrlist_from_table_where() {
    return SUCCESS;
}

int Frontend::select_from_join_where() {
    return SUCCESS;
}

int Frontend::select_attrlist_from_join_where() {
    return SUCCESS;
}


void print16(char char_string_thing[ATTR_SIZE]) {
    for (int i = 0; i < ATTR_SIZE; i++) {
        if (char_string_thing[i] == '\0') {
            break;
        }
        cout << char_string_thing[i];
    }
    cout << endl;
}

void print16(char char_string_thing[ATTR_SIZE], bool newline) {
    for (int i = 0; i < ATTR_SIZE; i++) {
        if (char_string_thing[i] == '\0') {
            break;
        }
        cout << char_string_thing[i];
    }
    if (newline) {
        cout << endl;
    }
    return;
}