//
// jaja, VM implement.
//
// 2009/08/14 Waync Cheng.
//

#include "vm.h"
#include "vm.hxx"
#include "ld.h"

#include "../../op.h"

extern void initNativeTable();

//
// Implementation.
//

VMImpl vm;

bool checkModLoaded(int iImport)
{
  //
  // Check is this MOD loaded.
  //

  if (-1 != vm.mod[vm.im].imports[iImport].iMod) {
    return true;
  }

  //
  // Try to load MOD.
  //

  int iMod = loadMod(vm.mod[vm.im].names[vm.mod[vm.im].imports[iImport].iModName]);
  if (-1 == iMod) {
    return false;
  }

  VMImpl::Mod& currMod = vm.mod[vm.im]; // The addr of currMod may change after loadMod called.
  VMImpl::Import& import = currMod.imports[iImport];
  std::string& name = currMod.names[import.iLabelName];

  VMImpl::Mod& mod = vm.mod[iMod];
  for (int i = 0; i < (int)mod.exports.size(); ++i) {
    if (name == mod.names[mod.exports[i].iLabelName]) {
      import.iMod = iMod;
      import.offset = mod.exports[i].offset;
      import.native = mod.exports[i].native;
      break;
    }
  }

  return true;
}

int readInt16()
{
  int a = vm.heap[vm.cs + vm.ip++];
  int b = vm.heap[vm.cs + vm.ip++] << 8;
  return a | b;
}

//
// VM implementation.
//

jajaVM& jajaVM::getInstance()
{
  static jajaVM instance;
  return instance;
}

jajaVM::jajaVM()
{
  vm.heap = new unsigned char[VMImpl::DEF_HEAP];
  vm.heapSize = VMImpl::DEF_HEAP;
  vm.hp = VMImpl::MAX_STACK + VMImpl::MAX_STACK2;
  vm.stack = (int*)vm.heap;
  vm.pp = vm.lp = vm.sp = VMImpl::MAX_STACK / 4;
  vm.xp = VMImpl::MAX_STACK / 4 + VMImpl::MAX_STACK2 / 4;
  vm.im = -1;
  vm.cs = -1;

  initNativeTable();
}

jajaVM::~jajaVM()
{
  delete [] vm.heap, vm.heap = 0;
  vm.stack = 0;
}

bool jajaVM::invoke(const std::string& modName, std::string const& path)
{
  vm.path = path;

  int iMod = loadMod(modName);
  if (-1 == iMod) {
    return false;
  }

  vm.stack[--vm.xp] = vm.im;
  vm.stack[--vm.xp] = vm.cs;

  vm.im = iMod;
  vm.cs = vm.mod[iMod].offset;

  vm.stack[--vm.xp] = vm.ip;
  vm.stack[--vm.xp] = -1;              // Dummy MOD call mark.
  vm.stack[--vm.xp] = vm.pp;

  return true;
}

//
// TODO: stack checking.
//

