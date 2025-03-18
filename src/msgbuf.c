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
/*
 * Message buffer is a 1 line display of napple 1 system. 
 */
#include <string.h>
#include <stdio.h>

#include "screen.h"
#include "msgbuf.h"
#include "memory.h"

void print_msgbuf(char *s)
{
	char msg[MSG_LEN_MAX + 1]; 

	if ((int)strlen(s) < 1)
		sprintf(msg, "ESC Trace Reset Quit");
	else
		sprintf(msg, 
			"%-40s", 
			s);
	printf( "INFO: %s", msg);
}

void init_msgbuf(void)
{
}

void gets_msgbuf(char *prompt, char *typed)
{
	print_msgbuf( prompt );
	scanf("%256s", typed );
}
	
