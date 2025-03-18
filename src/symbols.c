#include "symbols.h"

#include <stdlib.h>
#include <stdio.h>
#include "memory.h"
#include <string.h>

typedef struct symbolTable_t
{
    struct symbolTable_t *next;        // Next symbol table in the list
    char name[256];             // Name of the symbol table
    uint16_t startAdrs;         // Range of addresses
    uint16_t endAdrs;
    const char *globals[65536]; // Visible to all code
    const char *locals[65536];  // Only visible to code between startAdrs and endAdrs
}   symbolTable_t;

// symbolTable_t wozmonSymbols = {
//     0xff00, 0xFFFF,
//     {
//         [0xd010] = "KBD",
//         [0xd011] = "KBDCR",
//         [0xd012] = "DSP",
//         [0xd013] = "DSPCR",
//         [0xffef] = "ECHO",
//     },
//     {
//         [0x24] = "XAML",
//         [0x25] = "XAMH",
//         [0x26] = "STL",
//         [0x27] = "STH",
//         [0x28] = "L",
//         [0x29] = "H",
//         [0x2a] = "YSAV",
//         [0x2b] = "MODE",

//         [0x200] = "IN"
//     }
// };

int loadSymbolTable( symbolTable_t *symbols, const char *filename, u_int16_t startAdrs, u_int16_t endAdrs )
{
    trace_printf( "Loading symbols from %s\n", filename );

    int globalCount = 0;
    int localCount = 0;

    FILE *fd = fopen( filename, "r" );

    if (!fd)
    {
        trace_printf( "**** Error opening symbol file %s\n", filename );
        return -1;
    }

    symbols->startAdrs = startAdrs;
    symbols->endAdrs = endAdrs;
    for (int i=0; i<65536; i++)
    {
        symbols->globals[i] = NULL;
        symbols->locals[i] = NULL;
    }

    /* symbol file example
XAML, 0x0024, 0, 0x0000
XAMH, 0x0025, 0, 0x0000
STL, 0x0026, 0, 0x0000
STH, 0x0027, 0, 0x0000
L, 0x0028, 0, 0x0000
H, 0x0029, 0, 0x0000
YSAV, 0x002a, 0, 0x0000
*/
    char line[256];
    while (fgets( line, sizeof(line), fd ))
    {
        char *sym = strtok( line, "," );
        char *adrs = strtok( NULL, "," );
        // char *local = strtok( NULL, "," );
        // char *comment = strtok( NULL, "," );

        if (sym && adrs)
        {
            uint16_t adrsVal = strtol( adrs, NULL, 16 );
            if (adrsVal>=startAdrs && adrsVal<=endAdrs)
            {
                if (symbols->globals[adrsVal])
                {
                    trace_printf( "**** Symbol clash %s => %s at %04X\n", symbols->globals[adrsVal], sym, adrsVal );
                    free( (void *)symbols->globals[adrsVal] );
                    symbols->globals[adrsVal] = NULL;
                }
                symbols->globals[adrsVal] = strdup(sym);
                globalCount++;
            }
            else
            {
                if (symbols->locals[adrsVal])
                {
                    trace_printf( "**** Symbol clash %s => %s at %04X\n", symbols->locals[adrsVal], sym, adrsVal );
                    free( (void *)symbols->locals[adrsVal] );
                    symbols->locals[adrsVal] = NULL;
                }
                symbols->locals[adrsVal] = strdup(sym);
                localCount++;
            }
        }
    }

    trace_printf( "Loaded %d global symbols and %d local symbols at %04X-%04X\n", globalCount, localCount, startAdrs, endAdrs );

    fclose( fd );

    return 0;
}    

symbolTable_t *s_symbols;

symbolTable_t *createSymbolTable( const char *name )
{
    symbolTable_t *symbols = (symbolTable_t *)malloc( sizeof(symbolTable_t) );
    if (!symbols)
        return NULL;
    strcpy( symbols->name, name );
    symbols->startAdrs = 0;
    symbols->endAdrs = 0;
    for (int i=0; i<65536; i++)
    {
        symbols->globals[i] = NULL;
        symbols->locals[i] = NULL;
    }

    return symbols;
}

//  Input: a file name
//  Output thename of the file
//  xxx/yyy/zzz/abc.def -> abc
const char *stemName( const char *filename )
{
    const char *p = strrchr( filename, '/' );
    if (!p)
        p = filename;
    else
        p++;

    const char *q = strrchr( p, '.' );
    if (!q)
        q = p+strlen(p);

    static char buffer[256];
    strncpy( buffer, p, q-p );
    buffer[q-p] = 0;

    return buffer;
}

