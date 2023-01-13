// clang-format off
#include <cstring>
#include <fstream>
#include <string>
#include <iostream>
#include <readline/history.h>
#include <readline/readline.h>
// clang-format on

#include "../Disk_Class/Disk.h"
#include "../Frontend/Frontend.h"
#include "../define/constants.h"
#include "commands.h"

void stringToCharArray(string x, char *a, int size);

void displayHelp();

void printErrorMsg(int ret);

vector<string> extract_tokens(string input_command);

int executeCommandsFromFile(string fileName);

int getIndexOfFromToken(vector<string> command_tokens);

int getIndexOfWhereToken(vector<string> command_tokens);

string getAttrListStringFromCommand(string input_command, smatch m);

int getOperator(string op_str);

void print16(char char_string_thing[ATTR_SIZE], bool newline = true);

int regexMatchAndExecute(const string input_command) {
  smatch m;
  if (regex_match(input_command, help)) {
    displayHelp();
  } else if (regex_match(input_command, ex)) {
    return EXIT;
  } else if (regex_match(input_command, echo)) {
    regex_search(input_command, m, echo);
    string message = m[2];
    cout << message << endl;
  } else if (regex_match(input_command, run)) {
    regex_search(input_command, m, run);
    string file_name = m[2];
    if (executeCommandsFromFile(file_name) == EXIT) {
      return EXIT;
    }
  } else if (regex_match(input_command, open_table)) {
    regex_search(input_command, m, open_table);
    string tablename = m[3];
    char relname[ATTR_SIZE];
    stringToCharArray(tablename, relname, ATTR_SIZE - 1);

    int ret = Frontend::open_table(relname);
    if (ret >= 0) {
      cout << "Relation ";
      print16(relname, false);
      cout << " opened successfully\n";
    } else {
      printErrorMsg(ret);
      return FAILURE;
    }

  } else if (regex_match(input_command, close_table)) {
    regex_search(input_command, m, close_table);
    string tablename = m[3];
    char relname[ATTR_SIZE];
    stringToCharArray(tablename, relname, ATTR_SIZE - 1);

    int ret = Frontend::close_table(relname);
    if (ret == SUCCESS) {
      cout << "Relation ";
      print16(relname, false);
      cout << " closed successfully\n";
    } else {
      printErrorMsg(ret);
      return FAILURE;
    }
  } else if (regex_match(input_command, create_table)) {
    regex_search(input_command, m, create_table);
    string tablename = m[3];
    char relname[ATTR_SIZE];
    stringToCharArray(tablename, relname, ATTR_SIZE - 1);

    regex_search(input_command, m, temp);
    string attrs = m[0];
    vector<string> words = extract_tokens(attrs);

    int no_attrs = words.size() / 2;

    if (no_attrs > 125) {
      printErrorMsg(E_MAXATTRS);
      return FAILURE;
    }

    char attribute[no_attrs][ATTR_SIZE];
    int type_attr[no_attrs];

    for (int i = 0, k = 0; i < no_attrs; i++, k += 2) {
      stringToCharArray(words[k], attribute[i], ATTR_SIZE - 1);
      if (words[k + 1] == "STR")
        type_attr[i] = STRING;
      else if (words[k + 1] == "NUM")
        type_attr[i] = NUMBER;
    }

    int ret = Frontend::create_table(relname, no_attrs, attribute, type_attr);
    if (ret == SUCCESS) {
      cout << "Relation ";
      print16(relname, false);
      cout << " created successfully" << endl;
    } else {
      printErrorMsg(ret);
      return FAILURE;
    }

  } else if (regex_match(input_command, drop_table)) {
    regex_search(input_command, m, drop_table);
    string tablename = m[3];
    char relname[ATTR_SIZE];
    stringToCharArray(tablename, relname, ATTR_SIZE - 1);

    if (strcmp(relname, "RELATIONCAT") == 0 || strcmp(relname, "ATTRIBUTECAT") == 0) {
      cout << "Error: Cannot Delete Relation Catalog or Attribute Catalog" << endl;
      return FAILURE;
    }

    int ret = Frontend::drop_table(relname);
    if (ret == SUCCESS) {
      cout << "Relation ";
      print16(relname, false);
      cout << " deleted successfully" << endl;
    } else {
      printErrorMsg(ret);
      return FAILURE;
    }

  } else if (regex_match(input_command, create_index)) {
    regex_search(input_command, m, create_index);
    string tablename = m[4];
    string attrname = m[5];
    char relname[ATTR_SIZE], attr_name[ATTR_SIZE];

    stringToCharArray(tablename, relname, ATTR_SIZE - 1);
    stringToCharArray(attrname, attr_name, ATTR_SIZE - 1);

    int ret = Frontend::create_index(relname, attr_name);
    if (ret == SUCCESS) {
      cout << "Index created successfully\n";
    } else {
      printErrorMsg(ret);
      return FAILURE;
    }

  } else if (regex_match(input_command, drop_index)) {
    regex_search(input_command, m, drop_index);
    string tablename = m[4];
    string attrname = m[5];
    char relname[ATTR_SIZE], attr_name[ATTR_SIZE];
    stringToCharArray(tablename, relname, ATTR_SIZE - 1);
    stringToCharArray(attrname, attr_name, ATTR_SIZE - 1);

    int ret = Frontend::drop_index(relname, attr_name);
    if (ret == SUCCESS) {
      cout << "Index deleted successfully\n";
    } else {
      printErrorMsg(ret);
      return FAILURE;
    }

  } else if (regex_match(input_command, rename_table)) {
    regex_search(input_command, m, rename_table);
    string oldTableName = m[4];
    string newTableName = m[6];

    char old_relation_name[ATTR_SIZE];
    char new_relation_name[ATTR_SIZE];
    stringToCharArray(oldTableName, old_relation_name, ATTR_SIZE - 1);
    stringToCharArray(newTableName, new_relation_name, ATTR_SIZE - 1);

    int ret = Frontend::alter_table_rename(old_relation_name, new_relation_name);
    if (ret == SUCCESS) {
      cout << "Renamed Relation Successfully" << endl;
    } else {
      printErrorMsg(ret);
      return FAILURE;
    }

  } else if (regex_match(input_command, rename_column)) {
    regex_search(input_command, m, rename_column);
    string tablename = m[4];
    string oldcolumnname = m[6];
    string newcolumnname = m[8];
    char relname[ATTR_SIZE];
    char old_col[ATTR_SIZE];
    char new_col[ATTR_SIZE];
    stringToCharArray(tablename, relname, ATTR_SIZE - 1);
    stringToCharArray(oldcolumnname, old_col, ATTR_SIZE - 1);
    stringToCharArray(newcolumnname, new_col, ATTR_SIZE - 1);

    int ret = Frontend::alter_table_rename_column(relname, old_col, new_col);
    if (ret == SUCCESS) {
      cout << "Renamed Attribute Successfully" << endl;
    } else {
      printErrorMsg(ret);
      return FAILURE;
    }
  } else if (regex_match(input_command, insert_single)) {
    regex_search(input_command, m, insert_single);
    string table_name = m[3];
    char rel_name[ATTR_SIZE];
    stringToCharArray(table_name, rel_name, ATTR_SIZE - 1);
    regex_search(input_command, m, temp);
    string attrs = m[0];
    vector<string> words = extract_tokens(attrs);

    int attr_count = words.size();
    char attr_values_arr[words.size()][ATTR_SIZE];
    for (int i = 0; i < words.size(); ++i) {
      strcpy(attr_values_arr[i], words[i].c_str());
    }

    int ret = Frontend::insert_into_table_values(rel_name, words.size(), attr_values_arr);
    if (ret == SUCCESS) {
      cout << "Inserted successfully" << endl;
    } else {
      printErrorMsg(ret);
    }
    return ret;
  } else if (regex_match(input_command, insert_multiple)) {
    regex_search(input_command, m, insert_multiple);
    string tablename = m[3];
    char relname[ATTR_SIZE];
    stringToCharArray(tablename, relname, ATTR_SIZE - 1);

    string filepath = string(INPUT_FILES_PATH) + string(m[6]);
    std::cout << "File path: " << filepath << endl;

    ifstream file(filepath);
    if (!file.is_open()) {
      cout << "Invalid file path or file does not exist" << endl;
      return FAILURE;
    }

    string errorMsg("");
    string fileLine;

    int retVal = SUCCESS;
    int columnCount = -1, lineNumber = 1;
    while (getline(file, fileLine)) {
      vector<string> row;

      stringstream lineStream(fileLine);

      string item;
      while (getline(lineStream, item, ',')) {
        if (item.size() == 0) {
          errorMsg += "Null values not allowed in attribute values\n";
          retVal = FAILURE;
          break;
        }
        row.push_back(item);
      }
      if (retVal == FAILURE) {
        break;
      }

      if (columnCount == -1) {
        columnCount = row.size();
      } else if (columnCount != row.size()) {
        errorMsg += "Mismatch in number of attributes\n";
        retVal = FAILURE;
        break;
      }

      char rowArray[columnCount][ATTR_SIZE];
      for (int i = 0; i < columnCount; ++i) {
        stringToCharArray(row[i], rowArray[i], ATTR_SIZE - 1);
      }

      retVal = Frontend::insert_into_table_values(relname, columnCount, rowArray);

      if (retVal != SUCCESS) {
        break;
      }

      lineNumber++;
    }

    file.close();

    if (retVal == SUCCESS) {
      cout << lineNumber - 1 << " rows inserted successfully" << endl;
    } else {
      if (lineNumber > 1) {
        std::cout << "Rows till line " << lineNumber - 1 << " successfully inserted\n";
      }
      std::cout << "Insertion error at line " << lineNumber << " in file \n";
      std::cout << "Error:" << errorMsg;
      printErrorMsg(retVal);
      std::cout << "Subsequent lines will be skipped\n";
    }

    return retVal;

  } else if (regex_match(input_command, select_from)) {
    regex_search(input_command, m, select_from);
    string sourceRelName_str = m[4];
    string targetRelName_str = m[6];

    char sourceRelName[ATTR_SIZE];
    char targetRelName[ATTR_SIZE];

    stringToCharArray(sourceRelName_str, sourceRelName, ATTR_SIZE - 1);
    stringToCharArray(targetRelName_str, targetRelName, ATTR_SIZE - 1);

    int ret = Frontend::select_from_table(sourceRelName, targetRelName);
    if (ret == SUCCESS) {
      cout << "Selected successfully into ";
      print16(targetRelName);
    } else {
      printErrorMsg(ret);
      return FAILURE;
    }
  } else if (regex_match(input_command, select_from_where)) {
    regex_search(input_command, m, select_from_where);
    string sourceRel_str = m[4];
    string targetRel_str = m[6];
    string attribute_str = m[8];
    string op_str = m[9];
    string value_str = m[10];

    char sourceRelName[ATTR_SIZE];
    char targetRelName[ATTR_SIZE];
    char attribute[ATTR_SIZE];
    char value[ATTR_SIZE];
    stringToCharArray(sourceRel_str, sourceRelName, ATTR_SIZE - 1);
    stringToCharArray(targetRel_str, targetRelName, ATTR_SIZE - 1);
    stringToCharArray(attribute_str, attribute, ATTR_SIZE - 1);
    stringToCharArray(value_str, value, ATTR_SIZE - 1);

    int op = getOperator(op_str);

    int ret = Frontend::select_from_table_where(sourceRelName, targetRelName, attribute, op, value);
    if (ret == SUCCESS) {
      cout << "Selected successfully into ";
      print16(targetRelName);
    } else {
      printErrorMsg(ret);
      return FAILURE;
    }
  } else if (regex_match(input_command, select_attr_from)) {
    regex_search(input_command, m, select_attr_from);
    vector<string> command_tokens;
    for (auto token : m)
      command_tokens.push_back(token);
    int index_of_from = getIndexOfFromToken(command_tokens);

    string sourceRel_str = command_tokens[index_of_from + 1];
    string targetRel_str = command_tokens[index_of_from + 3];

    char sourceRelName[ATTR_SIZE];
    char targetRelName[ATTR_SIZE];
    stringToCharArray(sourceRel_str, sourceRelName, ATTR_SIZE - 1);
    stringToCharArray(targetRel_str, targetRelName, ATTR_SIZE - 1);

    /* Get the attribute list string from the input command */
    string attribute_list = getAttrListStringFromCommand(input_command, m);
    vector<string> attr_tokens = extract_tokens(attribute_list);

    int attr_count = attr_tokens.size();
    char attr_list[attr_count][ATTR_SIZE];
    for (int attr_no = 0; attr_no < attr_count; attr_no++) {
      stringToCharArray(attr_tokens[attr_no], attr_list[attr_no], ATTR_SIZE - 1);
    }

    int ret = Frontend::select_attrlist_from_table(sourceRelName, targetRelName, attr_count, attr_list);
    if (ret == SUCCESS) {
      cout << "Selected successfully into ";
      print16(targetRelName);
    } else {
      printErrorMsg(ret);
      return FAILURE;
    }
  } else if (regex_match(input_command, select_attr_from_where)) {
    regex_search(input_command, m, select_attr_from_where);
    vector<string> command_tokens;
    for (auto token : m)
      command_tokens.push_back(token);

    int index_of_from = getIndexOfFromToken(command_tokens);
    int index_of_where = getIndexOfWhereToken(command_tokens);

    string sourceRel_str = command_tokens[index_of_from + 1];
    string targetRel_str = command_tokens[index_of_from + 3];
    string attribute_str = command_tokens[index_of_where + 1];
    string op_str = command_tokens[index_of_where + 2];
    string value_str = command_tokens[index_of_where + 3];

    char sourceRelName[ATTR_SIZE];
    char targetRelName[ATTR_SIZE];
    char attribute[ATTR_SIZE];
    char value[ATTR_SIZE];
    int op = getOperator(op_str);

    stringToCharArray(attribute_str, attribute, ATTR_SIZE - 1);
    stringToCharArray(value_str, value, ATTR_SIZE - 1);
    stringToCharArray(sourceRel_str, sourceRelName, ATTR_SIZE - 1);
    stringToCharArray(targetRel_str, targetRelName, ATTR_SIZE - 1);

    string attribute_list = getAttrListStringFromCommand(input_command, m);
    vector<string> attr_tokens = extract_tokens(attribute_list);

    int attr_count = attr_tokens.size();
    char attr_list[attr_count][ATTR_SIZE];
    for (int attr_no = 0; attr_no < attr_count; attr_no++) {
      stringToCharArray(attr_tokens[attr_no], attr_list[attr_no], ATTR_SIZE - 1);
    }

    int ret = Frontend::select_attrlist_from_table_where(sourceRelName, targetRelName, attr_count, attr_list,
                                                         attribute, op, value);
    if (ret == SUCCESS) {
      cout << "Selected successfully into ";
      print16(targetRelName);
    } else {
      printErrorMsg(ret);
      return FAILURE;
    }
  } else if (regex_match(input_command, select_from_join)) {
    regex_search(input_command, m, select_from_join);

    // m[4] and m[10] should be equal ( = sourceRelOneName)
    // m[6] and m[102 should be equal ( = sourceRelTwoName)
    if (m[4] != m[10] || m[6] != m[12]) {
      cout << "Syntax Error" << endl;
      return FAILURE;
    }
    char sourceRelOneName[ATTR_SIZE];
    char sourceRelTwoName[ATTR_SIZE];
    char targetRelName[ATTR_SIZE];
    char joinAttributeOne[ATTR_SIZE];
    char joinAttributeTwo[ATTR_SIZE];

    stringToCharArray(m[4], sourceRelOneName, ATTR_SIZE - 1);
    stringToCharArray(m[6], sourceRelTwoName, ATTR_SIZE - 1);
    stringToCharArray(m[8], targetRelName, ATTR_SIZE - 1);
    stringToCharArray(m[11], joinAttributeOne, ATTR_SIZE - 1);
    stringToCharArray(m[13], joinAttributeTwo, ATTR_SIZE - 1);

    int ret = Frontend::select_from_join_where(sourceRelOneName, sourceRelTwoName, targetRelName,
                                               joinAttributeOne, joinAttributeTwo);
    if (ret == SUCCESS) {
      cout << "Selected successfully into ";
      print16(targetRelName);
    } else {
      printErrorMsg(ret);
      return FAILURE;
    }
  } else if (regex_match(input_command, select_attr_from_join)) {
    regex_search(input_command, m, select_attr_from_join);

    vector<string> tokens;
    for (auto token : m)
      tokens.push_back(token);

    int refIndex;
    for (refIndex = 0; refIndex < tokens.size(); refIndex++) {
      if (tokens[refIndex] == "from" || tokens[refIndex] == "FROM")
        break;
    }

    char sourceRelOneName[ATTR_SIZE];
    char sourceRelTwoName[ATTR_SIZE];
    char targetRelName[ATTR_SIZE];
    char joinAttributeOne[ATTR_SIZE];
    char joinAttributeTwo[ATTR_SIZE];

    stringToCharArray(tokens[refIndex + 1], sourceRelOneName, ATTR_SIZE - 1);
    stringToCharArray(tokens[refIndex + 3], sourceRelTwoName, ATTR_SIZE - 1);
    stringToCharArray(tokens[refIndex + 5], targetRelName, ATTR_SIZE - 1);
    stringToCharArray(tokens[refIndex + 8], joinAttributeOne, ATTR_SIZE - 1);
    stringToCharArray(tokens[refIndex + 10], joinAttributeTwo, ATTR_SIZE - 1);

    int attrListPos = 1;
    string attributesListAsStrings;
    string inputCommand = input_command;
    while (regex_search(inputCommand, m, attrlist)) {
      if (attrListPos == 2)
        attributesListAsStrings = m.str(0);
      attrListPos++;
      // suffix to find the rest of the string.
      inputCommand = m.suffix().str();
    }

    vector<string> attributesListAsWords = extract_tokens(attributesListAsStrings);
    int attrCount = attributesListAsWords.size();
    char attributeList[attrCount][ATTR_SIZE];
    for (int i = 0; i < attrCount; i++) {
      stringToCharArray(attributesListAsWords[i], attributeList[i], ATTR_SIZE - 1);
    }

    int ret = Frontend::select_attrlist_from_join_where(sourceRelOneName, sourceRelTwoName, targetRelName,
                                                        joinAttributeOne, joinAttributeTwo, attrCount,
                                                        attributeList);
    if (ret == SUCCESS) {
      cout << "Selected successfully into ";
      print16(targetRelName);
    } else {
      printErrorMsg(ret);
      return FAILURE;
    }
  } else {
    cout << "Syntax Error" << endl;
    return FAILURE;
  }
  return SUCCESS;
}

