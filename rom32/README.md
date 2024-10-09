# Examples of using a file for Silicon Insider RAM/ROM card emulation

The source and tools to build the rom can be found in [the apple1loader github repository](https://github.com/fstark/apple1loader).

## Usage

Launch ``napple1`` and either:

``ESC + 'exec rom32/rom32.cmd'``
Configure the machine, loads the rom and executes the hardware info module.

Use a ``tail -f trace.txt`` to check for errors.
