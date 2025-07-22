#include <stdint.h>

// Returns symbol for address, or NULL
const char *symbol(uint16_t adrs, uint16_t pc);

// Load symbols from a file
int loadSymbols(const char *filename, uint16_t startAdrs, uint16_t endAdrs);

//  Try to find the symbols for a file (rename it to .SYM)
int tryLoadSymbolsFor(const char *filename, uint16_t start, uint16_t end);

// List all symbol tables
void listSymbolTables();

// Dump a symbol table
int dumpSymbols(const char *tableName);

//  Symbol lookup
int lookupSymbol(const char *name, uint16_t *adrs);