int handleFrontend(int argc, char *argv[]) {
  // Taking Run Command as Command Line Argument(if provided)
  if (argc == 3 && strcmp(argv[1], "run") == 0) {
    string run_command("run ");
    run_command.append(argv[2]);
    int ret = regexMatchAndExecute(run_command);
    if (ret == EXIT) {
      return 0;
    }
  }
  char *buf;
  rl_bind_key('\t', rl_insert);
  while ((buf = readline("# ")) != nullptr) {
    if (strlen(buf) > 0) {
      add_history(buf);
    }
    int ret = regexMatchAndExecute(string(buf));
    free(buf);
    if (ret == EXIT) {
      return 0;
    }
  }
  return 0;
}

int executeCommandsFromFile(const string fileName) {
  const string filePath = BATCH_FILES_PATH;
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
  for (auto command : commands) {
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

int getIndexOfFromToken(vector<string> command_tokens) {
  int index_of_from;
  for (index_of_from = 0; index_of_from < command_tokens.size(); index_of_from++) {
    if (command_tokens[index_of_from] == "from" || command_tokens[index_of_from] == "FROM")
      break;
  }
  return index_of_from;
}

int getIndexOfWhereToken(vector<string> command_tokens) {
  int index_of_where;
  for (index_of_where = 0; index_of_where < command_tokens.size(); index_of_where++) {
    if (command_tokens[index_of_where] == "where" || command_tokens[index_of_where] == "WHERE")
      break;
  }
  return index_of_where;
}

string getAttrListStringFromCommand(const string input_command, smatch m) {
  int attrListPos = 1;
  string attribute_list;
  string inputCommand = input_command;
  /*
   * At second position of the input attribute list will be obtained
   *      SELECT AttrList FROM  ...
   *      1      2        3     ...
   */
  while (regex_search(inputCommand, m, attrlist)) {
    if (attrListPos == 2)
      attribute_list = m.str(0);
    attrListPos++;
    // suffix to find the rest of the string.
    inputCommand = m.suffix().str();
  }

  return attribute_list;
}

int getOperator(string op_str) {
  int op = 0;
  if (op_str == "=")
    op = EQ;
  else if (op_str == "<")
    op = LT;
  else if (op_str == "<=")
    op = LE;
  else if (op_str == ">")
    op = GT;
  else if (op_str == ">=")
    op = GE;
  else if (op_str == "!=")
    op = NE;
  return op;
}

void stringToCharArray(string x, char *a, int size) {
  // Reducing size of string to the size provided
  int i;
  if (size == ATTR_SIZE - 1) {
    for (i = 0; i < x.size() && i < ATTR_SIZE - 1; i++)
      a[i] = x[i];
    a[i] = '\0';
  } else {
    for (i = 0; i < size; i++) {
      a[i] = x[i];
    }
    a[i] = '\0';
  }
}

void displayHelp() {
  printf("CREATE TABLE tablename(attr1_name attr1_type ,attr2_name attr2_type....); \n\t -create a relation with given attribute names\n \n");
  printf("DROP TABLE tablename;\n\t-delete the relation\n  \n");
  printf("OPEN TABLE tablename;\n\t-open the relation \n\n");
  printf("CLOSE TABLE tablename;\n\t-close the relation \n \n");
  printf("CREATE INDEX ON tablename.attributename;\n\t-create an index on a given attribute. \n\n");
  printf("DROP INDEX ON tablename.attributename; \n\t-delete the index. \n\n");
  printf("ALTER TABLE RENAME tablename TO new_tablename;\n\t-rename an existing relation to a given new name. \n\n");
  printf("ALTER TABLE RENAME tablename COLUMN column_name TO new_column_name;\n\t-rename an attribute of an existing relation.\n\n");
  printf("INSERT INTO tablename VALUES ( value1,value2,value3,... );\n\t-insert a single record into the given relation. \n\n");
  printf("INSERT INTO tablename VALUES FROM filepath; \n\t-insert multiple records from a csv file \n\n");
  printf("SELECT * FROM source_relation INTO target_relation; \n\t-creates a relation with the same attributes and records as of source relation\n\n");
  printf("SELECT Attribute1,Attribute2,....FROM source_relation INTO target_relation; \n\t-creates a relation with attributes specified and all records\n\n");
  printf("SELECT * FROM source_relation INTO target_relation WHERE attrname OP value; \n\t-retrieve records based on a condition and insert them into a target relation\n\n");
  printf("SELECT Attribute1,Attribute2,....FROM source_relation INTO target_relation;\n\t-creates a relation with the attributes specified and inserts those records which satisfy the given condition.\n\n");
  printf("SELECT * FROM source_relation1 JOIN source_relation2 INTO target_relation WHERE source_relation1.attribute1 = source_relation2.attribute2; \n\t-creates a new relation with by equi-join of both the source relations\n\n");
  printf("SELECT Attribute1,Attribute2,.. FROM source_relation1 JOIN source_relation2 INTO target_relation WHERE source_relation1.attribute1 = source_relation2.attribute2; \n\t-creates a new relation by equi-join of both the source relations with the attributes specified \n\n");
  printf("echo <any message> \n\t  -echo back the given string. \n\n");
  printf("run <filename> \n\t  -run commands from an input file in sequence. \n\n");
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
  else if (ret == E_RELNOTOPEN)
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
  else if (ret == E_MAXRELATIONS)
    cout << "Error: Maximum number of relations already present" << endl;
  else if (ret == E_MAXATTRS)
    cout << "Error: Maximum number of attributes allowed for a relation is 125" << endl;
  else if (ret == E_NOTPERMITTED)
    cout << "Error: This operation is not permitted" << endl;
  else if (ret == E_INDEX_BLOCKS_RELEASED)
    cout << "Warning: Operation succeeded, but some indexes had to be dropped." << endl;
}

vector<string> extract_tokens(string input_command) {
  // tokenize with whitespace and brackets as delimiter
  vector<string> tokens;
  string token;
  for (int i = 0; i < input_command.length(); i++) {
    if (input_command[i] == '(' || input_command[i] == ')') {
      if (!token.empty()) {
        tokens.push_back(token);
      }
      token = "";
    } else if (input_command[i] == ',') {
      if (!token.empty()) {
        tokens.push_back(token);
      }
      token = "";
    } else if (input_command[i] == ' ' || input_command[i] == ';') {
      if (!token.empty()) {
        tokens.push_back(token);
      }
      token = "";
    } else {
      token += input_command[i];
    }
  }
  if (!token.empty())
    tokens.push_back(token);

  return tokens;
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