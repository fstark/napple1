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
/*
 * Memory maap of napple1
 * 
 * Apple I 8K mode & napple1 64K mode, common usage
 * -----------------------------------------------------------------------
 * $0000           Ststem & User Space 
 * $0200 - $027F   Input Buffer used by monitor
 * $0FFF
 * -----------------------------------------------------------------------
 * $D010	KBD	Keyboard input register. b7 is always 1 by hardware.
 * 		Read KBD will automatcically clear KBDCR's b7.
 * $D011	KBDCR	When key is pressed, b7 is set by hardware.
 * $D012	DSP	Bit b6..b0 is output character for the terminal.
 *	        Writing to DSP will set b7 by hardware.
 *              The termianl clear b7 after the character is accepted.
 * $D013	DSPCR	Unused.
 * -----------------------------------------------------------------------
 * $E000        Apple I Integer BASIC
 * $E2B3        Re-entry address
 * $EFFF
 * -----------------------------------------------------------------------
 * $FF00        Monitor
 * $FFEF	Echo
 * $FFFF	
 * ----------------------------------------------------------------------- 
 */
/* Apple I 8K mode memory map
 * --------------------------------- 
 * Start Type
 * addr
 * --------------------------------- 
 * $0000 4KB RAM
 * $1000 unused
 * $D010 Display and Keyboard I/O
 * $D014 unused
 * $E000 4KB RAM
 * $F000 unused
 * $FF00 256B ROM^ (Woz Monitor)
 * ---------------------------------
 * ^ ROM can be written by Load core  
 */
/* napple I 32K mode memory map
 * --------------------------------- 
 * Start Type
 * addr
 * --------------------------------- 
 * $0000 32K RAM
 * $8000 unused
 * $D010 Display and Keyboard I/O
 * $D014 unused
 * $E000 8K ROM^ 
 * ---------------------------------
 * ^ ROM can be written by Load core  
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include "pia6820.h"
#include "memory.h"
#include "screen.h"
#include "msgbuf.h"
#include "keyboard.h"

#define MEMMAX 0xFFFF
#define FNAME_LEN_MAX 1024

static unsigned char mem[65536];

static eMode mode = APPLE1_8K; /* 8 = Apple I 8K mode, 32 = napple1 32K mode */

char rombasic[FNAME_LEN_MAX];
char rommonitor[FNAME_LEN_MAX];

eMemType s_memType[256];	/* Type of all memory pages */

void memFill( uint8_t page, uint8_t value )
{
	int i;
	for (i = 0; i < 256; i++)
	{
		mem[page*256+i] = value;
	}
}

void setMemType( unsigned char page, eMemType type )
{
	unsigned char v = 0xaa;
	switch (type)
	{
		case MEM_RAM:
			v = 0x00;
			break;
		case MEM_ROM:
			v = 0xff;
			break;
		case MEM_UNALOCATED:
			v = page;	/* Apple1 unallocated space returns MSB */
			break;
	}

	s_memType[page] = type;
	memFill( page, v );
}

void fillMemType( unsigned char start, int len, eMemType type )
{
	int i;

	for (i = 0; i < len; i++)
		setMemType( start+i, type );
}

void resetMemType()
{
	fillMemType( 0, 256, MEM_UNALOCATED );
}

int loadRom( unsigned char startPage, const char *romfile, long start, long len )
{
	// printf( "loadRom: %s $%02x %ld %ld\n", romfile, startPage, start, len );
	FILE *fd = fopen(romfile, "rb");
	int i;

	if (!fd) {
		trace_printf(stderr, "Failed to open %s\n", romfile);
		perror("fopen");
		return -1;
	}

	if (!len)
	{
		fseek(fd, 0, SEEK_END);
		if ((len=ftell(fd))==-1) {
			fprintf(stderr, "Failed to size: %s\n", romfile);
			fclose(fd);
			return -1;
		}
	}

	fseek(fd, start, SEEK_SET);

	if ((len%256)!=0)
	{
		fprintf(stderr, "ROM size is not multiple of 256: %s\n", romfile);
		fclose(fd);
		return -1;
	}

	/* Read each page of the rom file */
	for (i = startPage; i < startPage + len/256; i++)
	{
		if (i*256>0xFFFF)
		{
			fprintf(stderr, "ROM file too large: %s\n", romfile);
			fclose(fd);
			return -1;
		}
		fillMemType( i, 1, MEM_ROM );
		fread(&mem[i*256], 1, 256, fd);
	}

	fclose(fd);

	return 0;
}

