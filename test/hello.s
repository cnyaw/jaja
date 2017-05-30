;
; export a function call
;
; 2009/08/14 Waync Cheng
;

export hello, msg

  jump        hello

msg:
  db          "Hello jaja!\n", 0

hello:
  push        msg
  call        stdio.printf
  ret
