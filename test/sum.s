;
; test, init a[10] = 1..10, calc sum of a[]
;
; 2009/08/14 Waync Cheng
;

;
;  init local variable a[10] to 1..10, and calc sum of a[]
;

main:

  push        a
  push        10
  call        inita

  push        a
  push        10
  call        suma

  push        msg
  call        stdio.printf

  ret

msg:          db "calc sum of 1 + 2 + .. + 10 = %d\n", 0
a:            RESB 10
i:            RESB 1
sum:          RESB 1

;
; void init(n, a)
;   var i
;

inita:

  push        0         ; i = 0
  push        i
  save1

init0:

  push        i
  load1
  inc                   ; i + 1

  push        i
  load1                 ; i
  push        1
  loadparam             ; a

  add                   ; a + i
  save1                 ; addr[a + i] = a[i] = i + 1

  push        i
  load1
  inc
  push        i
  save1                 ; i ++

  push        i
  load1                 ; i
  push        0
  loadparam             ; n

  jne         init0     ; i == n ?

  ret

;
; int suma(n, a)
;   var sum
;   var i
;

suma:

  push        0         ; i = 0
  push        i
  save1

  push        0         ; sum = 0
  push        sum
  save1

sum0:

  push        1
  loadparam             ; a

  push        i
  load1

  add                   ; a[i]
  load1

  push        sum
  load1

  add                   ; sum + a[i]

  push        sum
  save1                 ; sum = sum + a[i]

  push        i
  load1
  inc
  push        i
  save1                 ; i ++

  push        i
  load1

  push        0
  loadparam             ; n

  jne         sum0      ; i == n ?

  push        sum
  load1

  ret
