// A simple 6502 disassembler

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "memory.h"
#include "symbols.h"

static const char *opcodes[256] =
{
    "BRK", "ORA", "???", "???", "???", "ORA", "ASL", "???", "PHP", "ORA", "ASL", "???", "???", "ORA", "ASL", "???",
    "BPL", "ORA", "???", "???", "???", "ORA", "ASL", "???", "CLC", "ORA", "???", "???", "???", "ORA", "ASL", "???",
    "JSR", "AND", "???", "???", "BIT", "AND", "ROL", "???", "PLP", "AND", "ROL", "???", "BIT", "AND", "ROL", "???",
    "BMI", "AND", "???", "???", "???", "AND", "ROL", "???", "SEC", "AND", "???", "???", "???", "AND", "ROL", "???",
    "RTI", "EOR", "???", "???", "???", "EOR", "LSR", "???", "PHA", "EOR", "LSR", "???", "JMP", "EOR", "LSR", "???",
    "BVC", "EOR", "???", "???", "???", "EOR", "LSR", "???", "CLI", "EOR", "???", "???", "???", "EOR", "LSR", "???",
    "RTS", "ADC", "???", "???", "???", "ADC", "ROR", "???", "PLA", "ADC", "ROR", "???", "JMP", "ADC", "ROR", "???",
    "BVS", "ADC", "???", "???", "???", "ADC", "ROR", "???", "SEI", "ADC", "???", "???", "???", "ADC", "ROR", "???",
    "???", "STA", "???", "???", "STY", "STA", "STX", "???", "DEY", "???", "TXA", "???", "STY", "STA", "STX", "???",
    "BCC", "STA", "???", "???", "STY", "STA", "STX", "???", "TYA", "STA", "TXS", "???", "???", "STA", "???", "???",
    "LDY", "LDA", "LDX", "???", "LDY", "LDA", "LDX", "???", "TAY", "LDA", "TAX", "???", "LDY", "LDA", "LDX", "???",
    "BCS", "LDA", "???", "???", "LDY", "LDA", "LDX", "???", "CLV", "LDA", "TSX", "???", "LDY", "LDA", "LDX", "???",
    "CPY", "CMP", "???", "???", "CPY", "CMP", "DEC", "???", "INY", "CMP", "DEX", "???", "CPY", "CMP", "DEC", "???",
    "BNE", "CMP", "???", "???", "???", "CMP", "DEC", "???", "CLD", "CMP", "???", "???", "???", "CMP", "DEC", "???",
    "CPX", "SBC", "???", "???", "CPX", "SBC", "INC", "???", "INX", "SBC", "???", "SBC", "CPX", "SBC", "INC", "???",
    "BEQ", "SBC", "???", "???", "???", "SBC", "INC", "???", "SED", "SBC", "???", "???", "???", "SBC", "INC", "???"
};

typedef enum
{
    IMP, IMM, ZP0, ZPX, ZPY, REL, ABS, ABX, ABY, IND, IZX, IZY, N_A
} eAddressingMode;

static eAddressingMode addressing[256] =
{
    IMP, IZX, N_A, N_A, N_A, ZP0, ZP0, N_A, IMP, IMM, IMP, N_A, N_A, ABS, ABS, N_A,
    REL, IZY, N_A, N_A, N_A, ZPX, ZPX, N_A, IMP, ABY, N_A, N_A, N_A, ABX, ABX, N_A,
    ABS, IZX, N_A, N_A, ZP0, ZP0, ZP0, N_A, IMP, IMM, IMP, N_A, ABS, ABS, ABS, N_A,
    REL, IZY, N_A, N_A, N_A, ZPX, ZPX, N_A, IMP, ABY, N_A, N_A, N_A, ABX, ABX, N_A,
    IMP, IZX, N_A, N_A, N_A, ZP0, ZP0, N_A, IMP, IMM, IMP, N_A, ABS, ABS, ABS, N_A,
    REL, IZY, N_A, N_A, N_A, ZPX, ZPX, N_A, IMP, ABY, N_A, N_A, N_A, ABX, ABX, N_A,
    IMP, IZX, N_A, N_A, N_A, ZP0, ZP0, N_A, IMP, IMM, IMP, N_A, IND, ABS, ABS, N_A,
    REL, IZY, N_A, N_A, N_A, ZPX, ZPX, N_A, IMP, ABY, N_A, N_A, N_A, ABX, ABX, N_A,
    N_A, IZX, N_A, N_A, ZP0, ZP0, ZP0, N_A, IMP, N_A, IMP, N_A, ABS, ABS, ABS, N_A,
    REL, IZY, N_A, N_A, ZPX, ZPX, ZPY, N_A, IMP, ABY, IMP, N_A, N_A, ABX, N_A, N_A,
    IMM, IZX, IMM, N_A, ZP0, ZP0, ZP0, N_A, IMP, IMM, IMP, N_A, ABS, ABS, ABS, N_A,
    REL, IZY, N_A, N_A, ZPX, ZPX, ZPY, N_A, IMP, ABY, IMP, N_A, ABX, ABX, ABY, N_A,
    IMM, IZX, N_A, N_A, ZP0, ZP0, ZP0, N_A, IMP, IMM, IMP, N_A, ABS, ABS, ABS, N_A,
    REL, IZY, N_A, N_A, N_A, ZPX, ZPX, N_A, IMP, ABY, N_A, N_A, N_A, ABX, ABX, N_A,
    IMM, IZX, N_A, N_A, ZP0, ZP0, ZP0, N_A, IMP, IMM, IMP, IMM, ABS, ABS, ABS, N_A,
    REL, IZY, N_A, N_A, N_A, ZPX, ZPX, N_A, IMP, ABY, N_A, N_A, N_A, ABX, ABX, N_A
};

