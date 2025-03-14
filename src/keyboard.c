/* napple1 ncurses Apple 1 emulator
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
#include "m6502.h"
#include "pia6820.h"
#include "memory.h"
#include "keyboard.h"
#include "screen.h"

#include <stdio.h>
#include <stdlib.h>

FILE *autotyping_file = NULL;
int fastMode = 0;

int startAutotyping(const char *filename)
{
	if (autotyping_file)
		stopAutotyping();
	autotyping_file = fopen(filename, "r");
	return !!autotyping_file;
}

void autoCommands(void)
{
	int c;
	while ((c = fgetc(autotyping_file))!=EOF)
	{
		switch (c)
		{
			case 'F':
				fastMode = !fastMode;
				break;
			case 'B':
				loadBasic(1);
				resetPia6820();
				resetM6502();
				break;
			case 'D':
				char filename[256];
				fscanf(autotyping_file, "%s", filename);
				dumpCore( filename );
				resetPia6820();
				resetM6502();
				break;
			case 'M':
				flipMode();
				resetPia6820();
				resetM6502();
				break;
			case 'Q':
				exit( 0 ); /* Ugly */
			case '\n':
				return;
		}
	}
}

int nextAutotyping(void)
{
	if (!autotyping_file)
		return -1;
	int c;
	do
	{
		c = fgetc(autotyping_file);
		if (c=='\t')
			autoCommands();
	} while (c=='\t');

	if (c!=EOF) {
		if (c=='\n')
			c = 0x0d;
		if (c>='a' && c<='z')
			c = c - 'a' + 'A';
		writeKbd((unsigned char)(c + 0x80));
		writeKbdCr(0xA7); 
	}
	else
		stopAutotyping();
	return c;
}

void stopAutotyping(void)
{
	if (autotyping_file)
		fclose(autotyping_file);
	autotyping_file = NULL;
}

int handleInput(void)
{
	char tmp;

	tmp = '\0';
	while ( (tmp = getch_screen()) == '\0' )
		;
	if (tmp == 'F')
	{
		fastMode = !fastMode;
		return 1;
	}
	else if (tmp=='K') {
		if (startAutotyping("AUTOTYPING.TXT"))
			return 1;
	}
	else if (tmp == 'B') {
		loadBasic(0);
		resetPia6820();
		resetM6502();
		return 1;
	} else if (tmp == 'L') {
		loadCore();
		resetPia6820();
		resetM6502();
		return 1;
	} else if (tmp == 'D') {
		dumpCore( NULL );
		resetPia6820();
		resetM6502();
		return 1;
	} else if (tmp == 'R') {
		resetPia6820();
		resetM6502();
		return 1;
	} else if (tmp == 'H') {
 		resetScreen();
		resetPia6820();
		resetMemory();
		resetM6502();
		return 1;
	} else if (tmp == 'Q') {
		return 0;
	} else if (tmp == 'M') {
		flipMode();
		resetPia6820();
		resetM6502();
		return 1;
	} else if (tmp == '\n') {
		tmp = '\r';
	} else if (tmp == '\b') {
		tmp = 0x5f;
	} else if (tmp >= 'a' && tmp <= 'z') {
		tmp = tmp - 'a' + 'A';
	}

	writeKbd((unsigned char)(tmp + 0x80));
	writeKbdCr(0xA7); 

	return 1;
}
