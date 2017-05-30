;
; sample game: Tic Tac Toe
;
; 2009/08/14 Waync Cheng
;

  jump        main

msg:
  DB          "Sample Tic Tac Toe\n", 0

main:
  push        msg
  call        system.outstr

start:
  push        12
  book                  ; a[9], count, turn, c

  push        0
  local

  call        init      ; init a[0..8] = '.'

  push        0
  push        9
  savelocal             ; count = 0

  push        0
  push        10
  savelocal             ; turn = 0

  push        10
  push        11
  savelocal             ; c = 10, mark

loop:
  push        0
  local
  call        disp      ; display game board

  push        11
  loadlocal
  push        0
  local
  call        check

  jnz         loopEnd   ; X or O win?

  push        9
  loadlocal
  push        9
  jne         input     ; count == 9 ?

loopEnd:
  push        12
  free
  ret

input:
  push        msg2
  call        system.outstr

  call        system.readint

  push        11
  savelocal             ; cin >> c

  push        11
  loadlocal
  push        0
  jl          input

  push        11
  loadlocal
  push        9
  jge         input

  push        11
  loadlocal
  push        0
  local
  add
  dup

  load4
  push        '.'
  je          update    ; a[c] == '.' ?

  discard
  jump        input

update:
  push        10
  loadlocal
  jz          updateX

  push        'O'
  jump        updateBrd

updateX:
  push        'X'

updateBrd:
  swap
  save4                 ; a[c] == 'X' or 'O'

  push        9
  inclocal              ; count ++

  push        10
  loadlocal
  inc
  push        2
  mod
  push        10
  savelocal             ; turn %= 2

  jump        loop

msg2:
  DB          "Select (0..8) to Put: ", 0

;
; check is game end
; bool check(a[9], c);
;

check:
  push        2
  discardx
  push        0
  ret

;
; init game board
; void init(a[9]);
;

init:
  push        1
  book                  ; i

  push        0
  push        0
  savelocal             ; i = 0

init0:
  push        '.'
  push        0
  loadparam
  save4                 ; *a = a[i] = '.'

  push        0
  incparam              ; a ++
  push        0
  inclocal              ; i ++

init1:
    push      9
    push      0
    loadlocal
    jne       init0

    push      2
    free                ; a, i
    ret

;
; display game board
; void disp(a[9]);
;

disp:
  push        1
  book                  ; i

  push        0
  push        0
  savelocal             ; i = 0

disp0:
  push        '\t'
  call        system.outchr

  push        0
  loadparam
  load4
  call        system.outchr

  push        0
  incparam              ; i ++
  push        0
  inclocal              ; i ++

  push        0
  loadlocal
  push        3
  mod
  jnz         disp1     ; 0 == (i % 3)?

  push        '\n'
  call        system.outchr

disp1:
  push        9
  push        0
  loadlocal
  jne         disp0

  push        2
  free                  ; a, i
  ret