static int byteCount[N_A+1] = {
    1, // IMP
    2, // IMM
    2, // ZP0
    2, // ZPX
    2, // ZPY
    2, // REL
    3, // ABS
    3, // ABX
    3, // ABY
    3, // IND
    2, // IZX
    2, // IZY
    1  // N_A (not applicable)
};

const char *stringFromZP( uint8_t adrs, uint16_t pc )
{
    const char *sym = symbol( adrs, pc );
    if (sym)
        return sym;

    static char buffer[256];
    sprintf( buffer, "$%02X", adrs );
    return buffer;
}

const char *stringFromAdrs( u_int16_t adrs, uint16_t pc )
{
    const char *sym = symbol( adrs, pc );
    if (sym)
        return sym;

    static char buffer[256];
    sprintf( buffer, "$%04X", adrs );
    return buffer;
}

const char *stringFrom2Bytes( uint8_t mem0, uint8_t mem1, uint16_t pc )
{
    uint16_t adrs = (mem1<<8) | mem0;
    return stringFromAdrs( adrs, pc );
}

const char *disassemble( uint16_t pc, const uint8_t *mem, int *len )
{
    static char buffer[256];
    char mem_buffer[256];
    char operands[256];
    uint8_t opcode = mem[0];

    buffer[0] = 0;
    const char *symbolicPC = symbol( pc, pc );
    if (symbolicPC)
        sprintf( buffer, "%s:\n", symbolicPC );

    switch (byteCount[addressing[opcode]])
    {
        case 1:
            sprintf( mem_buffer, "%02X      ", mem[0] );
            break;
        case 2:
            sprintf( mem_buffer, "%02X %02X   ", mem[0], mem[1] );
            break;
        case 3:
            sprintf( mem_buffer, "%02X %02X %02X", mem[0], mem[1], mem[2] );
            break;
        default:
            exit(-1);
    }

    switch (addressing[opcode])
    {
        case IMP:
            sprintf( operands, "" );
            break;
        case IMM:
            sprintf( operands, "#$%02X", mem[1] );
            break;
        case ZP0:
            sprintf( operands, "%s", stringFromZP( mem[1], pc ) );
            break;
        case ZPX:
            sprintf( operands, "%s,X", stringFromZP( mem[1], pc ) );
            break;
        case ZPY:
            sprintf( operands, "%s,Y", stringFromZP( mem[1], pc ) );
            break;
        case REL:
            sprintf( operands, "%s", stringFromAdrs( pc+(signed char)mem[1]+2, pc ) );
            break;
        case ABS:
            sprintf( operands, "%s", stringFrom2Bytes( mem[1], mem[2], pc ) );
            break;
        case ABX:
            sprintf( operands, "%s,X", stringFrom2Bytes( mem[1], mem[2], pc ) );
            break;
        case ABY:
            sprintf( operands, "%s,Y", stringFrom2Bytes( mem[1], mem[2], pc ) );
            break;
        case IND:
            sprintf( operands, "(%s)", stringFrom2Bytes( mem[1], mem[2], pc ) );
            break;
        case IZX:
            sprintf( operands, "(%s,X)", stringFromZP( mem[1], pc ) );
            break;
        case IZY:
            sprintf( operands, "(%s),Y", stringFromZP( mem[1], pc ) );
            break;
        case N_A:
            sprintf( operands, "????" );
            break;
        default:
        	console_printf( "CPU: %d\n", addressing[opcode] );
            exit(-1);
    }

    sprintf( buffer, "%s%04X: %s   %s %s", buffer, (int)pc, mem_buffer, opcodes[opcode], operands );

    if (len)
        *len = byteCount[addressing[opcode]];

    return buffer;
}
