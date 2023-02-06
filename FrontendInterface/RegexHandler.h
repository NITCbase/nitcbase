#ifndef REGEX_HANDLER_H
#define REGEX_HANDLER_H

#include <regex>
#include <string>
#include <vector>

/* External File System Commands */
#define HELP_CMD "\\s*(HELP|help)\\s*;?"
#define EXIT_CMD "\\s*(EXIT|exit)\\s*;?"
#define RUN_CMD "\\s*(RUN|run)\\s+([a-zA-Z0-9_/.-]+)\\s*;?"
#define ECHO_CMD "\\s*(ECHO|echo)\\s+([a-zA-Z0-9 _,()'?:+*.-]+)\\s*;?"

/* DDL Commands*/
#define CREATE_TABLE_CMD "\\s*(?:CREATE|create)\\s+(?:TABLE|table)\\s+([A-Za-z0-9_-]+)\\s*\\(\\s*((?:[A-Za-z0-9_-]+\\s+(?:STR|NUM)\\s*,\\s*)*(?:[A-Za-z0-9_-]+\\s+(?:STR|NUM)))\\s*\\)\\s*;?"
#define DROP_TABLE_CMD "\\s*(DROP|drop)\\s+(TABLE|table)\\s+([A-Za-z0-9_-]+)\\s*;?"
#define OPEN_TABLE_CMD "\\s*(OPEN|open)\\s+(TABLE|table)\\s+([A-Za-z0-9_-]+)\\s*;?"
#define CLOSE_TABLE_CMD "\\s*(CLOSE|close)\\s+(TABLE|table)\\s+([A-Za-z0-9_-]+)\\s*;?"
#define CREATE_INDEX_CMD "\\s*(CREATE|create)\\s+(INDEX|index)\\s+(ON|on)\\s+([A-Za-z0-9_-]+)\\s*\\.\\s*([A-Za-z0-9_-]+)\\s*;?"
#define DROP_INDEX_CMD "\\s*(DROP|drop)\\s+(INDEX|index)\\s+(ON|on)\\s+([A-Za-z0-9_-]+)\\s*\\.\\s*([A-Za-z0-9_-]+)\\s*;?"
#define RENAME_TABLE_CMD "\\s*(ALTER|alter)\\s+(TABLE|table)\\s+(RENAME|rename)\\s+([a-zA-Z0-9_-]+)\\s+(TO|to)\\s+([a-zA-Z0-9_-]+)\\s*;?"
#define RENAME_COLUMN_CMD "\\s*(ALTER|alter)\\s+(TABLE|table)\\s+(RENAME|rename)\\s+([a-zA-Z0-9_-]+)\\s+(COLUMN|column)\\s+([a-zA-Z0-9_-]+)\\s+(TO|to)\\s+([a-zA-Z0-9_-]+)\\s*;?"

/* DML Commands */
#define SELECT_FROM_CMD "\\s*(select|SELECT)\\s+([*])\\s+(FROM|from)\\s+([A-Za-z0-9_-]+)\\s+(INTO|into)\\s+([A-Za-z0-9_-]+)\\s*;?"
#define SELECT_ATTR_FROM_CMD "\\s*(?:select|SELECT)\\s+((?:[A-Za-z0-9_-]+\\s*,\\s*)*(?:[A-Za-z0-9_-]+))\\s+(?:FROM|from)\\s+([A-Za-z0-9_-]+)\\s+(?:INTO|into)\\s+([A-Za-z0-9_-]+)\\s*;?"
#define SELECT_FROM_WHERE_CMD "\\s*(select|SELECT)\\s+([*])\\s+(FROM|from)\\s+([A-Za-z0-9_-]+)\\s+(INTO|into)\\s+([A-Za-z0-9_-]+)\\s+(WHERE|where)\\s+([A-Za-z0-9_-]+)\\s*(<|<=|>|>=|=|!=)\\s*([A-Za-z0-9_-]+|([0-9]+(\\.)[0-9]+))\\s*;?"
#define SELECT_ATTR_FROM_WHERE_CMD "\\s*(?:select|SELECT)\\s+((?:[A-Za-z0-9_-]+\\s*,\\s*)*(?:[A-Za-z0-9_-]+))\\s+(?:FROM|from)\\s+([A-Za-z0-9_-]+)\\s+(?:INTO|into)\\s+([A-Za-z0-9_-]+)\\s+(?:WHERE|where)\\s+([A-Za-z0-9_-]+)\\s*(<|<=|>|>=|=|!=)\\s*([A-Za-z0-9_-]+|([0-9]+(\\.)[0-9]+))\\s*;?"
#define SELECT_FROM_JOIN_CMD "\\s*(select|SELECT)\\s+([*])\\s+(FROM|from)\\s+([A-Za-z0-9_-]+)\\s+(JOIN|join)\\s+([A-Za-z0-9_-]+)\\s+(INTO|into)\\s+([A-Za-z0-9_-]+)\\s+(WHERE|where)\\s+([A-Za-z0-9_-]+)\\s*\\.([A-Za-z0-9_-]+)\\s*\\=\\s*([A-Za-z0-9_-]+)\\s*\\.([A-Za-z0-9_-]+)\\s*;?"
#define SELECT_ATTR_FROM_JOIN_CMD "\\s*(?:select|SELECT)\\s+((?:[A-Za-z0-9_-]+\\s*,\\s*)*(?:[A-Za-z0-9_-]+))\\s+(?:FROM|from)\\s+([A-Za-z0-9_-]+)\\s+(?:JOIN|join)\\s+([A-Za-z0-9_-]+)\\s+(?:INTO|into)\\s+([A-Za-z0-9_-]+)\\s+(?:WHERE|where)\\s+([A-Za-z0-9_-]+)\\s*\\.([A-Za-z0-9_-]+)\\s*\\=\\s*([A-Za-z0-9_-]+)\\s*\\.([A-Za-z0-9_-]+)\\s*;?"
#define INSERT_SINGLE_CMD "\\s*(?:INSERT|insert)\\s+(?:INTO|into)\\s+([A-Za-z0-9_-]+)\\s+(?:VALUES|values)\\s*\\(\\s*((?:(?:[A-Za-z0-9_-]+|[0-9]+\\.[0-9]+)\\s*,\\s*)*(?:[A-Za-z0-9_-]+|[0-9]+\\.[0-9]+))\\s*\\)\\s*;?"
#define INSERT_MULTIPLE_CMD "\\s*(INSERT|insert)\\s+(INTO|into)\\s+([A-Za-z0-9_-]+)\\s+(VALUES|values)\\s+(FROM|from)\\s+([a-zA-Z0-9_-]+\\.csv)\\s*;?"

