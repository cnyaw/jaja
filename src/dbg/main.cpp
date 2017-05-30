//
// jaja assembler debugger.
//
// 2009/08/14 Waync Cheng.
//

#include <iostream>
#include <string>
#include <map>

#include "../op.h"
#include "../vm/ccpp/vm.h"
#include "../vm/ccpp/vm.hxx"

//
// Global variables.
//

extern VMImpl vm;
int g_addrDump;
int g_addrUnasm;
int g_stackDump = true;
std::map<int,int> g_breakpoints;

//
// Unassemble.
//

void unasm(int addr, int count);
int unasm1cmd(int addr);

//
// Toggle breakpoint.
//

void breakpoint(int addr)
{
  if (-1 == vm.im) {
    return;
  }

  //
  // List breakpoints.
  //

  if (-1 == addr) {

    if (g_breakpoints.empty()) {
      return;
    }

    printf("Index\tAddr\n");

    int i = 0;
    for (std::map<int,int>::const_iterator it = g_breakpoints.begin();
         g_breakpoints.end() != it;
         ++it) {
      printf("%d\t%08x\n", i++, it->first);
    }

    return;
  }

  //
  // Disable.
  //

  if (g_breakpoints.end() != g_breakpoints.find(addr)) {
    g_breakpoints.erase(addr);
  } else {

    //
    // Add.
    //

    if (0 <= addr && vm.heapSize > addr) {
      g_breakpoints[addr] = 0;
    }
  }
}

//
// Dump memory.
//

void dump(int addr, int count)
{
  if (0 <= addr && vm.heapSize > addr) {
    g_addrDump = addr;
  }

  if (g_addrDump + count > vm.heapSize) {
    count = vm.heapSize - g_addrDump;
  }

  int addrTag = g_addrDump, extra = 0;
  if (0 < g_addrDump && 0 != (g_addrDump % 16)) {
    addrTag = g_addrDump - (g_addrDump % 16);
    extra = g_addrDump - addrTag;
  }

  int total = count + extra;
  for (int i = 0; i < total;++i) {

    printf("%08X  ", addrTag);

    int j;
    for (j = 0; j < 16 && i + j < total;++j) {
      if (addrTag + j >= g_addrDump) {
        printf("%02X", vm.heap[addrTag + j]);
      } else {
        printf("  ");
      }
      printf(7 == j ? "-" : " ");
    }

    if (16 != j) {
      for (int k = j; k < 16;++k) {
        printf("   ");
      }
    }

    printf(" ");

    for (j = 0; j < 16 && i + j < total;++j) {
      if (addrTag + j >= g_addrDump) {
          unsigned char c = vm.heap[addrTag + j];
          if (' ' <= c && '~' >= c) {
            printf("%c", c);
          } else {
            printf(".");
          }
      } else {
        printf(" ");
      }
    }

    i += j - 1;
    addrTag += 16;

    printf("\n");
  }

  g_addrDump += count;
}

//
// Usage.
//

void usage()
{
  printf("Usage: dbg [option] ModName\n");
  printf("option:\n");
  printf("    -ppath, MOD search path\n");
  printf("example:\n");
  printf("    dbg -p../../../test/ sum\n");
}

//
// Print help message.
//

void help()
{
  printf("jaja assembler debugger (alpha), 2009 Waync Cheng.\n");
  usage();
  printf(":\n");
  printf("breakpoint\tB [address]\n");
  printf("dump\t\tD [range]\n");
  printf("fill\t\tF range list(range must assign a count)\n");
  printf("go\t\tG\n");
  printf("module\t\tM [index]\n");
  printf("process\t\tP\n");
  printf("register\tR\n");
  printf("trace\t\tT\n");
  printf("stack\t\tS\n");
  printf("quit\t\tQ\n");
  printf("unassemble\tU [range]\n");
  printf("    range = address [count]\n");
  printf("    list = HH HH ... or \"abc...\"\n");
  printf("    address: hex, count: dec\n");
  printf("    default range count: dump=128, unassemble=16\n");
}

//
// List loaded module info.
//

void module(int index)
{
  //
  // List all loaded modules.
  //

  if (-1 == index) {

    printf("Index\t%-20s\t%-8s\tCode Size\n", "Mod Name", "Heap Offset");
    for (int i = 0; i < (int)vm.mod.size();++i) {
      VMImpl::Mod const& mod = vm.mod[i];
      printf(
        "%d\t%-20s\t%08x\t%x(%d)\n",
        i,
        mod.modName.c_str(),
        mod.offset,
        mod.size,
        mod.size);
    }

  } else if (0 <= index && (int)vm.mod.size() > index) {

    //
    // List specified module info.
    //

    VMImpl::Mod const& mod = vm.mod[index];

    printf(
      "Mod Name=%s, Heap Offset=%x(%d), Code Size=%x(%d)\n",
      mod.modName.c_str(),
      mod.offset,
      mod.offset,
      mod.size,
      mod.size);

    if (mod.exports.size()) {
      printf("Exports\n");
      for (int i = 0; i < (int)mod.exports.size();++i) {
        VMImpl::Export const& exp = mod.exports[i];
        printf(
          "  %4x\t%c%-12s\t%8x\n",
          i,
          TAG_NATIVE == exp.offset ? '*' : ' ',
          mod.names[exp.iLabelName].c_str(),
          exp.offset);
      }
    }

    if (mod.imports.size()) {
      printf("Imports\n");
      for (int i = 0; i < (int)mod.imports.size();++i) {
        VMImpl::Import const& imp = mod.imports[i];
        printf(
          "  %4x\t %s.%s\n",
          i,
          mod.names[imp.iModName].c_str(),
          mod.names[imp.iLabelName].c_str());
      }
    }

  } else {

    //
    // Error index.
    //

    printf(" Error, index out of range\n");
  }
}

