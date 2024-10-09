// A simple 6502 disassembler

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "memory.h"

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

const char *disassemble( uint16_t pc, const uint8_t *mem )
{
    static char buffer[256];
    char mem_buffer[256];
    char operands[256];
    uint8_t opcode = mem[0];

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
            sprintf( operands, "$%02X", mem[1] );
            break;
        case ZPX:
            sprintf( operands, "$%02X,X", mem[1] );
            break;
        case ZPY:
            sprintf( operands, "$%02X,Y", mem[1] );
            break;
        case REL:
            sprintf( operands, "$%04X", pc+(signed char)mem[1]+2 );
            break;
        case ABS:
            sprintf( operands, "$%02X%02X", mem[2], mem[1] );
            break;
        case ABX:
            sprintf( operands, "$%02X%02X,X", mem[2], mem[1] );
            break;
        case ABY:
            sprintf( operands, "$%02X%02X,Y", mem[2], mem[1] );
            break;
        case IND:
            sprintf( operands, "($%02X%02X)", mem[2], mem[1] );
            break;
        case IZX:
            sprintf( operands, "($%02X,X)", mem[1] );
            break;
        case IZY:
            sprintf( operands, "($%02X),Y", mem[1] );
            break;
        case N_A:
            sprintf( operands, "????" );
            break;
        default:
        	trace_printf( "CPU: %d\n", addressing[opcode] );
            exit(-1);
    }

    sprintf( buffer, "%04X: %s   %s %s", (int)pc, mem_buffer, opcodes[opcode], operands );

    return buffer;
}
