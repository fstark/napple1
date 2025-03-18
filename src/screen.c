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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, 
 * USA.
 */
/** screen.c
 * Screen is a vitrual display of Apple I 
 */
#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#include "screen.h"
#include "memory.h"
#include <stdio.h>

#include <termios.h>

int x = 0;

struct termios oldt, newt;

void raw_mode( int raw )
{
	if (raw)
		tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	else
		tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

void init_screen(void)
{
	x = 0;
	printf( "\n" );

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);

	raw_mode( 1 );
	
	// atexit((void (*)(void)) {
	// 	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	// });
}

void resetScreen(void)
{
	for (int i=0;i!=24;i++)
		printf( "\n" );
}

void outputDsp(unsigned char dsp)
{
	// console_printf("DISPLAY: %02X '%c'\n", dsp, dsp);

	switch (dsp)
	{
		case 0x0A:
		case 0x0D:
			printf( "\n" );
			x = 0;
			break;
		case 0x00:
			break;
		case 0x7F:
			printf( "_" );
			x++;
			fflush( stdout );
			break;
		default:
			printf( "%c", dsp );
			x++;
			fflush( stdout );
			break;
	}
	if (x==40)
	{
		printf( "\n" );
		x = 0;
	}
}

char getch_screen(void)
{
	raw_mode( 1 );
	//	#### Need to hack for canonical mode, etc
	int c = getchar();

	raw_mode( 0 );

	// printf( "[%02x]", c );
	// fflush( stdout );
	
	if (c==0x0a)
		c = 0x0d;
	return (char)c;
}
