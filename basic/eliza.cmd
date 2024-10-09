; 8K ram machine
memory reset
memory ram 0000 1FFF
memory rom E000 FFFF

; Load applesoft basic in ROM
bload basic/ASOFT.BIN e000

; Launch basic
cpu jump e000

; Goes in fast mode
mode cpu fast
mode display fast

; Wait for basic to start (100ms is too much)
;sleep 100

; Type in the content of the ELIZA.BAS program
type -sync @basic/ELIZA.BAS

; Wait a very little bit
sleep 10

; Dumps the tokenized program
bsave basic/ELIZA.BIN 40 1FC0

; Done. The file ELIZA.BIN contains the tokenized program
quit
