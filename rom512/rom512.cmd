; 8K ram machine
memory reset
memory ram 0000 1FFF

; Loads wozmon
memory rom machines/WOZMON ff00
;disas ff00 ffff

; Loads a siliconinsider 512K rom
;rom512 load rom512/sample.rom
rom512 load rom512/512KA1COMPIL.BIN
rom512 bank 0

sym load rom512/WOZDLE.SYM 2000 9FFF

;type -sync "2000R\n"
;sleep 1000
;cpu reset

; type -sync "A002\n"
; type -sync "5000R\n"
;sleep 1000
;cpu reset
