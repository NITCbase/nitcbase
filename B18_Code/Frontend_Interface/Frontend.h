//
// Created by Gokul on 25/12/21.
//

#ifndef FRONTEND_INTERFACE_FRONTEND_H
#define FRONTEND_INTERFACE_FRONTEND_H

#include <vector>
#include "../define/constants.h"
#include "../define/errors.h"

using namespace std;

// TODO: Complete DML Commands, DECIDE: Should the functions be static or not?
class Frontend {
public:
    // DDL
    static int
    create_table(char relname[ATTR_SIZE], int no_attrs, char attribute[no_attrs][ATTR_SIZE], int type_attr[no_attrs]);

    static int drop_table(char relname[ATTR_SIZE]);

    static int open_table(char relname[ATTR_SIZE]);

    static int close_table(char relname[ATTR_SIZE]);

    static int create_index(char relname[ATTR_SIZE], char attrname[ATTR_SIZE]);

    static int drop_index(char relname[ATTR_SIZE], char attrname[ATTR_SIZE]);

    static int alter_table_rename(char relname_from[ATTR_SIZE], char relname_to[ATTR_SIZE]);

    static int alter_table_rename_column(char relname[ATTR_SIZE], char attrname_from[16], char attrname_to[16]);

    // DML
    static int insert_into_table_from_file(char relname[ATTR_SIZE], char filepath[ATTR_SIZE]);

    static int insert_into_table_values(char relname[ATTR_SIZE], vector<string> attr_values);

    static int select_from_table(char relname_source[ATTR_SIZE], char relname_target[ATTR_SIZE]);

    static int select_attrlist_from_table(char relname_source[ATTR_SIZE], char relname_target[ATTR_SIZE],
                                          int attr_count, char attr_list[attr_count][ATTR_SIZE]);

    static int select_from_table_where(char relname_source[ATTR_SIZE], char relname_target[ATTR_SIZE],
                                       char attribute[ATTR_SIZE], int op, char value[ATTR_SIZE]);

    static int select_attrlist_from_table_where(char relname_source[ATTR_SIZE], char relname_target[ATTR_SIZE],
                                                int attr_count, char attr_list[attr_count][ATTR_SIZE],
                                                char attribute[ATTR_SIZE], int op, char value[ATTR_SIZE]);

    static int select_from_join_where(char relname_source_one[ATTR_SIZE], char relname_source_two[ATTR_SIZE],
                                      char relname_target[ATTR_SIZE],
                                      char join_attr_one[ATTR_SIZE], char join_attr_two[ATTR_SIZE]);

    static int select_attrlist_from_join_where(char relname_source_one[ATTR_SIZE], char relname_source_two[ATTR_SIZE],
                                               char relname_target[ATTR_SIZE],
                                               char join_attr_one[ATTR_SIZE], char join_attr_two[ATTR_SIZE],
                                               int attr_count, char attr_list[attr_count][ATTR_SIZE]);
};


#endif //FRONTEND_INTERFACE_FRONTEND_H
