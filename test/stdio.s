;
; stdio lib
;
; 2012/03/28 Waync Cheng
;

export printf

;
; void printf(const char* format, ...)
;
; %specifier 
; c   character
; d   decimal integer
; s   string
;

printf:
  push        1
  book

  push        0
  dup
  savelocal

prt_1:
  push        0
  loadparam
  load1
  dup
  jz          prt_end

  dup
  push        '%'
  jne         prt_chr

  discard

  push        0
  dup
  incparam

  loadparam
  load1
  dup
  push        'd'
  jne         prt_chkstr

  discard

  push        0
  loadlocal
  inc
  loadparam
  call        system.outint

prt_handled:
  push        0
  inclocal
  
  jump        prt_next

prt_chkstr:
  dup
  push        's'
  jne         prt_chkchr

  discard

  push        0
  loadlocal
  inc
  loadparam
  call        system.outstr

  jump        prt_handled
  
prt_chkchr:
  push        'c'
  jne         prt_next

  push        0
  loadlocal
  inc
  loadparam
  call        system.outchr

  jump        prt_handled

prt_chr:
  call        system.outchr

prt_next:
  push        0
  incparam

  jump        prt_1

prt_end:
  push        0
  loadlocal
  push        3
  add
  free

  ret
