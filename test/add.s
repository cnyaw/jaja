;
; test push and add numbers
;
; 2009/08/16 Waync Cheng
;

  push        100
  push        'x'
  add

  push        500
  add

  push        -300
  add

  push        80000
  add

  push        msg
  call        stdio.printf

  ret

msg:
  db          "100 + 'x' + 500 - 300 + 80000 = %d\n", 0
