;
; calling conversion
;
; 2012/03/27 Waync Cheng
;

  push        0                         ; last param first push
  push        1
  push        2
  push        3
  call        aaa                       ; aaa(3, 2, 1, 0)

  ret

;
; void aaa(a, b, c, d)
;
; | 3 | a
; | 2 | b
; | 1 | c
; | 0 | d
; +---+
;

aaa:
  push        0                         ; a
  loadparam
  call        system.outint             ; 3

  push        1                         ; b
  loadparam
  call        system.outint             ; 2

  push        2                         ; c
  loadparam
  call        system.outint             ; 1

  push        3                         ; d
  loadparam
  call        system.outint             ; 0

  ret
