;
; test, calc 1 + 2 + ... + 10 = ?
;
; 2012/02/22 Waync Cheng
;

  jump        init0

i:            db 10
sum:          db 0

init0:
  push        i                         ; 0 == i?
  load1
  jz          done

  push        sum                       ; sum += i
  load1
  push        i
  load1
  add
  push        sum
  save1

  push        i                         ; i -= 1
  load1
  dec
  push        i
  save1

  jump        init0

done:
  push        sum
  load1
  push        msg
  call        stdio.printf

  ret

msg:
  db          "1 + 2 + ... + 10 = %d\n", 0
