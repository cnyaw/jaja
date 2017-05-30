//
// jaja, assembler.
//
// 2009/08/14 Waync Cheng
//

#include "as.h"
#include "y.tab.h"

#include "../op.h"
#include "../mod.h"

extern void yyerror(char*);

extern int line;
extern char currName[];

int offset;                             // Current offset.
int size;                               // Code size.

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

// internal
struct Label
{
  int iName;
  int offset;
};

struct Import
{
  int iMod;
  int iLabel;
};

struct Node
{
  int op;                               // OP code or node ID.
  int iName, iName2;
  int iConst;
  int val;
  int line;                             // Line number.
};

//
// Phase 1 tables.
//

std::vector<std::string> names, consts;
std::vector<Label> labels;
std::vector<Import> imports;
std::vector<Node> nodes;

//
// Phase 2 tables.
//

std::vector<std::string> namesTable;
std::vector<Label> exportsTable;
std::vector<Label> nativesTable;
std::vector<Import> importsTable;

void addNameTable(int iName)
{
  for (int i = 0; i < namesTable.size(); i++) {
    if (namesTable[i] == names[iName]) {
      return;
    }
  }

  namesTable.push_back(names[iName]);
}

int addExportTable(int iName)
{
  for (int i = 0; i < exportsTable.size(); i++) {
    if (names[iName] == namesTable[exportsTable[i].iName]) {
      return i;
    }
  }

  for (int i = 0; i < labels.size(); i++) {
    if (labels[i].iName == iName) {
      for (int j = 0; j < namesTable.size(); j++) {
        if (names[labels[i].iName] == namesTable[j]) {
          Label e;
          e.iName = j;
          e.offset = labels[i].offset;
          exportsTable.push_back(e);
          return i;
        }
      }
    }
  }

  return -1;
}

int addNativeTable(int iName)
{
  for (int i = 0; i < nativesTable.size(); i++) {
    if (names[iName] == namesTable[nativesTable[i].iName]) {
      return i;
    }
  }

  for (int i = 0; i < namesTable.size(); i++) {
    if (names[iName] == namesTable[i]) {
      Label e;
      e.iName = i;
      e.offset = TAG_NATIVE;
      nativesTable.push_back(e);
      return i;
    }
  }

  return -1;
}

int addImportTable(int iName, int iName2)
{
  for (int i = 0; i < importsTable.size(); i++) {
    if (names[iName] == namesTable[importsTable[i].iMod] &&
        names[iName2] == namesTable[importsTable[i].iLabel]) {
      return i;
    }
  }

  for (int i = 0; i < namesTable.size(); i++) {
    if (names[iName] == namesTable[i]) {
      for (int j = 0; j < namesTable.size(); j++) {
        if (names[iName2] == namesTable[j]) {
          Import im;
          im.iMod = i;
          im.iLabel = j;
          importsTable.push_back(im);
          return importsTable.size() - 1;
        }
      }
    }
  }

  return -1;
}

int findName(const char* str)
{
  if (0 == str) {
    return -1;
  }

  //
  // Find existence.
  //

  for (int i = 0; i < names.size(); i++) {
    if (names[i] == str) {
      return i;
    }
  }

  //
  // Add new.
  //

  names.push_back(str);

  return names.size() - 1;
}

int findConst(const char* str)
{
  if (0 == str) {
    return -1;
  }

  //
  // Find existence.
  //

  for (int i = 0; i < consts.size(); i++) {
    if (consts[i] == str) {
      return i;
    }
  }

  //
  // Add new.
  //

  consts.push_back(str);

  return consts.size() - 1;
}

void newLabel(const char* str)
{
  //
  // Check existence.
  //

  for (int i = 0; i < labels.size(); i++) {
    if (names[labels[i].iName] == str) {
      char msg[CONST_MAX_NAME_LEN];
      sprintf(msg, "'%s' redefinition", str);
      yyerror(msg);
      return;
    }
  }

  //
  // Allocate new.
  //

  Label lb;
  lb.iName = findName(str);
  lb.offset = offset;

  labels.push_back(lb);
}

int findImport(const char* mod, const char* lb)
{
  for (int i = 0; i < imports.size(); i++) {
    if (names[imports[i].iMod] == mod && names[imports[i].iLabel] == lb) {
      return i;
    }
  }

  Import ip;
  ip.iMod = findName(mod);
  ip.iLabel = findName(lb);

  imports.push_back(ip);

  return imports.size() - 1;
}

int findLabel(int iName)
{
  if (-1 == iName) {
    return -1;
  }

  for (int i = 0; i < labels.size(); i++) {
    if (labels[i].iName == iName) {
      return labels[i].offset;
    }
  }

  return -1;
}

