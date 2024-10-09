# Basic programs manipulation

ASOFT.BIN is a patched Applesoft BASIC that stored tokenized program starting at $300 (and have 3328 more bytes available).

Using the scripts in this directory, you can convert a text file with a BASIC program into a binary file that can be loaded into the Apple1.

## Usage

Launch ``napple1`` and either:

``ESC + 'exec basic/eliza.cmd'``
Configure the machine, loads the basic, runs it, loads ELIZA and saves the tokenized version.

``ESC + 'exec basic/eliza2.cmd'``
Configure the machine, loads the basic, the tokenized version of ELIZA created in the above step, and runs it.

Use a ``tail -f trace.txt`` to check for errors.