void symbolTableStats( symbolTable_t *sym, int *globalCount, int *localCount )
{
    *globalCount = 0;
    *localCount = 0;

    for (int i=0; i<65536; i++)
    {
        if (sym->globals[i])
            (*globalCount)++;
        if (sym->locals[i])
            (*localCount)++;
    }
}

int loadSymbols( const char *filename, uint16_t startAdrs, uint16_t endAdrs )
{
    symbolTable_t *sym = createSymbolTable( stemName(filename) );
    if (!sym)
        return -1;

    if (loadSymbolTable( sym, filename, startAdrs, endAdrs )==-1)
    {
        free( sym );
        return -1;
    }

    sym->next = s_symbols;
    s_symbols = sym;


    int globalCount = 0;
    int localCount = 0;

    symbolTableStats( sym, &globalCount, &localCount );

    trace_printf( "Loaded %d+%d symbols from %s\n", globalCount, localCount, filename );

    return 0;
}

int tryLoadSymbolsFor( const char *filename, uint16_t start, uint16_t end )
{
    trace_printf( "#### tryLoadSymbolsFor: %s %04X %04X\n", filename, start, end );
    //  Replace extension (if any) with .SYM and loads the resulting file
    char buffer[1024];
    strcpy( buffer, filename );

    char *dot = strrchr( buffer, '.' );
    char *slash = strrchr( buffer, '/' );

    if( dot && (!slash || dot > slash) )
        *dot = 0;

    strcat( buffer, ".SYM" );

    trace_printf( "#### tryLoadSymbolsInFile: %s %04X %04X\n", buffer, start, end );

    return loadSymbols( buffer, start, end );
}

void dumpSymbolTable( symbolTable_t *symbols )
{
    console_printf( "Symbol table [%s]\n", symbols->name );
    console_printf( "Global symbols (for any address in $%04X-$%04X)\n", symbols->startAdrs, symbols->endAdrs );
    for (int i=0; i<65536; i++)
    {
        if (symbols->globals[i])
            console_printf( "%04X: %s\n", i, symbols->globals[i] );
    }

    console_printf( "Local symbols (for any instructions in $%04X-$%04X)\n", symbols->startAdrs, symbols->endAdrs );
    for (int i=0; i<65536; i++)
    {
        if (symbols->locals[i])
            console_printf( "%04X: %s\n", i, symbols->locals[i] );
    }
}

const char *symbolTableLookup0( symbolTable_t *me, uint16_t adrs, uint16_t pc )
{
    const char *sym = NULL;

    // If an address is within the range, the symbol table is authoritative
    if (adrs>=me->startAdrs && adrs<=me->endAdrs)
        sym = me->globals[adrs];

    // If the pc is withing the range, the symbol table local symbols are used
    if (!sym && pc>=me->startAdrs && pc<=me->endAdrs)
        sym = me->locals[adrs];

    return sym;
}

const char *symbolTableLookup( symbolTable_t *me, uint16_t adrs, uint16_t pc )
{
    const char *res = symbolTableLookup0( me, adrs, pc );

    if (res)
        return res;

    res = symbolTableLookup0( me, adrs-1, pc );

    if (!res)
        return NULL;

    static char buffer[256];
    sprintf( buffer, "%s+1", res );

    return buffer;
}

const char *symbolTableLookupChained( symbolTable_t *me, uint16_t adrs, uint16_t pc )
{
    if (!me)
        return NULL;

    const char *res = symbolTableLookup( me, adrs, pc );
    if (res)
        return res;
    
    return symbolTableLookupChained( me->next, adrs, pc );
}

const char *symbol( uint16_t adrs, uint16_t pc )
{
    return symbolTableLookupChained( s_symbols, adrs, pc );
}

void listSymbolTables()
{
    console_printf( "Loaded symbol tables:\n" );
    for (symbolTable_t *sym=s_symbols; sym; sym=sym->next)
    console_printf( "%04x-%04X : %s\n", sym->startAdrs, sym->endAdrs, sym->name );
}

int dumpSymbols( const char *tableName )
{
    for (symbolTable_t *sym=s_symbols; sym; sym=sym->next)
        if (!strcmp( sym->name, tableName ))
        {
            dumpSymbolTable( sym );
            return 0;
        }

    return -1;
}

int lookupSymbol( const char *name, uint16_t *adrs )
{
    for (symbolTable_t *sym=s_symbols; sym; sym=sym->next)
    {
        for (int i=0; i<65536; i++)
        {
            if (sym->globals[i] && !strcmp( sym->globals[i], name ))
            {
                *adrs = i;
                return 0;
            };
            if (sym->locals[i] && !strcmp( sym->locals[i], name ))
            {
                *adrs = i;
                return 0;
            }
        }
    }

    *adrs = 0xffff;

    return -1;
}
