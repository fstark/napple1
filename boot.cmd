bind Q quit
bind D "memory dump"
bind T "cpu trace on"
bind Y "cpu trace off"
bind R "cpu reset"
bind F "mode cpu fast"
bind S "mode cpu default"

; 8K ram machine
memory reset
memory ram 0000 2000

; Loads wozmon
memory rom rom/monitor.rom ff00

; Loads basic
memory rom rom/basic.rom e000

; Reboot
cpu reset
