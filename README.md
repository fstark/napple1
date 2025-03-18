# FReD's version of napple1

This is a fork of napple1 that focuses on making napple1 a tool to simulate real world hardware for helping in creating ROMs for the machine and/or reviving/patching old software.

Originally, napple1 is a fork of pom1, to curses.

## Features not present upstream

* Accurate unmapped memory access (read as page#)
* Correct emulation of video (wait for DSP) that prevent the apple1loader to detect that the machine has successfully booted.
* Support for several Apple1 configurations
* Support for SiliconInsider RAM/ROM card emulation
* Support for accelerated execution
* Support for tracing with integrated disassembler
* Memory dump
* Command mode (access with ESC, used 'help' to get the list of commands)
* Batch commands (script files)
* Configuration files (init boot script)
* Key bindings (in boot.cmd file)

## Features removed from upstream

* Autotyping is replaced by a the ``type`` command
* Curses has been removed so we can use this in more automated settings

## Forthcoming

* Support for SiliconInsider 512K rom card with bank switching
* Auto-reload of ROMs when the are changed (``watch <file>`` command?)
* Breakpoints (``break`` command. With attached commands?). Data breakpoints?
* Support for symbolic debugging (``symbol`` command)

## Maybe?

* Extend core format for additional configurations (usure if useful)
* Automated tokenized basic programs? via some binding?

Go in [the samples subdirectory](samples/README.md) for more information on how to use the scripts to start the Apple1 with different configurations.

Go in [the basic subdirectory](basic/README.md) for more information on how to use the scripts to convert a text file with a BASIC program into a binary file that can be loaded into the Apple1.

Go in [the rom32 subdirectory](rom32/README.md) for more information on how to emulate a Silicon Insider RAM/ROM card.

## BUGS

There is a freeze sometimes, unsure what can trigger it
Error tracing is pretty bad
Errors don't go back to the UI (need the trace file)
Commands don't check their arguments
Hexadecimal numbers are case-sensitive
