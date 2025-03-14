#include "commands.h"
#include "keyboard.h"
#include "memory.h"
#include "m6502.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "rom512.h"
#include "disassemble.h"
#include "symbols.h"

// The command table
// The first element of each command is the command name
// The second element is the function to execute
// The third element is the help string
// The next element is the number of arguments (minus the command name)
typedef struct {
	const char *name;
	int (*function)(int argc, const char **argv);
	const char *help;
} command_t;

int executeType( int argc, const char **argv )
{
	int sync = 0;
	argv++;
	if (!strcmp(*argv,"-sync"))
	{
		sync = 1;
		argv++;
	}
    if (**argv=='@')
    {
        if (!startAutotypingFile( (*argv)+1 ))
            return -1;
    }
    else
        if (!startAutotypingBuffer( *argv ))
            return -1;

	if (sync)
    {
        trace_printf( "Waiting for typing to end\n" );
		while (isAutotyping())
			;
        trace_printf( "Done\n" );
    }
	return 0;
}

int executeQuit( int argc, const char **argv )
{
	exit( 0 );
}

int executeDisplay( int argc, const char **argv )
{
    extern int fastCpu;
    extern int fastDsp;

	argv++;
	if (!strcmp(*argv,"speed"))
	{
        argv++;
		if (!strcmp(*argv,"fast"))
		{
			fastDsp = 1;
			return 0;
		}
		if (!strcmp(*argv,"default"))
		{
			fastDsp = 0;
			return 0;
		}
	}

	return -1;
}


int executeExec( int argc, const char **argv )
{
	if (argc < 2)
		return -1;
	FILE *f = fopen( argv[1], "r" );
	if (!f)
		return -1;
	char command[256];
	while (fgets( command, 256, f ))
	{
        // Remove trailing newline
        char *nl = strchr( command, '\n' );
        if (nl)
            *nl = 0;

        // Execute the command
        trace_printf( "Executing: %s\n", command );
		if (executeCommandString( command ))
		{
			fclose( f );
			return -1;	// Command failed
		}
	}
	fclose( f );
	return 0;		// All commands succeeded
}

int executeCommandFile( const char *filename )
{
    const char *argv[] = { "exec", filename, NULL };
    return executeExec( 2, argv );
}

int executeMemory( int argc, const char **argv )
{
    if (!strcmp(argv[1],"reset"))
    {
        trace_printf( "Memory reset\n" );
        resetMemType();
        return 0;
    }
    if (!strcmp(argv[1],"dump"))
    {
        dumpMem();
        return 0;
    }
    if (!strcmp(argv[1],"ram"))
    {
        int pageStart;
        int pageEnd;
        // PageStart and PageEnd are 4 char hexadecimal strings in argv[2] and argv[3]
        if (sscanf(argv[2],"%x",&pageStart) != 1)
            return -1;
        if (sscanf(argv[3],"%x",&pageEnd) != 1)
            return -1;
        pageEnd++;
        pageStart /= 256;
        pageEnd /= 256;

        trace_printf( "Filling RAM pages %02X to %02X\n", pageStart, pageEnd );
        fillMemType( pageStart, pageEnd - pageStart, MEM_RAM );

        return 0;
    }
    //  #### Should be using "bload" (which should be "memory load")
    if (!strcmp(argv[1],"rom"))
    {
        const char *romfile = argv[2];
        int addr;
        if (sscanf(argv[3],"%x",&addr) != 1)
            return -1;
        
        uint16_t startAdrs = 0;
        uint16_t endAdrs = 0;
        if (loadRom( addr/256, romfile, &startAdrs, &endAdrs )==0)
        {    if (tryLoadSymbolsFor( argv[1], startAdrs, endAdrs )!=-1)
                trace_printf( "(found & loaded a symbol file))\n" );
            return 0;
        }
        return -1;
    }
    if (!strcmp(argv[1],"rom32k"))
    {
        const char *filename = argv[2];
        const char *mappingString = argv[3];

        unsigned short mapping = 0;
        // Loop over the mapping string
        // Each character corresponds to a bit in the mapping
        // 0->bit 0, 1->bit 1, ... 'a' or 'A' -> bit 10, 'b' or 'B' -> bit 11, etc.
        for (int i = 0; i < strlen(mappingString); i++)
        {
            char c = mappingString[i];
            if (c >= '0' && c <= '9')
                mapping |= 1 << (c - '0');
            else if (c >= 'a' && c <= 'f')
                mapping |= 1 << (c - 'a' + 10);
            else if (c >= 'A' && c <= 'F')
                mapping |= 1 << (c - 'A' + 10);
            else
            {
                trace_printf( "Invalid mapping string '%s', must contain '0-9', 'A-F' and 'a-f'\n", mappingString );
                return 0;
            }
        }
        trace_printf( "Mapping: %04X\n", mapping );
        if (loadRom32( filename, mapping ))
            return 0;
        return 0;
    }

    return 0;
}

