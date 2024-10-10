bind Q quit
bind D "memory dump"
bind T "cpu trace on"
bind Y "cpu trace off"
bind R "cpu reset"
bind F "mode cpu fast"
bind S "mode cpu default"

; 8K ram machine
exec "machines/4+4k.cmd"

; Reboot
cpu reset
