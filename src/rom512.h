//  SiliconInsider rom512k

/* This ROM card has the following characteristics:

    512K in 16 banks of 32K
    Banks are numbered 0-15

    Each 32K will be mapped contiguously in the Apple1 memory space
    From $2000 to $9FFF
    $E000 will be a mirror of $6000-$6FFF, so BASIC can be used.

    On the physical layout of the eeprom, the 32K will be mapped in the following way:
    0000-1FFF : 8000-9FFF
    2000-7FFF : 2000-7FFF
    (with 6000-6FFF replicated at E000-EFFF)

    Banks are switched by any access to $A000-$BFFF.
    The last nibble is the accessed page.
*/

#include <stdint.h>

//  Loads the ROM file
int loadRom512(const char *rom512k);

//  Releases the loaded ROM
void releaseRom512(void);

//  Copy a bank of the ROM in memory (pass a pointer to a 6K block)
int copyBank512(uint8_t bank, uint8_t *memory);
