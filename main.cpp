#include <iostream>

#include "Buffer/StaticBuffer.h"
#include "Cache/OpenRelTable.h"
#include "Disk_Class/Disk.h"
#include "Frontend_Interface/frontend-runner.h"

int main(int argc, char *argv[]) {
  /* Initialize the Run Copy of Disk */
  Disk disk_run;
  std::cout << "Run Copy of Disk Initialized\n";
  // StaticBuffer buffer;
  // OpenRelTable cache;

  return handleFrontend(argc, argv);
}