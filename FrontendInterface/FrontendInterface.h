#ifndef FRONTEND_INTERFACE_H
#define FRONTEND_INTERFACE_H

#include "RegexHandler.h"

class FrontendInterface {
 private:
  RegexHandler regexHandler;

 public:
  int handleFrontend(int argc, char *argv[]);
};

#endif