//
// Display VM stat.
//

void vmstat()
{
  printf(
    "Heap Capacity=%d(%x), Size=%d(%x), Mod=%s\n",
    vm.heapSize,
    vm.heapSize,
    vm.hp,
    vm.hp,
    -1 == vm.im ? "" : vm.mod[vm.im].modName.c_str());
  printf(
    "CS=%04x  IP=%04x  SP=%04x\n",
    vm.cs,
    vm.ip + (-1 == vm.im ? 0 : vm.mod[vm.im].offset),
    sizeof(int) * vm.sp);
  unasm1cmd(vm.cs + vm.ip);
  if (g_stackDump) {
    printf("[ ");
    for (int i = VMImpl::MAX_STACK / 4 - 1; i >= vm.sp; i--) {
      printf("%x ", vm.stack[i]);
    }
    printf("]\n");
  }
}

//
// Continue run.
//

bool go()
{
  while (jajaVM::getInstance().runStep()) {
    if (g_breakpoints.end() != g_breakpoints.find(vm.cs + vm.ip)) { // Breakpoint.
      return true;
    }
  }

  return false;
}

int main(int argc, char **argv)
{
  printf("jaja assembler debugger (alpha), 2009 Waync Cheng.\n");

  std::string path;                     // MOD search path.
  std::string mod;

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

  if (!jajaVM::getInstance().invoke(mod, path)) {
    printf("Load MOD %s failed!\n", mod.c_str());
    return 0;
  }

  printf("'?' for more help\n");

  g_addrDump = vm.cs;
  g_addrUnasm = vm.cs;

  std::string str;

  bool quit = false;
  while (!quit) {

    printf("-");                        // Prompt.

    while (!std::getline(std::cin, str)) {} // Read command line.

    str.erase(0, str.find_first_not_of(" \t\r\n")); // Trim space.
    str.erase(str.find_last_not_of(" \t\r\n") + 1);

    if (str.empty()) {
      continue;
    }

    //
    // Handle command.
    //

    switch (str[0])
    {
    case '?':                           // Help.
      help();
      break;

    case 'b': case 'B':                 // Breakpoint.
      {
        int addr = -1;
        sscanf(str.c_str() + 1, "%x", &addr);
        breakpoint(addr);
      }
      break;

    case 'd': case 'D':                 // Dump memory.
      {
        int addr = -1, count = 128;
        sscanf(str.c_str() + 1, "%x %d", &addr, &count);
        dump(addr, count);
      }
      break;

    case 'f': case 'F':                 // Fill memory.
      break;

    case 'g': case 'G':                 // Go.
      {
        bool r = go();
        vmstat();
        if (!r) {
          printf(" Done\n");
        }
      }
      break;

    case 'm': case 'M':                 // Module info.
      {
        int idx = -1;
        sscanf(str.c_str() + 1, "%d", &idx);
        module(idx);
      }
      break;

    case 'p': case 'P':                 // Process.
      {
        bool r = true;
        int op = vm.heap[vm.cs + vm.ip] & 0xff;
        if (OP_CALL == op || OP_CALLMOD == op) {
          int nextip = vm.cs + vm.ip + 3;
          bool bpExist = g_breakpoints.end() != g_breakpoints.find(nextip);
          if (!bpExist) {
            breakpoint(nextip);         // Tmp add a breakpoint.
          }
          r = go();
          if (!bpExist) {
            breakpoint(nextip);         // Remove tmp breakpoint.
          }
        } else {
          jajaVM::getInstance().runStep();
        }

        vmstat();

        if (!r) {
          printf(" Done\n");
        }
      }
      break;

    case 'r': case 'R':                 // Register.
      vmstat();
      break;

    case 't': case 'T':                 // Trace.
      jajaVM::getInstance().runStep();
      vmstat();
      break;

    case 's': case 'S':
      g_stackDump = !g_stackDump;
      break;

    case 'q': case 'Q':                 // Quit.
      quit = true;
      break;

    case 'u': case 'U':                 // Unassemble.
      {
        int addr = -1, count = 16;
        sscanf(str.c_str() + 1, "%x %d", &addr, &count);
        unasm(addr, count);
      }
      break;

    default:
      printf(" Error\n");
      break;

    }
  }

  return 0;
}

// end of main.cpp