void newNode(const Node& n)
{
  offset = size;
  switch (n.op)
  {
  case DB:
    size += (-1 == n.iConst) ? 1 : 1 * consts[n.iConst].size() - 2;
    break;

  case RESB:
    size += n.val;
    break;

  case OP_DISCARD:case OP_DISCARDX:case OP_DUP:case OP_SWAP:
  case OP_LOAD1:case OP_LOAD2:case OP_LOAD4:case OP_SAVE1:case OP_SAVE2:case OP_SAVE4:
  case OP_ADD:case OP_SUB:case OP_MUL:case OP_DIV:case OP_MOD:case OP_INC:case OP_DEC:
  case OP_NOT:case OP_NEG:case OP_AND:case OP_OR:case OP_XOR:case OP_SHL:case OP_SHR:case OP_RET:
  case OP_LOADPARAM:case OP_LOADLOCAL:case OP_SAVEPARAM:case OP_SAVELOCAL:
  case OP_PARAM:case OP_LOCAL:case OP_BOOK:case OP_FREE:case OP_INCPARAM:
  case OP_INCLOCAL:case OP_DECPARAM:case OP_DECLOCAL:
    size += 1;
    break;

  case OP_PUSH1:
    size += 2;
    break;

  case OP_PUSH2:case OP_PUSHLABEL:case OP_PUSHMOD:
  case OP_JZ:case OP_JNZ:case OP_JE:case OP_JNE:case OP_JL:case OP_JLE:case OP_JG:
  case OP_JGE:case OP_JUMP:case OP_CALL:case OP_CALLMOD:
    size += 3;
    break;

  case OP_PUSH4:
    size += 5;
    break;

  }

  if (64 * 1024 < size) {
    yyerror("Code size exceed 64k limit.");
  }
}

void newNode(int op, int val, const char* id, const char* id2)
{
  Node n;
  n.op = op;
  n.line = line;

  if (DB == op) {
    n.iName = n.iName2 = -1;
    n.iConst = findConst(id);
    n.val = val;
  } else {
    n.iName = findName(id);
    n.iName2 = findName(id2);
    n.iConst = -1;
    n.val = val;
  }

  newNode(n);

  switch (op)
  {
  case NATIVE:
  case EXPORT:
    break;

  case ID:
    newLabel(id);
    break;

  case OP_CALLMOD:
    findImport(id, id2);
    break;

  }

  nodes.push_back(n);
}

void outputMod(FILE* pFile)
{
  //
  // Tables.
  //

  if (namesTable.size()) {
    int tag = TAG_NAME;
    fwrite(&tag, 1, 1, pFile);
    int nItem = namesTable.size();
    fwrite(&nItem, 2, 1, pFile);
    for (int i = 0; i < nItem; i++) {
      int len = namesTable[i].size();
      fwrite(&len, 2, 1, pFile);
      fwrite(namesTable[i].c_str(), 1, len, pFile);
    }
  }

  if (exportsTable.size() || nativesTable.size()) {
    int tag = TAG_EXPORT;
    fwrite(&tag, 1, 1, pFile);
    int nItem = exportsTable.size() + nativesTable.size();
    fwrite(&nItem, 2, 1, pFile);
    for (int i = 0; i < exportsTable.size(); i++) {
      fwrite(&exportsTable[i].iName, 2, 1, pFile);
      fwrite(&exportsTable[i].offset, 2, 1, pFile);
    }
    for (int i = 0; i < nativesTable.size(); i++) {
      fwrite(&nativesTable[i].iName, 2, 1, pFile);
      fwrite(&nativesTable[i].offset, 2, 1, pFile);
    }
  }

  if (importsTable.size()) {
    int tag = TAG_IMPORT;
    fwrite(&tag, 1, 1, pFile);
    int nItem = importsTable.size();
    fwrite(&nItem, 2, 1, pFile);
    for (int i = 0; i < nItem; i++) {
      fwrite(&importsTable[i].iMod, 2, 1, pFile);
      fwrite(&importsTable[i].iLabel, 2, 1, pFile);
    }
  }

  //
  // Codes.
  //

  if (size) {
    int tag = TAG_CODE;
    fwrite(&tag, 1, 1, pFile);
    fwrite(&size, 2, 1, pFile);
    for (int i = 0; i < nodes.size(); i++) {
      Node& n = nodes[i];
      switch (n.op)
      {
      case NATIVE:case EXPORT:
      case ID:
        break;

      case DB:
        if (-1 != n.iConst) {
          fwrite(consts[n.iConst].c_str() + 1, 1, consts[n.iConst].size() - 2, pFile);
        } else {
          fwrite(&n.val, 1, 1, pFile);
        }
        break;

      case RESB:
        {
          char* p = new char[n.val];
          memset(p, 0, n.val);
          fwrite(p, 1, n.val, pFile);
          delete [] p;
        }
        break;

      case OP_PUSH1:
        fwrite(&n.op, 1, 1, pFile);
        fwrite(&n.val, 1, 1, pFile);
        break;

      case OP_PUSH4:
        fwrite(&n.op, 1, 1, pFile);
        fwrite(&n.val, 1, 4, pFile);
        break;

      case OP_DISCARD:case OP_DISCARDX:case OP_DUP:case OP_SWAP:
      case OP_LOAD1:case OP_LOAD2:case OP_LOAD4:case OP_SAVE1:case OP_SAVE2:case OP_SAVE4:
      case OP_ADD:case OP_SUB:case OP_MUL:case OP_DIV:case OP_MOD:case OP_INC:case OP_DEC:
      case OP_NOT:case OP_NEG:case OP_AND:case OP_OR:case OP_XOR:case OP_SHL:case OP_SHR:
      case OP_RET:
      case OP_LOADPARAM:case OP_LOADLOCAL:case OP_SAVEPARAM:case OP_SAVELOCAL:
      case OP_PARAM:case OP_LOCAL:case OP_BOOK:case OP_FREE:
      case OP_INCPARAM:case OP_INCLOCAL:case OP_DECPARAM:case OP_DECLOCAL:
        fwrite(&n.op, 1, 1, pFile);
        break;

      case OP_JZ:case OP_JNZ:case OP_JE:case OP_JNE:case OP_JL:case OP_JLE:
      case OP_JG:case OP_JGE:case OP_JUMP:case OP_CALL:
      case OP_PUSH2:case OP_PUSHLABEL:case OP_PUSHMOD:case OP_CALLMOD:
        fwrite(&n.op, 1, 1, pFile);
        fwrite(&n.val, 1, 2, pFile);
        break;

      }
    }
  }

  //
  // End tag.
  //

  {
    int tag = TAG_DONE;
    fwrite(&tag, 1, 1, pFile);
  }
}

