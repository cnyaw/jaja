//
// jaja, instrcution set OP code definitions.
//
// 2009/08/14 Waync Cheng
//

#ifndef JAJA_OP
#define JAJA_OP

#define TAG_NATIVE 0xffff

enum JAJA_OPCODE_
{
  OP_NOP,
  // stack
  OP_PUSH1,
  OP_PUSH2,
  OP_PUSH4,
  OP_PUSHLABEL,
  OP_PUSHMOD,
  OP_DISCARD,
  OP_DISCARDX,
  OP_DUP,
  OP_SWAP,
  // heap
  OP_LOAD1,
  OP_LOAD2,
  OP_LOAD4,
  OP_SAVE1,
  OP_SAVE2,
  OP_SAVE4,
  OP_PARAM,
  OP_LOCAL,
  OP_BOOK,
  OP_FREE,
  OP_LOADPARAM,
  OP_SAVEPARAM,
  OP_LOADLOCAL,
  OP_SAVELOCAL,
  // arithmetic
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV,
  OP_MOD,
  OP_INC,
  OP_DEC,
  OP_INCPARAM,
  OP_DECPARAM,
  OP_INCLOCAL,
  OP_DECLOCAL,
  // logic
  OP_NOT,
  OP_NEG,
  OP_AND,
  OP_OR,
  OP_XOR,
  OP_SHL,
  OP_SHR,
  // condition
  OP_JZ,
  OP_JNZ,
  OP_JE,
  OP_JNE,
  OP_JL,
  OP_JLE,
  OP_JG,
  OP_JGE,
  // flow control
  OP_JUMP,
  OP_CALL,
  OP_CALLMOD,
  OP_RET,
};

#endif // JAJA_OP

// end of op.h
