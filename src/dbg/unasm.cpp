//
// jaja assembler debugger, unassemble.
//
// 2009/08/16 Waync Cheng
//

#include <iostream>
#include <string>

#include "../op.h"
#include "../vm/ccpp/vm.h"
#include "../vm/ccpp/vm.hxx"

//
// Global variable.
//

extern VMImpl vm;
extern int g_addrUnasm;

//
// Unassemble.
//

int outputCmdNum(int addr, char const* cmd, bool relocate = true)
{
  printf("%02X", vm.heap[addr]);
  printf("%02X", vm.heap[addr + 1]);
  int a = vm.heap[addr], b = vm.heap[addr + 1] << 8;
  printf("\t\t%-10s\t%04X\n", cmd, (relocate ? vm.cs : 0) + (a | b));
  addr += 2;
  return addr;
}

int unasm1cmd(int addr)
{
  printf("%08X ", addr);

  int op = vm.heap[addr++] & 0xff;
  printf("%02X", op);

  int a, b;
  switch (op)
  {
  case OP_NOP:
    printf("\t\tNOP\n");
    break;
  // stack
  case OP_PUSH1:
    printf("%02X", vm.heap[addr]);
    printf("\t\t%-10s\t%02X\n", "PUSH", vm.heap[addr]);
    addr += 1;
    break;
  case OP_PUSH2:
    printf("%02X", vm.heap[addr]);
    printf("%02X", vm.heap[addr + 1]);
    printf("\t\t%-10s\t%04X\n", "PUSH", vm.heap[addr] | (vm.heap[addr + 1] << 8));
    addr += 2;
    break;
  case OP_PUSH4:
    printf("%02X", vm.heap[addr]);
    printf("%02X", vm.heap[addr + 1]);
    printf("%02X", vm.heap[addr + 2]);
    printf("%02X", vm.heap[addr + 3]);
    printf("\t%-10s\t%08X\n", "PUSH", vm.heap[addr] | (vm.heap[addr + 1] << 8) | (vm.heap[addr + 2] << 16) | (vm.heap[addr + 3] << 24));
    addr += 4;
    break;
  case OP_PUSHLABEL:
    printf("%02X", vm.heap[addr]);
    printf("%02X", vm.heap[addr + 1]);
    a = vm.heap[addr], b = vm.heap[addr + 1] << 8;
    printf("\t\t%-10s\t%04X\n", "PUSH", vm.cs + (a|b));
    addr += 2;
    break;
  case OP_PUSHMOD:
    addr = outputCmdNum(addr, "PUSHMOD");
    break;
  case OP_DISCARD:
    printf("\t\tDISCARD\n");
    break;
  case OP_DISCARDX:
    printf("\t\tDISCARDX\n");
    break;
  case OP_DUP:
    printf("\t\tDUP\n");
    break;
  case OP_SWAP:
    printf("\t\tSWAP\n");
    break;
  // heap
  case OP_LOAD1:
    printf("\t\tLOAD1\n");
    break;
  case OP_LOAD2:
    printf("\t\tLOAD2\n");
    break;
  case OP_LOAD4:
    printf("\t\tLOAD4\n");
    break;
  case OP_SAVE1:
    printf("\t\tSAVE1\n");
    break;
  case OP_SAVE2:
    printf("\t\tSAVE2\n");
    break;
  case OP_SAVE4:
    printf("\t\tSAVE4\n");
    break;
  case OP_PARAM:
    printf("\t\tPARAM\n");
    break;
  case OP_LOCAL:
    printf("\t\tLOCAL\n");
    break;
  case OP_BOOK:
    printf("\t\tBOOK\n");
    break;
  case OP_FREE:
    printf("\t\tFREE\n");
    break;
  case OP_LOADPARAM:
    printf("\t\tLOADPARAM\n");
    break;
  case OP_SAVEPARAM:
    printf("\t\tSAVEPARAM\n");
    break;
  case OP_LOADLOCAL:
    printf("\t\tLOADLOCAL\n");
    break;
  case OP_SAVELOCAL:
    printf("\t\tSAVELOCAL\n");
    break;
  // arithmetic
  case OP_ADD:
    printf("\t\tADD\n");
    break;
  case OP_SUB:
    printf("\t\tSUB\n");
    break;
  case OP_MUL:
    printf("\t\tMUL\n");
    break;
  case OP_DIV:
    printf("\t\tDIV\n");
    break;
  case OP_MOD:
    printf("\t\tMOD\n");
    break;
  case OP_INC:
    printf("\t\tINC\n");
    break;
  case OP_DEC:
    printf("\t\tDEC\n");
    break;
  case OP_INCPARAM:
    printf("\t\tINCPARAM\n");
    break;
  case OP_DECPARAM:
    printf("\t\tDECPARAM\n");
    break;
  case OP_INCLOCAL:
    printf("\t\tINCLOCAL\n");
    break;
  case OP_DECLOCAL:
    printf("\t\tDECLOCAL\n");
    break;
  // logic
  case OP_NOT:
    printf("\t\tNOT\n");
    break;
  case OP_NEG:
    printf("\t\tNEG\n");
    break;
  case OP_AND:
    printf("\t\tAND\n");
    break;
  case OP_OR:
    printf("\t\tOR\n");
    break;
  case OP_XOR:
    printf("\t\tXOR\n");
    break;
  case OP_SHL:
    printf("\t\tSHL\n");
    break;
  case OP_SHR:
    printf("\t\tSHR\n");
    break;
  // condition
  case OP_JZ:
    addr = outputCmdNum(addr, "JZ");
    break;
  case OP_JNZ:
    addr = outputCmdNum(addr, "JNZ");
    break;
  case OP_JE:
    addr = outputCmdNum(addr, "JE");
    break;
  case OP_JNE:
    addr = outputCmdNum(addr, "JNE");
    break;
  case OP_JL:
    addr = outputCmdNum(addr, "JL");
    break;
  case OP_JLE:
    addr = outputCmdNum(addr, "JLE");
    break;
  case OP_JG:
    addr = outputCmdNum(addr, "JG");
    break;
  case OP_JGE:
    addr = outputCmdNum(addr, "JGE");
    break;
  // flow control
  case OP_JUMP:
    addr = outputCmdNum(addr, "JUMP");
    break;
  case OP_CALL:
    addr = outputCmdNum(addr, "CALL");
    break;
  case OP_CALLMOD:
    if (-1 != vm.im)
    {
      printf("%02X", vm.heap[addr]);
      printf("%02X", vm.heap[addr + 1]);
      a = vm.heap[addr], b = vm.heap[addr + 1] << 8;
      int iImport = a | b;

      for (size_t i = 1; i < vm.mod.size(); i++)
      {
          VMImpl::Mod const& m = vm.mod[i];
          if (m.offset <= addr && m.offset + m.size > addr)
          {
              if ((int)m.imports.size() <= iImport)
              {
                  printf("\t\t%-10s\t%04X\n", "CALLMOD", iImport);
                  break;
              }

              VMImpl::Import const& import = m.imports[iImport];
              std::string name = m.names[import.iLabelName];
              std::string modName = m.names[import.iModName];
              printf("\t\t%-10s\t%s.%s\n", "CALL", modName.c_str(), name.c_str());

              break;
          }
      }

      addr += 2;
    }
    else
      addr = outputCmdNum(addr, "CALLMOD");
    break;
  case OP_RET:
    printf("\t\tRET\n");
    break;
  default:
    printf("\t\t%-10s\t%02X\n", "DB", op);
    break;
  }

  return addr;
}

void unasm(int addr, int count)
{
  if (0 <= addr && vm.heapSize > addr)
    g_addrUnasm = addr;

  for (int i = 0; i < count;++i)
  {
    g_addrUnasm = unasm1cmd(g_addrUnasm);
    if (g_addrUnasm >= vm.heapSize)
      break;
  }
}

// end of unasm.cpp
