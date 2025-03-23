memory reset
memory ram 0000 2000

; Loads wozmon
memory rom rom/monitor.rom ff00

; Loads basic
memory rom rom/basic.rom e000

; Loads mandelbrot
bload ../mandelbrot65/mandelbrot65.o65 0280

; Starts mandelbrot checksum
type "280R\n"
