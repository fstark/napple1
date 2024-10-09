; 8K ram machine
memory reset
memory ram 0000 1FFF

; Loads a siliconinsider 32K rom that maps in addresses 5xxx, 7xxx to Cxxx, Exxx and Fxxx
memory rom32k eliza.rom 5789abcef

; Reboot
cpu reset

; Wait for ROM to kick in
sleep 50

; Goes in fast mode
mode cpu fast
mode display fast

; Go to basic
type B

; Wait for basic to start (100ms is too much)
sleep 100

; Load the ELIZA.BAS program
type -sync @E.BAS

; Wait a very little bit
sleep 10

; Dumps the tokenized program
bsave ELIZA 40 1FC0

; Done

mode cpu default
mode display default
cpu stop

;quit