static FILE *trace_file;

// a variant of fprintf that prints in the file "trace_file"
int trace_printf( const char *format, ... )
{
	va_list args;
	int res;

	if (!trace_file)
	{
		// open and truncate the file
		trace_file = fopen( "trace.txt", "w" );
	}

	va_start( args, format );
	res = vfprintf( trace_file, format, args );
	va_end( args );

	fflush( trace_file );

	return res;
}

#include <pthread.h>

// print thread id
void trace_tid()
{
	trace_printf( "Thread id: %d\n", pthread_self() );
}

void dumpMem( void )
{
	//	 Loop over all memory pages and print the type
	for (int i=0;i!=256;i++)
	{
		trace_printf( "%02x: %s\n", i, s_memType[i]==MEM_RAM ? "RAM" : s_memType[i]==MEM_ROM ? "ROM" : "UNALLOCATED" );

		// print content of page in hex, address followed by 16 bytes per line
		trace_printf( "type: %d\n", s_memType[i] );
		if (s_memType[i]==MEM_RAM || s_memType[i]==MEM_ROM)
			for (int j=0;j!=256;j+=16)
			{
				trace_printf( "%04x: ", i*256+j );
				for (int k=0;k!=16;k++)
				{
					trace_printf( "%02x ", mem[i*256+j+k] );
				}
				trace_printf( "\n" );
			}
	}
}

int loadRom32Page( const char *rom32, int page4k )
{
	int offset = page4k;
	if (offset>=32768)
		offset -= 32768;
	trace_printf( "loadRom32Page: %s $%04x %d\n", rom32, page4k, offset );
	return loadRom( page4k/256, rom32, offset, 4096 );
}

/* Loads a 32K rom according to a map configuration */
int loadRom32( const char *rom32, uint16_t config )
{
	for (int i=0;i!=16;i++)
	{
		if (config & (1<<i))
			if (loadRom32Page( rom32, i*4096 ))
				return -1;
	}

	return 0;
}

void flipMode(void)
{
	int newMode = mode + 1;
	if (newMode == LAST_MODE)
		newMode = APPLE1_4K;

	setMode( newMode );	

	/* update message buffer */
	print_msgbuf("");
}

void setMode( eMode aMode )
{
	if (mode < LAST_MODE)
	{
		mode = aMode;
		resetMemType();
		switch (mode)
		{
			case APPLE1_4K:
				fillMemType( 0, 16, MEM_RAM );
				loadRom( 0xFF, rommonitor, 0, 0 );
				break;
			case APPLE1_8K:
				fillMemType( 0, 32, MEM_RAM );
				loadRom( 0xFF, rommonitor, 0, 0 );
				break;
			case APPLE1_4_4K:
				fillMemType( 0, 16, MEM_RAM );
				fillMemType( 0xE0, 16, MEM_RAM );
				loadRom( 0xFF, rommonitor, 0, 0 );
				break;
			case APPLE1_32K:
				fillMemType( 0, 128, MEM_RAM );
				loadRom( 0xFF, rommonitor, 0, 0 );
				break;
			case CUSTOM:
				fillMemType( 0, 32, MEM_RAM );
				// .... .5.7 89ab c.ef
				// 0000 0101 1111 1011
				// 1101 1111 1010 0000
				// DFA0
				// loadRom32( "silicrom.rom", 0xdfa0 );
				loadRom32( "eliza.rom", 0xdfe0 );
				break;
			default:
				break;
		}
	}
}

eMode memMode(void) 
{
	return mode;
}

void loadBasic(int silent)
{
	FILE *fd = fopen(rombasic, "rb");
	char input[MSG_LEN_MAX +1];
	
	if (!fd) {
		gets_msgbuf("Failed to open basic.rom", input);
		return;
	}

	if (silent)
		input[0] = 'y';
	else
		gets_msgbuf("Load basic.rom to ram? y/n: ", input);
	if (input[0] == 'y') {
		size_t s = fread(&mem[0xE000], 1, 4096, fd);
		if (!silent) {
			if (s) {
				gets_msgbuf("Load completed: ", input);
			} else {
				gets_msgbuf("Load failed: ", input);
			}
		}
	}

	fclose(fd);
	return;
}

