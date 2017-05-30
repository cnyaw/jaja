;
; test, init a[10] = 1..10, calc sum of a[]
;
; 2009/08/14 Waync Cheng
;

;
;  init local variable a[10] to 1..10, and calc sum of a[]
;

main:

  push        10
  book

  push        0
  local
  push        10
  call        init

  push        0
  local
  push        10
  call        sum

  push        msg
  call        stdio.printf

  push        10
  free
  ret

msg:  db    "calc sum of 1 + 2 + .. + 10 = %d\n", 0

;
; void init(n, a)
;   var i
;

init:

  push        1
  book

  push        0         ; i = 0
  dup
  savelocal

init0:

  push        0
  loadlocal             ; i + 1
  inc

  push        0
  loadlocal             ; i
  push        1
  loadparam             ; a

  add                   ; a + i
  save4                 ; addr[a + i] = a[i] = i + 1

  push        0
  inclocal              ; i ++

  push        0
  loadlocal             ; i
  push        0
  loadparam             ; n

  jne         init0     ; i == n ?

  push        3
  free                  ; release a, n, i

  ret

;
; int sum(n, a)
;   var sum
;   var i
;

sum:

  push        2
  book

  push        0         ; i = 0
  push        1
  savelocal

  push        0         ; sum = 0
  push        0
  savelocal

sum0:

  push        1
  loadparam             ; a
  push        1
  loadlocal             ; i

  add                   ; a[i]
  load4

  push        0
  loadlocal             ; sum

  add                   ; sum + a[i]

  push        0
  savelocal             ; sum = sum + a[i]

  push        1
  inclocal              ; i ++

  push        1
  loadlocal             ; i
  push        0
  loadparam             ; n

  jne         sum0      ; i == n ?

  push        0
  loadlocal             ; sum

  push        1
  saveparam             ; ret = sum

  push        3
  free                  ; free n, i, sum

  ret