void outputMod()
{
  //
  // Find out output name.
  //

  std::string outName = currName;
  std::string::size_type idx = outName.rfind(".", outName.size(), 1);
  outName = outName.substr(0, idx) + ".m";

  //
  // Open it to write.
  //

  FILE* pFile = fopen(outName.c_str(), "wb");
  if (0 == pFile) {
    char msg[CONST_MAX_NAME_LEN];
    sprintf(msg, "create output file '%s' failed", outName.c_str());
    yyerror(msg);
    return;
  }

  std::cout << "'" << currName << "' assembled, output '" << outName << "'..\n";

  //
  // Output file header.
  //

  //fwrite(HDR_DUMMY, sizeof(HDR_DUMMY), 1, pFile);
  {
    char p[sizeof(HDR_DUMMY)];
    fwrite(p, sizeof(HDR_DUMMY), 1, pFile);
  }
  fwrite(HDR_TAG, sizeof(HDR_TAG), 1, pFile);
  fwrite(HDR_VER, sizeof(HDR_VER), 1, pFile);

  //
  // Output content.
  //

  outputMod(pFile);

  fclose(pFile);
}

void validate()                         // And prepare output tables.
{
  for (int i = 0; i < nodes.size(); i++) {
    Node& n = nodes[i];
    switch (n.op)
    {
    case NATIVE:
      addNameTable(n.iName);
      if (-1 == addNativeTable(n.iName)) {
        char msg[CONST_MAX_NAME_LEN];
        sprintf(msg, "'%s' undeclared identifier", names[n.iName].c_str());
        line = n.line;
        yyerror(msg);
      }
      break;

    case EXPORT:
      addNameTable(n.iName);
      if (-1 == addExportTable(n.iName)) {
        char msg[CONST_MAX_NAME_LEN];
        sprintf(msg, "'%s' undeclared identifier", names[n.iName].c_str());
        line = n.line;
        yyerror(msg);
      }
      break;

    case OP_PUSHLABEL:case OP_JZ:case OP_JNZ:case OP_JE:case OP_JNE:case OP_JL:
    case OP_JLE:case OP_JG:case OP_JGE:case OP_JUMP:case OP_CALL:
      if (-1 == (n.val = findLabel(n.iName))) {
        char msg[CONST_MAX_NAME_LEN];
        sprintf(msg, "'%s' undeclared identifier", names[n.iName].c_str());
        line = n.line;
        yyerror(msg);
        return;
      }
      break;

    case OP_PUSHMOD:case OP_CALLMOD:
      addNameTable(n.iName);
      addNameTable(n.iName2);
      n.val = addImportTable(n.iName, n.iName2);
      break;

    }
  }
}

//
// Public.
//

void addNode(int op)
{
  newNode(op, 0, 0, 0);
}

void addNode(int op, int val)
{
  newNode(op, val, 0, 0);
}

void addNode(int op, const char* id)
{
  newNode(op, 0, id, 0);
}

void addNode(int op, const char* id, const char* id2)
{
  newNode(op, 0, id, id2);
}

void initAssemble()
{
  size = offset = 0;
}

void processAssemble()
{
  validate();
  outputMod();
}

void uninitAssemble()
{
  names.clear();
  consts.clear();
  labels.clear();
  imports.clear();
  nodes.clear();

  namesTable.clear();
  exportsTable.clear();
  nativesTable.clear();
  importsTable.clear();
}

// end of file as.cpp
