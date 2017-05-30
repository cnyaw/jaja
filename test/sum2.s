;
; test, calc 1 + 2 + ... + 10 = ?
;
; 2012/02/22 Waync Cheng
;

  push        2
  book

  push        10
  push        0
  savelocal

  push        0
  push        1
  savelocal

init0:
  push        0
  loadlocal
  jz          done
  push        0
  loadlocal
  push        1
  loadlocal
  add
  push        1
  savelocal
  push        0
  declocal
  jump        init0

done:
  push        1
  loadlocal

  push        msg
  call        stdio.printf

  push        2
  free

  ret

msg:
  db          "1 + 2 + ... + 10 = %d\n", 0
