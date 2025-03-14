bind A "type -sync \"2000R\\\n\""

; 8K ram machine
memory reset
memory ram 0000 1FFF

memory rom rom/monitor.rom ff00

; Loads a siliconinsider 32K rom that maps in addresses 2000-9fff, with mirror at e000-efff
;memory rom32k rom32/32KA1COMPIL.BIN 23456789e
memory rom32k rom32/WOZDLE.BIN 23456789

; Reboot
cpu reset

; Go to hardware display
type "2000R\n?"

; Wait for ROM to kick in
sleep 500

; Goes in fast mode
mode cpu fast
mode display fast

sleep 200

; Default perf
mode cpu default
mode display default

