//
// jaja, VM instance implement.
//
// 2009/08/14 Waync Cheng
//

#pragma once

#include <stdio.h>
#include <string.h>

#include <map>
#include <string>
#include <vector>

class VMImpl
{
public:

  struct Export
  {
    int iLabelName;
    int offset;
    void (*native)(void);
  };

  struct Import
  {
    int iModName;
    int iLabelName;
    int iMod;                                   // Cache.
    int offset;                                 // Cache.
    void (*native)(void);                       // Cache.
  };

  struct Mod
  {
    int iMod;                                   // Index in vm.mod.
    std::string modName;                        // MOD name.
    int offset;                                 // Offset in the heap.
    int size;                                   // Code size.
    std::vector<std::string> names;             // Name table.
    std::vector<Export> exports;                // Export table.
    std::vector<Import> imports;                // Import table.
  };

  enum CONST
  {
    MAX_STACK   = 512,                          // Max stack size(in byte).
    MAX_STACK2  = 512,                          // Max internal stack size, call/param/local(in byte).
    DEF_HEAP    = 2 * (MAX_STACK + MAX_STACK2), // Default heap size(in byte).
  };

  //
  // Context.
  //

  std::vector<Mod> mod;                         // MOD DB.
  std::map<std::string, void (*)(void)> native; // Native call lookup table.

  //
  // MOD search path.
  //

  std::string path;

  //
  // Heap space.
  //

  unsigned char* heap;                          // Heap space, contain total stack and code/data.
  int heapSize;                                 // Current capacity.
  int hp;                                       // Current available position offset.

  int im;                                       // Current active MOD.
  int cs;                                       // Code position of current active MOD.

  int ip;                                       // Instruction pointer.
  int pp;                                       // Param pointer.
  int lp;                                       // Local pointer.

  int* stack;                                   // Pointer to heap.
  int sp;                                       // Stack pointer.
  int xp;                                       // Internal stack pointer, call/param/local.
};

// end of vm.hxx
