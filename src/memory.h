/* Pom1 Apple 1 Emulator
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

#include <stdint.h>

#define FNAME_LEN_MAX   1024

typedef enum {
    MEM_RAM,
    MEM_ROM,
    MEM_UNALOCATED
} eMemType;

extern int rom512kpresent;

void resetMemory(void);
void resetMemType();
void fillMemType( unsigned char start, int len, eMemType type );
int loadRom( unsigned char startPage, const char *romfile, uint16_t *startAdrs, uint16_t *endAdrs );
int loadRom32( const char *rom32, uint16_t config );
void dumpMem( void );



uint8_t *getMemoryPtr( uint16_t address );
unsigned char memPeek(unsigned short address);
unsigned char memRead(unsigned short address);
void memWrite(unsigned short address, unsigned char value);
void dumpCore( const char *filename );
int loadCore(void);
eMemType memType(unsigned char page);
void setRomFiles(void);

// move me
int trace_printf( const char *format, ... );
int console_printf( const char *format, ... );
void trace_tid();

void ansi_red();
void ansi_green();
void ansi_black();

