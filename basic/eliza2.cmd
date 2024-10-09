; 8K ram machine
memory reset
memory ram 0000 1FFF
memory rom E000 FFFF

; Load applesoft basic in ROM
bload basic/ASOFT.BIN e000

; Load ELIZA
bload basic/ELIZA.BIN 40 1FC0

; Warm launch basic
cpu jump e003

; Run the basic program
type -sync "RUN\n"

; Satrt taling
type -sync "This is success!\n"

type -sync "Sorry I wasn't listening\n"