int executeRom512( int argc, const char **argv )
{
    // rom512 load <file>
    if (!strcmp(argv[1],"load"))
    {
        const char *filename = argv[2];
        if (loadRom512( filename ))
            rom512kpresent = 1;
        else
            rom512kpresent = 0;
        fillMemType( 0x20, 0x80-0x20, MEM_ROM );
        fillMemType( 0x80, 0xa0-0x80, MEM_ROM );
        fillMemType( 0xe0, 0xf0-0xe0, MEM_ROM );
        copyBank512( 0, getMemoryPtr(0) );
        return 0;
    }

    // rom512 release
    if (!strcmp(argv[1],"release"))
    {
        releaseRom512();

        fillMemType( 0x20, 0x80-0x20, MEM_UNALOCATED );
        fillMemType( 0x80, 0xa0-0x80, MEM_UNALOCATED );
        fillMemType( 0xe0, 0xf0-0xe0, MEM_UNALOCATED );

        return 0;
    }

    // rom512 bank <n>
    if (!strcmp(argv[1],"bank"))
    {
        int bank = atoi( argv[2] );
        if (bank<0 || bank>15)
            return -1;

        copyBank512( bank, getMemoryPtr(0) );

        return 0;
    }

    return -1;
}

extern int traceCPU;

int executeTrace( int argc, const char **argv )
{
    return -1;
}

int executeCpu( int argc, const char **argv )
{
    //  start stop and reset
    if (!strcmp(argv[1],"start"))
    {
        startM6502();
        return 0;
    }
    if (!strcmp(argv[1],"stop"))
    {
        stopM6502();
        return 0;
    }
    if (!strcmp(argv[1],"reset"))
    {
        resetM6502();
        return 0;
    }
    if (!strcmp(argv[1],"jump"))
    {
        int adrs;
        sscanf(argv[2],"%x",&adrs);
        setProgramCounter( adrs );
        return 0;
    }

	if (!strcmp(argv[1],"speed"))
	{
        extern int fastCpu;
        extern int fastDsp;

		if (!strcmp(argv[2],"fast"))
		{
			fastCpu = 1;
			return 0;
		}
		if (!strcmp(argv[2],"default"))
		{
			fastCpu = 0;
			return 0;
		}
	}

    if (!strcmp(argv[1],"trace"))
    {
        trace_printf( "Trace command\n" );
        if (!strcmp(argv[2],"on"))
        {
            trace_printf( "Trace on\n" );
            traceCPU = 1;
            return 0;
        }
        if (!strcmp(argv[2],"off"))
        {
            trace_printf( "Trace off\n" );
            traceCPU = 0;
            return 0;
        }
    }  
    return -1;
}

int executeSleep( int argc, const char **argv )
{
    if (argc < 2)
        return -1;
    int ms;
    if (sscanf(argv[1],"%d",&ms) != 1)
        return -1;
    usleep( ms * 1000 );
    return 0;
}

// bsave FILE start lenght
int executeBsave( int argc, const char **argv )
{
    if (argc < 4)
        return -1;
    int start;
    int length;
    if (sscanf(argv[2],"%x",&start) != 1)
        return -1;
    if (sscanf(argv[3],"%x",&length) != 1)
        return -1;
    FILE *f = fopen( argv[1], "wb" );
    if (!f)
        return -1;
    fwrite( getMemoryPtr(start), 1, length, f );
    fclose( f );
    return 0;
}

// bload FILE start
int executeBload( int argc, const char **argv )
{
    if (argc < 3)
        return -1;
    int start;
    if (sscanf(argv[2],"%x",&start) != 1)
        return -1;
    FILE *f = fopen( argv[1], "rb" );
    if (!f)
        return -1;
    fseek( f, 0, SEEK_END );
    int length = ftell( f );
    fseek( f, 0, SEEK_SET );
    fread( getMemoryPtr(start), 1, length, f );
    fclose( f );

    if (tryLoadSymbolsFor( argv[1], start, start+length )!=-1)
        trace_printf( "(found & loaded a symbol file))\n" );

    return 0;
}

