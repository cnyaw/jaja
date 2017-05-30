//
// jaja, native api implement.
//
// 2009/08/14 Waync Cheng
//

#include "vm.hxx"
#include "ld.h"

#include "system.c"

extern VMImpl vm;

void NATIVEsystem_outchr()
{
  printf("%c", (char)(*(const char*)(vm.stack + vm.sp)));
  vm.sp++;
}

void NATIVEsystem_outint()
{
  printf("%d", vm.stack[vm.sp]);
  vm.sp++;
}

void NATIVEsystem_outstr()
{
  printf("%s", (const char*)(vm.heap + vm.stack[vm.sp]));
  vm.sp++;
}

void NATIVEsystem_readint()
{
  int i;
  scanf("%d", &i);
  vm.stack[--vm.sp] = i;
}

//
void initNativeTable()
{
  vm.native.clear();

  //
  // stdio MOD.
  //

  vm.native["system.outchr"] = &NATIVEsystem_outchr;
  vm.native["system.outint"] = &NATIVEsystem_outint;
  vm.native["system.outstr"] = &NATIVEsystem_outstr;
  vm.native["system.readint"] = &NATIVEsystem_readint;

  //
  // Preload system MOD.
  //

  if (-1 == loadMod("system", sizeof(SYSTEM_MOD), SYSTEM_MOD)) {
    printf("Preload system.m failed.\n");
    return;
  }
}

// end of native.cpp
