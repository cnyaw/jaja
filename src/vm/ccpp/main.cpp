//
// Simple VM implementation.
//
// 2009/08/14 Waync Cheng
//

#include <string.h>

#include <stdio.h>

#include "vm.h"

void usage()
{
  printf("Usage: jaja [option] ModName\n");
  printf("option:\n");
  printf("    -ppath, MOD search path\n");
}

int main(int argc, char **argv)
{
  printf("jaja virtual machine (alpha), 2009 Waync Cheng.\n");

  std::string path;                     // MOD search path.
  std::string mod;

  //
  // Process arguments.
  //

  if (2 == argc) {
    if (!strncmp("-p", argv[1], 2)) {
      usage();
      return 1;
    }
    path = ".";
    mod = argv[1];
  } else if (3 == argc) {
    if (!strncmp("-p", argv[1], 2)) {
      path = argv[1] + 2;
      mod = argv[2];
    } else if (!strncmp("-p", argv[2], 2)) {
      path = argv[2] + 2;
      mod = argv[1];
    } else {
      usage();
      return 1;
    }
  } else {
    usage();
    return 1;
  }

  if ('/' != path[path.length() -1]) {
    path.push_back('/');
  }

  //
  // Execute MOD.
  //

  jajaVM& vm = jajaVM::getInstance();
  if (vm.invoke(mod, path)) {
    while (vm.runStep()) {
    }
  }

  return 0;
}

// end of main.cpp
