/* napple1 Apple 1 emulator
 * Copyright (C) 2008 Nobu Hatano
 *
 * Pom1 Apple 1 Emulator
 * Copyright (C) 2000 Verhille Arnaud
 * Copyright (C) 2006 John D. Corrado
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include <stdlib.h>

#include "m6502.h"
#include "memory.h"
#include "keyboard.h"
#include "screen.h"
#include "msgbuf.h"
#include "commands.h"
#include <string.h>
#include <stdio.h>

int main( int argc, const char **argv )
{
	/* initialize ncurse */
	// noecho();
	// cbreak();
	init_screen();
	init_msgbuf();

	/* initialize apple1 screen */
	resetScreen();
	setSpeed(1000000, 50); /* 1M Hz. Sync emulation every 50 msec */

	/* start processor */
	resetM6502();
	startM6502();
	atexit(stopM6502);

	executeCommandFile( "boot.cmd" );

	if (argc>1)
	{
		if (!strcmp(argv[1], "-c"))
		{
			if (argc>2)
				executeCommand( argc-2, argv+2 );
		}
		else if (executeCommandFile(argv[1]))
		{
			fprintf( stderr, "Failed to execute file\n" );
			exit(1);
		}
	}

	while (handleInput());

	/* ending */
	return 0;
}
