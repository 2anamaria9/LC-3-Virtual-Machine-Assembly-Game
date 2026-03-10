#ifndef LC3_H
#define LC3_H

#include<stdint.h>
#include<stdio.h>

#define TRACE_SIZE 1000
#define MEMORY_MAX (1 << 16)

//registers
enum {
    R_R0 = 0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC,
    R_COND,
    R_COUNT
};

typedef struct {
    uint16_t memory[MEMORY_MAX];
    uint16_t reg[R_COUNT];
    int running;
} vm_state_t;

extern vm_state_t vm;

//opcodes
enum {
    OP_BR = 0, // branch
    OP_ADD,    // add
    OP_LD,     // load
    OP_ST,     // store
    OP_JSR,    // jump register
    OP_AND,    // bitwise and
    OP_LDR,    // load register
    OP_STR,    // store register
    OP_RTI,    // unused
    OP_NOT,    // bitwise not
    OP_LDI,    // load indirect
    OP_STI,    // store indirect
    OP_JMP,    // jump
    OP_RES,    // reserved (unused)
    OP_LEA,    // load effective address
    OP_TRAP    // execute trap
};

//condition flags
enum {
    FL_POS = 1 << 0,
    FL_ZRO = 1 << 1,
    FL_NEG = 1 << 2,
};

//TRAP codes
enum {
    TRAP_GETC = 0x20,  /* get character from keyboard, not echoed onto the terminal */
    TRAP_OUT = 0x21,   /* output a character */
    TRAP_PUTS = 0x22,  /* output a word string */
    TRAP_IN = 0x23,    /* get character from keyboard, echoed onto the terminal */
    TRAP_PUTSP = 0x24, /* output a byte string */
    TRAP_HALT = 0x25   /* halt the program */
};

//memory mapped registers
enum {
    MR_KBSR = 0xFE00, /* keyboard status */
    MR_KBDR = 0xFE02  /* keyboard data */
};

//utils
uint16_t sign_extend(uint16_t x, int bit_count);
uint16_t swap16(uint16_t x);
void update_flags(uint16_t r);
uint16_t mem_read(uint16_t addr);
void mem_write(uint16_t addr, uint16_t value);
uint16_t check_key();

//instructions
void br(uint16_t instruction);
void add(uint16_t instruction);
void ld(uint16_t instruction);
void st(uint16_t instruction);
void jsr(uint16_t instruction);
void andd(uint16_t instruction);
void ldr(uint16_t instruction);
void str(uint16_t instruction);
void nott(uint16_t instruction);
void ldi(uint16_t instruction);
void sti(uint16_t instruction);
void jmp(uint16_t instruction);
void lea(uint16_t instruction);
void trap(uint16_t instruction);
void rti_or_res(uint16_t instruction);

#endif