const char *modeName()
{
	static const char *modes[] =
	{
		"4K", "8K", "4+4", "32K"
	};
	const char *res = "???";
	if (mode<sizeof(modes)/sizeof(modes[0]))
		res = modes[mode];
	return res;
}

int loadMonitor(void)
{
	FILE *fd = fopen(rommonitor, "rb");

	if (fd) {
		fread(&mem[0xFF00], 1, 256, fd);
		fclose(fd);
	}
	else{
		return 0;
	}

	return 1;
}

void resetMemory(void)
{
	/* #### Need to have a ROM mode */
	memset(mem, 0, 0x10000 - 256); /* rom is within tail 256b */
}

uint8_t *getMemoryPtr( uint16_t address )
{
	return mem+address;
}

unsigned char memPeek( unsigned short address )
{
	return mem[address];
}

unsigned char memRead(unsigned short address)
{
	if (address == 0xD010)
	{
		unsigned char c = readKbd();
		// trace_printf( "KEYBOARD: READ %02x '%c'\n", c, c&0x7f );
		writeKbdCr( 0x27 );
// 		if (!(readKbdCr()&0x80))
// \			trace_printf( "KEYBOARD: cleared\n" );
		return c;
	}
	if (address == 0xD011) {
		unsigned char v = readKbdCr();
		// if (v&0x80)
		// 	trace_printf( "KEYBOARD: key pressed\n" );
		if (!(v & 0x80))
			nextAutotyping();
		return v;
	}
	if (address == 0xD012)
		return readDsp();
	if (address == 0xD013)
		return readDspCr();

	return mem[address];
}

int isRam(unsigned short address)
{
	return s_memType[address/256] == MEM_RAM;
}

int isRom(unsigned short address)
{
	return s_memType[address/256] == MEM_ROM;
}

void memWrite(unsigned short address, unsigned char value)
{
	if (isRam(address))
		mem[address] = value;
	else if (address == 0xD013)
		writeDspCr(value);
	else if (address == 0xD012)
		writeDsp(value);
	// no write to KBD

	return;
}

void dumpCore( const char *filename )
{
	int i;
	FILE *fd;
	char input[MSG_LEN_MAX +1];
	char corename[5 + MSG_LEN_MAX +1]; /* 'core/' + input string */

	if (filename) {
		strcpy(input, filename);
	} else {
		gets_msgbuf("Dump core. Filename: ", input);
	}

	sprintf(corename, "core/%s", input);

	fd = fopen(corename, "w");
	for (i = 0; i <= MEMMAX; i++)
		fputc(mem[i], fd);
	fclose(fd);
	if (!filename)
		gets_msgbuf("Dump core completed: ", input);
}

int loadCore(void)
{
	FILE *fd;
	char input[MSG_LEN_MAX +1];
	char corename[5 + MSG_LEN_MAX +1]; /* 'core/' + input string */
	size_t s = 0;
	unsigned char buf[65536];
	int i;

	gets_msgbuf("Load core. Filename: ", input);
	sprintf(corename, "core/%s", input);

	fd = fopen(corename, "r");
	if (fd) {
		s = fread(&buf[0], 1, MEMMAX+1, fd);
		fclose(fd);
	}
	if (s) { 
		gets_msgbuf("Load core completed: ", input);
	} else {
		gets_msgbuf("Failed to open core file: ", input);
		return 0;
	}

	/* 0xF000 is unused area of 8K mode or
	 * ROM area of 32K mode. So,  if 0xF000 has a value,
	 * The mode should better change to 32K mode.
	 */
	if ((buf[0xF000] != 0) && (memMode() == 8)) {
		setMode( APPLE1_32K );
	}

	for (i = 0; i <= 0xFFFF; i++)
		if (isRam(i) || isRom(i))
			mem[i] = buf[i];
	return 1;
}

/* set ROM file name using ROMDIR env variable
 * default path is ./rom 
 * need to be called before loadBasic() and loadMonitor()
 */
void setRomFiles(void)
{
    char env[FNAME_LEN_MAX];
    char *p;

    strcpy(rombasic, "rom/basic.rom");
    strcpy(rommonitor, "rom/monitor.rom");

    p = env;
    if (getenv("ROMDIR")) {
        p = getenv("ROMDIR");
        sprintf(rombasic, "%s/basic.rom", p);
        sprintf(rommonitor, "%s/monitor.rom", p);
    } 
}