const char *keyTable[256];

// bind key command
int executeBind( int argc, const char **argv )
{
    if (argc < 3)
        return -1;
    int key = argv[1][0];
    keyTable[key] = strdup( argv[2] );
    return 0;
}

int isBound( int key )
{
    return keyTable[key] != NULL;
}

const char *getBoundCommand( int key )
{
    return keyTable[key];
}

int uint16FromString( const char *s, uint16_t *num )
{
    int len;

    if (sscanf(s, "%hx%n", num, &len) == 1)
    {
        if (len == strlen(s))
            return 0;
    }

    return -1;
}

int addressFromString( const char *s, uint16_t *adrs )
{
    int len;

    if (sscanf(s, "%hx%n", adrs, &len) == 1)
    {
        if (len == strlen(s))
            return 0;
    }

    if (lookupSymbol( s, adrs )==0)
        return 0;

    trace_printf( "Invalid address: %s -- not a symbol either\n", s );

    return -1;
}

// disa adrs len : disassemble
int executeDisas( int argc, const char **argv )
{
    trace_printf( "Disassembling [%s] [%s]\n", argv[1], argv[2] );
    uint16_t adrs1;
    if (addressFromString( argv[1], &adrs1 )==-1)
        return -1;

    uint16_t adrs2 = adrs1+32;
    uint16_t len;

    int counter;

    if (argv[2])
    {
        if (argv[2][0] == '+')
        {
            if (uint16FromString( argv[2]+1, &len )==-1)
                return -1;
            adrs2 = adrs1 + len;
        }
        else
        {
            if (addressFromString( argv[2], &adrs2 )==-1)
                return -1;
        }
    }

    if (adrs2 < adrs1)
    {
        trace_printf( "Invalid range: %04X to %04X\n", adrs1, adrs2 );
        return -1;
    }

    len = adrs2 - adrs1;
    counter = len; // signed version

    trace_printf( "Disassembling %d bytes from %04X to %04X\n", len, adrs1, adrs2 );

    while (counter>0)
    {
        int instr_len;
        const char *dis = disassemble( adrs1, getMemoryPtr(adrs1), &instr_len );
        trace_printf( "%s\n", dis );
        counter -= instr_len;
        adrs1 += instr_len;
    }

    return 0;
}

//  sym dump name
int executeSym( int argc, const char **argv )
{
    if (!strcmp(argv[1],"list"))
    {
        listSymbolTables();
        return 0;
    }
    if (!strcmp(argv[1],"dump"))
    {
        dumpSymbols( argv[2] );
        return 0;
    }
    if (!strcmp(argv[1],"load"))
    {
        int startAdrs;
        int endAdrs;
        sscanf(argv[3],"%x",&startAdrs);
        sscanf(argv[4],"%x",&endAdrs);

        loadSymbols( argv[2], startAdrs, endAdrs );
        return 0;
    }
    return -1;
}

int executeHelp( int argc, const char **argv );

command_t commands[] = {
	{ "help", executeHelp, "displays list of commands" },
	{ "display", executeDisplay, "display speed [default|fast]" },
	{ "memory", executeMemory, "memory reset (all memory is unallocated)\nmemory ram start end (allocate RAM)\nmemory rom <file> address (loads rom in memory)\nmemory rom32k <file> <jumpers> (load a 32KRAM/ROM image)" },
	{ "type", executeType, "type [-sync] (@<filename>|string) - type the contents of a string or file" },
	{ "exec", executeExec, "exec <file> - execute a command file" },
    { "cpu", executeCpu, "cpu [start|stop|reset|speed fast|speed default|trace on|trace off] - start, stop, reset, speedup or trace the CPU" },
    { "sleep", executeSleep, "sleep <ms> - sleep for a number of milliseconds" },
    { "bsave", executeBsave, "bsave <file> <start> <length> - save memory to a binary file (numbers in hex)" }, // Should be part of memort
    { "bload", executeBload, "bload <file> <start> - load memory from a binary file (numbers in hex)" }, // Should be part of memory
    { "bind", executeBind, "bind <key> <command> - bind a key to a command" },
    { "rom512", executeRom512, "rom512 load <file> - load a 512K ROM image" },  // rom32 should be separate too
    { "disassemble", executeDisas, "disas <address> <length> - disassemble memory" }, // Should be part of memory
    { "symbol", executeSym, "sym load <file>|list|dump <name> - dump symbols" },
	{ "quit", executeQuit, "exit the emulator" },
};

