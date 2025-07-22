#include "rom512.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint8_t *rom512 = NULL;

int loadRom512(const char *filename)
{
    if (rom512)
        releaseRom512();
    rom512 = (uint8_t *)malloc(512 * 1024);
    if (!rom512)
        return -1;

    FILE *fd = fopen(filename, "rb");
    if (!fd)
    {
        releaseRom512();
        return -1;
    }

    if (fread(rom512, 512 * 1024, 1, fd) != 1)
    {
        releaseRom512();
        fclose(fd);
        return -1;
    }

    fclose(fd);

    return 0;
}

//  Releases the loaded ROM
void releaseRom512(void)
{
    if (rom512)
    {
        free(rom512);
        rom512 = NULL;
    }
}

//  Copy a bank of the ROM in memory (pass a pointer to a 64K block)
int copyBank512(uint8_t bank, uint8_t *memory)
{
    if (!rom512)
        return -1;

    u_int8_t *src = rom512 + bank * 32 * 1024;
    //  Copy range 2000-7fff
    memcpy(memory + 0x2000, src + 0x2000, 24 * 1024);
    //  Copy range 0000-1fff
    memcpy(memory + 0x8000, src, 8 * 1024);
    //  Duplicate 6000-6fff
    memcpy(memory + 0xe000, src + 0x6000, 4 * 1024);

    printf("Copying bank %d to memory\n", bank);

    return 0;
}
