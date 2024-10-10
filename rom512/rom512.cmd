; 8K ram machine
memory reset
memory ram 0000 1FFF

; Loads wozmon
memory rom rom/monitor.rom ff00

; Loads a siliconinsider 512K rom
rom512 load rom512/sample.rom
rom512 bank 0

type -sync "5000R\n"
sleep 1000
cpu reset

type -sync "A002\n"
type -sync "5000R\n"
sleep 1000
cpu reset