bool jajaVM::runStep()
{
  //
  // No active MOD.
  //

  if (-1 == vm.im) {
    return false;
  }

  //
  // Process byte code.
  //

  int op = vm.heap[vm.cs + vm.ip++] & 0xff;
  switch (op)
  {

  //
  // Do nothing.
  //

  case OP_NOP:
    break;

  //
  // Stack OP.
  //

  case OP_PUSH1:
    vm.stack[--vm.sp] = *(char*)(vm.heap + vm.cs + vm.ip);
    vm.ip += 1;
    break;

  case OP_PUSH2:
    vm.stack[--vm.sp] = *(short*)(vm.heap + vm.cs + vm.ip);
    vm.ip += 2;
    break;

  case OP_PUSH4:
    vm.stack[--vm.sp] = *(int*)(vm.heap + vm.cs + vm.ip);
    vm.ip += 4;
    break;

  case OP_PUSHLABEL:
    vm.stack[--vm.sp] = vm.cs + readInt16();
    break;

  case OP_PUSHMOD:
    {
      int iImport = readInt16();
      if (!checkModLoaded(iImport)) {
        return false;
      }

      if (-1 != vm.mod[vm.im].imports[iImport].iMod) { // Cached.
        VMImpl::Mod& mod = vm.mod[vm.im];
        VMImpl::Import& import = mod.imports[iImport];
        vm.stack[--vm.sp] = vm.mod[import.iMod].offset + import.offset;
      }
    }
    break;

  case OP_DISCARD:
    vm.sp += 1;
    break;

  case OP_DISCARDX:
    vm.sp += vm.stack[vm.sp] + 1;
    break;

  case OP_DUP:
    vm.stack[--vm.sp] = vm.stack[vm.sp + 1];
    break;

  case OP_SWAP:
    std::swap(vm.stack[vm.sp], vm.stack[vm.sp + 1]);
    break;

  //
  // Heap OP.
  //

  case OP_LOAD1:
    vm.stack[vm.sp] = (int)vm.heap[vm.stack[vm.sp]];
    break;

  case OP_LOAD2:
    vm.stack[vm.sp] = (int)*((short*)vm.heap + vm.stack[vm.sp]);
    break;

  case OP_LOAD4:
    vm.stack[vm.sp] = *((int*)vm.heap + vm.stack[vm.sp]);
    break;

  case OP_SAVE1:
    vm.heap[vm.stack[vm.sp]] = (unsigned char)vm.stack[vm.sp + 1];
    vm.sp += 2;
    break;

  case OP_SAVE2:
    *((short*)vm.heap + vm.stack[vm.sp]) = (short)vm.stack[vm.sp + 1];
    vm.sp += 2;
    break;

  case OP_SAVE4:
    *((int*)vm.heap + vm.stack[vm.sp]) = (int)vm.stack[vm.sp + 1];
    vm.sp += 2;
    break;

  case OP_PARAM:
    vm.stack[vm.sp] += vm.pp;
    break;

  case OP_LOCAL:
    vm.stack[vm.sp] += vm.lp;
    break;

  case OP_BOOK:
    vm.sp -= vm.stack[vm.sp] - 1;
    vm.stack[--vm.xp] = vm.lp;
    vm.lp = vm.sp;
    break;

  case OP_FREE:
    vm.sp += vm.stack[vm.sp] + 1;
    vm.lp = vm.stack[vm.xp++];
    break;

  case OP_LOADPARAM:
    vm.stack[vm.sp] = vm.stack[vm.pp + vm.stack[vm.sp]];
    break;

  case OP_SAVEPARAM:
    vm.stack[vm.pp + vm.stack[vm.sp]] = vm.stack[vm.sp + 1];
    vm.sp += 2;
    break;

  case OP_LOADLOCAL:
    vm.stack[vm.sp] = vm.stack[vm.lp + vm.stack[vm.sp]];
    break;

  case OP_SAVELOCAL:
    vm.stack[vm.lp + vm.stack[vm.sp]] = vm.stack[vm.sp + 1];
    vm.sp += 2;
    break;

  //
  // Arithmetic OP.
  //

  case OP_ADD:
    vm.stack[vm.sp + 1] += vm.stack[vm.sp];
    vm.sp += 1;
    break;

  case OP_SUB:
    vm.stack[vm.sp + 1] -= vm.stack[vm.sp];
    vm.sp += 1;
    break;

  case OP_MUL:
    vm.stack[vm.sp + 1] *= vm.stack[vm.sp];
    vm.sp += 1;
    break;

  case OP_DIV:
    vm.stack[vm.sp + 1] /= vm.stack[vm.sp];
    vm.sp += 1;
    break;

  case OP_MOD:
    vm.stack[vm.sp + 1] %= vm.stack[vm.sp];
    vm.sp += 1;
    break;

  case OP_INC:
    vm.stack[vm.sp]++;
    break;

  case OP_DEC:
    vm.stack[vm.sp]--;
    break;

  case OP_INCPARAM:
    vm.stack[vm.pp + vm.stack[vm.sp]]++;
    vm.sp += 1;
    break;

  case OP_DECPARAM:
    vm.stack[vm.pp + vm.stack[vm.sp]]--;
    vm.sp += 1;
    break;

  case OP_INCLOCAL:
    vm.stack[vm.lp + vm.stack[vm.sp]]++;
    vm.sp += 1;
    break;

  case OP_DECLOCAL:
    vm.stack[vm.lp + vm.stack[vm.sp]]--;
    vm.sp += 1;
    break;

  //
  // Logical OP.
  //

  case OP_NOT:
    vm.stack[vm.sp] = ~ vm.stack[vm.sp];
    break;

  case OP_NEG:
    vm.stack[vm.sp] = - vm.stack[vm.sp];
    break;

  case OP_AND:
    vm.stack[vm.sp + 1] &= vm.stack[vm.sp];
    vm.sp += 1;
    break;

  case OP_OR:
    vm.stack[vm.sp + 1] |= vm.stack[vm.sp];
    vm.sp += 1;
    break;

  case OP_XOR:
    vm.stack[vm.sp + 1] ^= vm.stack[vm.sp];
    vm.sp += 1;
    break;

  case OP_SHL:
    vm.stack[vm.sp] <<= 1;
    break;

  case OP_SHR:
    vm.stack[vm.sp] >>= 1;
    break;

  //
  // Condition OP.
  //

  case OP_JZ:
  case OP_JNZ:
  case OP_JE:
  case OP_JNE:
  case OP_JL:
  case OP_JLE:
  case OP_JG:
  case OP_JGE:
    {
      int ip = readInt16();
      switch (op)
      {
      case OP_JZ:
        if (0 == vm.stack[vm.sp]) {
          vm.ip = ip;
        }
        vm.sp += 1;
        break;

      case OP_JNZ:
        if (0 != vm.stack[vm.sp]) {
          vm.ip = ip;
        }
        vm.sp += 1;
        break;

      case OP_JE:
        if (vm.stack[vm.sp + 1] == vm.stack[vm.sp]) {
          vm.ip = ip;
        }
        vm.sp += 2;
        break;

      case OP_JNE:
        if (vm.stack[vm.sp + 1] != vm.stack[vm.sp]) {
          vm.ip = ip;
        }
        vm.sp += 2;
        break;

      case OP_JL:
        if (vm.stack[vm.sp + 1] < vm.stack[vm.sp]) {
          vm.ip = ip;
        }
        vm.sp += 2;
        break;

      case OP_JLE:
        if (vm.stack[vm.sp + 1] <= vm.stack[vm.sp]) {
          vm.ip = ip;
        }
        vm.sp += 2;
        break;

      case OP_JG:
        if (vm.stack[vm.sp + 1] > vm.stack[vm.sp]) {
          vm.ip = ip;
        }
        vm.sp += 2;
        break;

      case OP_JGE:
        if (vm.stack[vm.sp + 1] >= vm.stack[vm.sp]) {
          vm.ip = ip;
        }
        vm.sp += 2;
        break;

      }
    }
    break;

  //
  // Flow control.
  //

  case OP_JUMP:
    vm.ip = readInt16();
    break;

  case OP_CALL:
    vm.stack[--vm.xp] = vm.ip + 2;
    vm.stack[--vm.xp] = vm.pp;
    vm.ip = readInt16();
    vm.pp = vm.sp;
    break;

  case OP_CALLMOD:
    {
      int iImport = readInt16();
      if (!checkModLoaded(iImport)) {
        return false;
      }

      if (-1 != vm.mod[vm.im].imports[iImport].iMod) { // Cached.
        VMImpl::Mod& mod = vm.mod[vm.im];
        VMImpl::Import& import = mod.imports[iImport];
        if (TAG_NATIVE == import.offset) { // Native call?
          import.native();
        } else {

          //
          // General call.
          //

          vm.stack[--vm.xp] = vm.im;
          vm.stack[--vm.xp] = vm.cs;
          vm.im = import.iMod;
          vm.cs = vm.mod[import.iMod].offset;
          vm.stack[--vm.xp] = vm.ip;
          vm.stack[--vm.xp] = -1;       // MOD call mark.
          vm.stack[--vm.xp] = vm.pp;
          vm.ip = import.offset;
          vm.pp = vm.sp;
        }
      } else {
        VMImpl::Mod& mod = vm.mod[vm.im];
        printf(
          "Unresolved mod call %s.%s '%s.mod'\n",
          mod.names[mod.imports[iImport].iModName].c_str(),
          mod.names[mod.imports[iImport].iLabelName].c_str(),
          mod.modName.c_str());
        return false;
      }
    }
    break;

  case OP_RET:
    vm.pp = vm.stack[vm.xp++];
    vm.ip = vm.stack[vm.xp++];
    if (-1 == vm.ip) {                  // Returned from a mod call.
      vm.ip = vm.stack[vm.xp++];
      vm.cs = vm.stack[vm.xp++];
      vm.im = vm.stack[vm.xp++];
      if (-1 == vm.im) {                // Finished mod, end of process.
        return false;
      }
    }
    break;

  default:
    printf("Unknown OP code: %d\n", op);
    return false;

  }

  return true;                          // Not finish, need next step.
}

// end of vm.cpp
