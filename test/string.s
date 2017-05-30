;
; std string lib
;
; 2012/03/27 Waync Cheng
;

export strlen, strcpy, strcat, strcmp

;
; size_t strlen(const char* s)
;

strlen:
  push        0

slen_loop:
  push        0
  loadparam
  load1
  jz          slen_end

  push        0
  incparam

  inc

  jump        slen_loop

slen_end:
  push        0
  saveparam

  ret

;
; void strcpy(char* dst, const char* src)
;

strcpy:
  push        1
  loadparam
  load1
  dup
  jz          scpy_end

  push        0
  loadparam
  save1

  push        0
  incparam
  push        1
  incparam

  jump        strcpy

scpy_end:
  push        0
  dup
  loadparam
  save1

  push        3
  discardx

  ret

;
; void strcat(char* dst, const char* src)
;

strcat:
  push        0
  loadparam
  load1
  jz          scat_1

  push        0
  incparam

  jump        strcat

scat_1:
  push        1
  loadparam
  push        0
  loadparam
  call        strcpy

  push        2
  discardx

  ret

;
; int strcmp(const char* s1, const char* s2)
;

strcmp:
  push        1
  loadparam
  load1
  push        0
  loadparam
  load1
  sub
  dup
  jnz         scmp_end

  push        1
  loadparam
  load1
  jz          scmp_end

  push        0
  incparam
  push        1
  incparam

  discard
  jump        strcmp

scmp_end:
  push        1
  saveparam
  discard
  ret
