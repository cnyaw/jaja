//
// jaja, assembler.
//
// 2009/08/14 Waync Cheng
//

#pragma once

#include <stdio.h>
#include <string.h>

#define CONST_MAX_NAME_LEN 128

void addNode(int op);
void addNode(int op, int val);
void addNode(int op, const char* id);
void addNode(int op, const char* id, const char* id2);

void initAssemble();
void processAssemble();
void uninitAssemble();

// end of as.h