class RegexHandler {
  typedef int (RegexHandler::*handlerFunction)(void);  // function pointer type

 private:
  // command to handler mappings
  const std::vector<std::pair<std::regex, handlerFunction>> handlers = {
      {std::regex(HELP_CMD), &RegexHandler::helpHandler},
      {std::regex(EXIT_CMD), &RegexHandler::exitHandler},
      {std::regex(ECHO_CMD), &RegexHandler::echoHandler},
      {std::regex(RUN_CMD), &RegexHandler::runHandler},
      {std::regex(OPEN_TABLE_CMD), &RegexHandler::openHandler},
      {std::regex(CLOSE_TABLE_CMD), &RegexHandler::closeHandler},
      {std::regex(CREATE_TABLE_CMD), &RegexHandler::createTableHandler},
      {std::regex(DROP_TABLE_CMD), &RegexHandler::dropTableHandler},
      {std::regex(CREATE_INDEX_CMD), &RegexHandler::createIndexHandler},
      {std::regex(DROP_INDEX_CMD), &RegexHandler::dropIndexHandler},
      {std::regex(RENAME_TABLE_CMD), &RegexHandler::renameTableHandler},
      {std::regex(RENAME_COLUMN_CMD), &RegexHandler::renameColumnHandler},
      {std::regex(INSERT_SINGLE_CMD), &RegexHandler::insertSingleHandler},
      {std::regex(INSERT_MULTIPLE_CMD), &RegexHandler::insertFromFileHandler},
      {std::regex(SELECT_FROM_CMD), &RegexHandler::selectFromHandler},
      {std::regex(SELECT_FROM_WHERE_CMD), &RegexHandler::selectFromWhereHandler},
      {std::regex(SELECT_ATTR_FROM_CMD), &RegexHandler::selectAttrFromHandler},
      {std::regex(SELECT_ATTR_FROM_WHERE_CMD), &RegexHandler::selectAttrFromWhereHandler},
      {std::regex(SELECT_FROM_JOIN_CMD), &RegexHandler::selectFromJoinHandler},
      {std::regex(SELECT_ATTR_FROM_JOIN_CMD), &RegexHandler::selectAttrFromJoinHandler},
  };

  // extract tokens delimited by whitespace and comma
  std::vector<std::string> extractTokens(std::string input);

  // handler functions
  std::smatch m;  // to store matches while parsing the regex
  int helpHandler();
  int exitHandler();
  int echoHandler();
  int runHandler();
  int openHandler();
  int closeHandler();
  int createTableHandler();
  int dropTableHandler();
  int createIndexHandler();
  int dropIndexHandler();
  int renameTableHandler();
  int renameColumnHandler();
  int insertSingleHandler();
  int insertFromFileHandler();
  int selectFromHandler();
  int selectFromWhereHandler();
  int selectAttrFromHandler();
  int selectAttrFromWhereHandler();
  int selectFromJoinHandler();
  int selectAttrFromJoinHandler();

 public:
  int handle(const std::string command);
};

#endif  // REGEX_HANDLER_H
