; 8K ram machine
memory reset
memory ram 0000 2000

; Loads wozmon
memory rom rom/monitor.rom ff00

; Loads basic
memory rom rom/basic.rom e000

; Reboot
cpu reset

; Launch basic
type -sync "E000R\n"

type -sync "10 PRINT \"Hello, world! \"; : GOTO 10\nRUN"
