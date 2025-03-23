bind A "type -sync \"2000R\\\n\""

; 8K ram machine
memory reset
memory ram 0000 1FFF

memory rom rom/monitor.rom ff00

; Loads a siliconinsider 32K rom that maps in addresses 2000-9fff, with mirror at e000-efff
mem rom32k rom32/silicrom.rom 23456789e
; memory rom32k rom32/WOZDLE.BIN 23456789

; Reboot
cpu reset

; Go to hardware display
type "2000R\n?"

; Wait for ROM to kick in
; (ROM will not start if speed is not default
; due to test for proper hardware reset for real machines)
sleep 500

; Goes in fast mode
cpu speed fast
display speed fast

sleep 200

; Default perf
cpu speed default
display speed default

; Return to monitor
cpu reset
