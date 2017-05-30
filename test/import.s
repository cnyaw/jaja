;
; call external mod call
;
; 2009/08/14 Waync Cheng
;

  call        hello.hello

  push        hello.msg
  call        system.outstr

  ret
