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
#include "m6502.h"
#include "pia6820.h"
#include "memory.h"
#include "keyboard.h"
#include "screen.h"
#include "msgbuf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"

FILE *autotyping_file = NULL;
const char *autotyping_ptr = NULL;
#define MAX_AUTO_TYPE 2048
char autotyping_buffer[MAX_AUTO_TYPE+1];
int fastCpu = 0;
int fastDsp = 0;

int startAutotypingFile(const char *filename)
{
	if (isAutotyping())
		stopAutotyping();
	autotyping_file = fopen(filename, "r");
	return !!autotyping_file;
}

int startAutotypingBuffer( const char *buffer )
{
	if (isAutotyping())
		stopAutotyping();
	strcpy( autotyping_buffer, buffer );
	autotyping_ptr = autotyping_buffer;
	return 1;
}

int isAutotyping(void)
{
	return autotyping_file || autotyping_ptr;
}

int nextAutotypingBuffer(void)
{
	int c;
	c = *autotyping_ptr++;

	if (c)
	{
		if (c=='\n')
			c = 0x0d;
		if (c>='a' && c<='z')
			c = c - 'a' + 'A';
		keyPressed(c);
	}
	else
		stopAutotyping();
	return c;
}

int nextAutotypingFile(void)
{
	int c;
	
	c = fgetc(autotyping_file);

	if (c!=EOF) {
		if (c=='\n')
			c = 0x0d;
		if (c>='a' && c<='z')
			c = c - 'a' + 'A';
		keyPressed(c);
	}
	else
		stopAutotyping();
	return c;
}

int nextAutotyping(void)
{
	if (autotyping_file)
		return nextAutotypingFile();
	if (autotyping_ptr)
		return nextAutotypingBuffer();
	return 0;
}

void stopAutotyping(void)
{
	if (autotyping_file)
		fclose(autotyping_file);
	autotyping_file = NULL;
	autotyping_ptr = NULL;
}

int handleInput(void)
{
	char tmp;

	tmp = '\0';
	while ( (tmp = getch_screen()) == '\0' )
	{
		printf( "." );
		fflush( stdout );
	}

/*
	Usure how to handle this
	Bound keys being just "shitf" doesn't look like such a great idea...

	if (isBound(tmp))
	{
		console_printf( "Bound key [%c]", tmp );
		const char *command = getBoundCommand(tmp);
		if (command)
			executeCommandString(command);
		return 1;
	}
*/

	if (tmp==0x1b)
	{	// Command mode
		char command[256];
		gets_msgbuf("> ", command);
		executeCommandString(command);
		return 1;
	}

	if (tmp == '\n') {
		tmp = '\r';
	} else if (tmp == '\b') {
		tmp = 0x5f;
	} else if (tmp >= 'a' && tmp <= 'z') {
		tmp = tmp - 'a' + 'A';
	}

	trace_printf( "GOT A KEY: %02X\n", tmp );
	keyPressed(tmp);

	return 1;
}
