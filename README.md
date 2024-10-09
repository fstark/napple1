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
* Integrated dissasembler
* Memory dump

## TODO

* Configuration files (init boot script)
* Extend core format for additional configurations (usure if useful)
* Auto-reload of ROMs when the are changed (unsure if useful, maybe a watch file command would be better)
* Support for 512K rom card with bank switching
* Breakpoints
* Support symbolic debugging
* Curses removal ?