int executeHelp( int argc, const char **argv )
{
	trace_printf( "List of emulator commands:\n" );
	for (int i = 0; i < sizeof(commands)/sizeof(commands[0]); i++)
	{
		trace_printf( "  %s: %s\n", commands[i].name, commands[i].help );
	}
	trace_printf( "\n" );
	return 0;
}

int executeCommand( int argc, const char **argv )
{
    for (int j = 0; j < argc; j++)
        trace_printf( "  %d: '%s'\n", j, argv[j] );

	// Look at each command in the table
    // If only one matches the prefix, execute it
    // If more than one matches, print an error message
    // If none matches, print an error message
    int match = -1;
    for (int i = 0; i < sizeof(commands)/sizeof(commands[0]); i++)
    {
        if (strncmp( argv[0], commands[i].name, strlen(argv[0]) ) == 0)
        {
            if (match != -1)
            {
                trace_printf( "Ambiguous command: '%s'\n    ", argv[0] );
                // print the possibilities
                for (int j = 0; j < sizeof(commands)/sizeof(commands[0]); j++)
                {
                    if (strncmp( argv[0], commands[j].name, strlen(argv[0]) ) == 0)
                        trace_printf( "%s ", commands[j].name );
                }
                trace_printf( "\n" );
                return -1;
            }
            match = i;
        }
    }
    if (match == -1)
    {
        trace_printf( "Unknown command: '%s'\n", argv[0] );
        return -1;
    }  
    trace_printf( "Executing command: '%s' (%s)\n", argv[0], commands[match].name );
    int result = commands[match].function( argc, argv );
    if (result)
    {
        trace_printf( "Command failed: '%s'\n", argv[0] );
        return -1;
    }
	return 0;
}

int executeCommandString( const char *command )
{
	//	Ignore leading whitespace
	while (*command == ' ' || *command == '\t')
		command++;
	
	// 	';' denotes a comment
	if (*command == ';')
		return 0;	//	Successful execution

    //  Empty commands are ignored
    if (!*command)
        return 0;    

	//	Copy command into a new buffer
	char *cmd = strdup( command );

	//	Create an argv-like array of pointers to each word of the copied command.
	//	The array is terminated by a NULL pointer.
	//	The maximum number of arguments is 16.
	const char *argv[16];
    // Clear argv
    for (int i = 0; i < 16; i++)
        argv[i] = NULL;

    int argc = 0;
    char *token = cmd;

    // not nice: we should do the escape handling with the tokenizing
    while (*token)
    {
        // Skip leading whitespace
        while (*token == ' ' || *token == '\t')
            token++;
        if (!*token)
            break;

        // Check is string is quoted
        if (*token == '"')
        {
            // Skip the quote
            token++;
            argv[argc++] = token;
            char *p = token;
            char c;
            // Find the end of the token

            do
            {
                c = *p++;
                if (c=='\\')
                {
                    c = *p++;
                    if (c=='n')
                        c = '\n';
                    if (c==0)
                        p--;
                }
                else
                    if (c=='\"')
                        c = 0;
                *token++ = c;
            }   while (c);
            if (!*p)
                break;
        }
        else
        {
            argv[argc++] = token;
            // Find the end of the token
            while (*token && *token != ' ' && *token != '\t')
                token++;
            if (!*token)
                break;
            *token++ = 0;
        }
    }

    // Handle escape sequences
    // for (int i = 0; i < argc; i++) {
    //     char *src = (char *)argv[i];
    //     char *dst = src;
    //     while (*src) {
    //         if (*src == '\\' && *(src + 1) == 'n') {
    //         *dst++ = '\n';
    //         src += 2;
    //         } else if (*src == '\\' && *(src + 1) == '"') {
    //         *dst++ = '"';
    //         src += 2;
    //         } else {
    //         *dst++ = *src++;
    //         }
    //     }
    //     *dst = '\0';
    // }
	argv[argc] = NULL; // Ensure the array is NULL-terminated

	//	Execute the command
	int result = executeCommand( argc, argv );

	//	Free the copied command
	free( cmd );

	if (result)
		trace_printf( "Command failed: '%s' => %d\n", command, result );

	return result;
}
