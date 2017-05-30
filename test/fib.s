;
; calc Fibonacci Number 15
;
; 2009/08/14 Waync Cheng
;

  jump        start

msg:
  db          "calc fib(15) = %d\n", 0

start:
  push        15
  call        fib

  push        msg
  call        stdio.printf

  ret

fib:

  push        0
  loadparam
  push        0
  jg          fib0      ; n <= 0 return 0

  push        0
  jump        fibE

fib0:
  push        0
  loadparam
  push        1
  jg          fib1      ; n == 1 return 1

  push        1
  jump        fibE

fib1:
  push        0
  loadparam
  dec

  dup
  call        fib       ; fib(n - 1)

  swap
  dec
  call        fib       ; fib(n - 2)

  add                   ; return fib(n-2) + fib(n-1)

fibE:
  push        0
  saveparam
  ret
