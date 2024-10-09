; 8K ram machine
memory reset
memory ram 0000 1FFF

; Loads a siliconinsider 32K rom that maps in addresses 5xxx, 7xxx to Cxxx, Exxx and Fxxx
memory rom32k rom32/silicrom.rom 5789abcef

; Reboot
cpu reset

; Wait for ROM to kick in
sleep 500

; Go to harware display
type ?

; Goes in fast mode
mode cpu fast
mode display fast

; Done
