;
; string sample
;
; 2012/03/27 Waync Cheng
;

  call        test_strcpy
  call        test_strcmp
  call        test_strlen

  ret

;
; strcpy
;

msg1:         DB "hello world! ", 0
              RESB 16
var:          RESB 32

test_strcpy:

  ; reserve 32 bytes local variable

  push        8
  book

  ; string copy msg1 -> local[0]

  push        msg1
  push        0
  local
  call        string.strcpy

  ; string copy local[0] -> var

  push        0
  local
  push        var
  call        string.strcpy

  ; append hello.msg -> var

  push        hello.msg
  push        var
  call        string.strcat

  ; print string var

  push        var
  call        system.outstr

  ; free 32 bytes local variable

  push        8
  free

  ret

;
; strcmp
;

ch1:          resb 1
scmpmsg:      db "compare string: %s %c %s\n", 0
s1:           db "4", 0
s2:           db "45", 0

test_strcmp:
  push        s1
  push        s2
  call        string.strcmp             ; strcmp(s2, s1)

  dup
  push        0
  jg          scmp_g

  dup
  push        0
  jl          scmp_l

  push        '='
  jump        scmp_n

scmp_g:
  push        '>'
  jump        scmp_n

scmp_l:
  push        '<'

scmp_n:
  push        ch1
  save1

  discard

  push        s2

  push        ch1
  load1

  push        s1
  push        scmpmsg
  call        stdio.printf
  ret

;
; strlen
;

test_strlen:
  push        msg
  call        string.strlen
  push        msg
  push        fmt
  call        stdio.printf
  ret

fmt:          db "strlen of %s is: %d\n", 0
msg:          db "'This is a string.'", 0
