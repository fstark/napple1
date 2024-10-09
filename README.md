# FReD's version of napple1

This is a fork of napple1 that focuses on making napple1 more useful to simulate real world hardware and helps in creating ROMs for the machine.

Originally, napple1 is a fork of pom1, to curses.

## Features not present upstream

* Support for several Apple1 configurations
* Support for SiliconInsider RAM/ROM card emulation
* Support for tracing
* Support for accelerated execution
* Accurate unmapped memory access (read as page#)
* Automatic key file selection
* Macros in automatic key file (will be removed)
* Correct emulation of video (wait for DSP) that prevent the apple1loader to detect that the machine has successfully booted.
* Extraction of tokenized basic programs.
* Use ESC key to go in command mode
* Batch commands
* Integrated dissassembler
* Memory dump
* Configuration files (init boot script)
* Replaced menu by key bindings (in boot.cmd file)

## TODO

* Extend core format for additional configurations (usure if useful)
* Auto-reload of ROMs when the are changed (unsure if useful, maybe a watch file command would be better)
* Support for 512K rom card with bank switching
* Breakpoints
* Support symbolic debugging
* Curses removal ?

Go in [the samples subdirectory](samples/README.md) for more information on how to use the scripts to start the Apple1 with different configurations.

Go in [the basic subdirectory](basic/README.md) for more information on how to use the scripts to convert a text file with a BASIC program into a binary file that can be loaded into the Apple1.

Go in [the rom32 subdirectory](rom32/README.md) for more information on how to emulate a Silicon Insider RAM/ROM card.

## BUGS

There is a freeze sometimes, unsure what can trigger it
Error tracing is pretty bad
Errors don't go back to the UI (need the trace file)
Commands don't check their arguments
Hexadecimal numbers are case-sensitive
