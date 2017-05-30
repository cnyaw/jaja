%{
#include <stdio.h>
#include <string.h>

#include "../op.h"

#include "as.h"

extern int yylex();
extern FILE* yyin;

void yyerror(char*);
int yypush(int val);

int line;

%}

%union
{
  int val;
  char *pstr;
  char str[128];
};

%token NATIVE EXPORT
%token DB
%token RESB

%token PUSH DISCARD DISCARDX DUP SWAP
%token LOAD1 LOAD2 LOAD4 SAVE1 SAVE2 SAVE4
%token PARAM LOCAL BOOK FREE LOADPARAM SAVEPARAM LOADLOCAL SAVELOCAL
%token ADD SUB MUL DIV MOD INC DEC
%token INCPARAM DECPARAM
%token INCLOCAL DECLOCAL
%token NOT NEG AND OR XOR SHL SHR
%token JZ JNZ JE JNE JL JLE JG JGE
%token JUMP CALL RET

%token <val> NUM
%token <str> ID
%token <pstr> STR

%%

lines
  :
  | lines line '\n'   { line ++; }
  ;

line
  : EXPORT export
  | NATIVE native
  | ID ':' { addNode(ID, $1); } instruction
  | ID instruction    { yyerror("':' expected"); }
  | instruction
  ;

export
  : ID                { addNode(EXPORT, $1); }
  | export ',' ID     { addNode(EXPORT, $3); }
  | export ID         { yyerror("',' expected"); }
  ;

native
  : ID                { addNode(NATIVE, $1); }
  | native ',' ID     { addNode(NATIVE, $3); }
  | native ID         { yyerror("',' expected"); }
  ;

const
  : NUM               { addNode(DB, $1); }
  | STR               { addNode(DB, $1); }
  | const ',' NUM     { addNode(DB, $3); }
  | const NUM         { yyerror("',' expected"); }
  | const ',' STR     { addNode(DB, $3); }
  | const STR         { yyerror("',' expected"); }
  ;

instruction
  : /* EMPTY */
  | DB const
  | RESB NUM          { addNode(RESB, $2); }
  | PUSH NUM          { addNode(yypush($2), $2); }
  | PUSH ID           { addNode(OP_PUSHLABEL, $2); }
  | PUSH ID '.' ID    { addNode(OP_PUSHMOD, $2, $4); }
  | DISCARD           { addNode(OP_DISCARD); }
  | DISCARDX          { addNode(OP_DISCARDX); }
  | DUP               { addNode(OP_DUP); }
  | SWAP              { addNode(OP_SWAP); }
  | LOAD1             { addNode(OP_LOAD1); }
  | LOAD2             { addNode(OP_LOAD2); }
  | LOAD4             { addNode(OP_LOAD4); }
  | SAVE1             { addNode(OP_SAVE1); }
  | SAVE2             { addNode(OP_SAVE2); }
  | SAVE4             { addNode(OP_SAVE4); }
  | PARAM             { addNode(OP_PARAM); }
  | LOCAL             { addNode(OP_LOCAL); }
  | BOOK              { addNode(OP_BOOK); }
  | FREE              { addNode(OP_FREE); }
  | LOADPARAM         { addNode(OP_LOADPARAM); }
  | LOADLOCAL         { addNode(OP_LOADLOCAL); }
  | SAVEPARAM         { addNode(OP_SAVEPARAM); }
  | SAVELOCAL         { addNode(OP_SAVELOCAL); }
  | ADD               { addNode(OP_ADD); }
  | SUB               { addNode(OP_SUB); }
  | MUL               { addNode(OP_MUL); }
  | DIV               { addNode(OP_DIV); }
  | MOD               { addNode(OP_MOD); }
  | INC               { addNode(OP_INC); }
  | DEC               { addNode(OP_DEC); }
  | INCPARAM          { addNode(OP_INCPARAM); }
  | DECPARAM          { addNode(OP_DECPARAM); }
  | INCLOCAL          { addNode(OP_INCLOCAL); }
  | DECLOCAL          { addNode(OP_DECLOCAL); }
  | NOT               { addNode(OP_NOT); }
  | NEG               { addNode(OP_NEG); }
  | AND               { addNode(OP_AND); }
  | OR                { addNode(OP_OR); }
  | XOR               { addNode(OP_XOR); }
  | SHL               { addNode(OP_SHL); }
  | SHR               { addNode(OP_SHR); }
  | JZ ID             { addNode(OP_JZ, $2); }
  | JNZ ID            { addNode(OP_JNZ, $2); }
  | JE ID             { addNode(OP_JE, $2); }
  | JNE ID            { addNode(OP_JNE, $2); }
  | JL ID             { addNode(OP_JL, $2); }
  | JLE ID            { addNode(OP_JLE, $2); }
  | JG ID             { addNode(OP_JG, $2); }
  | JGE ID            { addNode(OP_JGE, $2); }
  | JUMP ID           { addNode(OP_JUMP, $2); }
  | CALL ID           { addNode(OP_CALL, $2); }
  | CALL ID '.' ID    { addNode(OP_CALLMOD, $2, $4); }
  | RET               { addNode(OP_RET); }
  ;

%%

char currName[256];

int yypush(int val)
{
  if (-128 <= val && 127 >= val) {
    return OP_PUSH1;
  } else if (-32768 <= val && 32767 >= val) {
    return OP_PUSH2;
  } else {
    return OP_PUSH4;
  }
}

void yyerror(char* msg)
{
  printf("%s:%d: %s\n", currName, line, msg);
  exit(1);
}

int main(int argc, char** argv)
{
  printf("jaja assembler (alpha), 2009 Waync Cheng.\n");

  if (1 == argc) {
    printf("Usage: as file...\n");
    exit(0);
  }

  for (int i = 0; i < argc - 1; i ++) {

    strcpy(currName, argv[1 + i]);

    FILE* pFile = fopen(currName, "rt");
    if (0 == pFile) {
      printf("Error: can not open file [%s]\n", currName);
      continue;
    }

    line = 1;
    initAssemble();

    yyin = pFile;
    yyparse();

    fclose(pFile);

    processAssemble();
    uninitAssemble();
  }
}
