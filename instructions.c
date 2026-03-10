#include "lc3.h"
#include <stdio.h>

void br(uint16_t instruction) {
    uint16_t offset = sign_extend(instruction & 0x1FF, 9);
    uint16_t cond = (instruction >> 9) & 0x7;
    if (cond & vm.reg[R_COND]) {
        vm.reg[R_PC] = vm.reg[R_PC] + offset;
    }
}
void add(uint16_t instruction) {
    uint16_t dest = (instruction >> 9) & 0x7;
    uint16_t first = (instruction >> 6) & 0x7;
    uint16_t imm = (instruction >> 5) & 0x1;
    if (imm) {
        uint16_t val = sign_extend(instruction & 0x1F, 5);
        vm.reg[dest] = vm.reg[first] + val;
    } else {
        uint16_t sec = instruction & 0x7;
        vm.reg[dest] = vm.reg[first] + vm.reg[sec];
    }
    update_flags(dest);
}
void ld(uint16_t instruction) {
    uint16_t dest = (instruction >> 9) & 0x7;
    uint16_t offset = sign_extend(instruction & 0x1FF, 9);
    vm.reg[dest] = mem_read(vm.reg[R_PC] + offset);
    update_flags(dest);
}
void st(uint16_t instruction) {
    uint16_t dest = (instruction >> 9) & 0x7;
    uint16_t offset = sign_extend(instruction & 0x1FF, 9);
    mem_write(vm.reg[R_PC] + offset, vm.reg[dest]);
}
void jsr(uint16_t instruction) {
    uint16_t flag = (instruction >> 11) & 1;
    vm.reg[R_R7] = vm.reg[R_PC];
    if (flag) {
        uint16_t offset = sign_extend(instruction & 0x7FF, 11);
        vm.reg[R_PC] = vm.reg[R_PC] + offset;
    } else {
        uint16_t addr = (instruction >> 6) & 0x7;
        vm.reg[R_PC] = vm.reg[addr];
    }
}
void andd(uint16_t instruction) {
    uint16_t dest = (instruction >> 9) & 0x7;
    uint16_t first = (instruction >> 6) & 0x7;
    uint16_t imm = (instruction >> 5) & 0x1;
    if (imm) {
        uint16_t val = sign_extend(instruction & 0x1F, 5);
        vm.reg[dest] = vm.reg[first] & val;
    } else {
        uint16_t sec = instruction & 0x7;
        vm.reg[dest] = vm.reg[first] & vm.reg[sec];
    }
    update_flags(dest);
}
void ldr(uint16_t instruction) {
    uint16_t dest = (instruction >> 9) & 0x7;
    uint16_t addr = (instruction >> 6) & 0x7;
    uint16_t offset = sign_extend(instruction & 0x3F, 6);
    vm.reg[dest] = mem_read(vm.reg[addr] + offset);
    update_flags(dest);
}
void str(uint16_t instruction) {
    uint16_t dest = (instruction >> 9) & 0x7;
    uint16_t addr = (instruction >> 6) & 0x7;
    uint16_t offset = sign_extend(instruction & 0x3F, 6);
    mem_write(vm.reg[addr] + offset, vm.reg[dest]);
}
void nott(uint16_t instruction) {
    uint16_t dest = (instruction >> 9) & 0x7;
    uint16_t addr = (instruction >> 6) & 0x7;
    vm.reg[dest] = ~vm.reg[addr];
    update_flags(dest);
}
void ldi(uint16_t instruction) {
    uint16_t dest = (instruction >> 9) & 0x7;
    uint16_t offset = sign_extend(instruction & 0x1FF, 9);
    vm.reg[dest] = mem_read(mem_read(vm.reg[R_PC] + offset));
    update_flags(dest);
}
void sti(uint16_t instruction) {
    uint16_t dest = (instruction >> 9) & 0x7;
    uint16_t offset = sign_extend(instruction & 0x1FF, 9);
    mem_write(mem_read(vm.reg[R_PC] + offset), vm.reg[dest]);
}
void jmp(uint16_t instruction) {
    uint16_t addr = (instruction >> 6) & 0x7;
    vm.reg[R_PC] = vm.reg[addr];
}
void lea(uint16_t instruction) {
    uint16_t dest = (instruction >> 9) & 0x7;
    uint16_t offset = sign_extend(instruction & 0x1FF, 9);
    vm.reg[dest] = vm.reg[R_PC] + offset;
    update_flags(dest);
}

static void trap_puts() {
    uint16_t* c = vm.memory + vm.reg[R_R0];
    while(*c) {
        putc((char)*c, stdout);
        ++c;
    }
    fflush(stdout);
}
static void trap_putsp() {
    uint16_t* c = vm.memory + vm.reg[R_R0];
    while(*c) {
        char c1 = (*c) & 0xFF;
        putc(c1, stdout);
        char c2 = (*c) >> 8;
        if (c2) {
            putc(c2, stdout);
        }
        ++c;
    }
    fflush(stdout);
}
void trap(uint16_t instruction) {
    vm.reg[R_R7] = vm.reg[R_PC];
    switch (instruction & 0xFF) {
        case TRAP_GETC:
            vm.reg[R_R0] = (uint16_t)getchar();
            update_flags(R_R0);
            break;
        case TRAP_OUT:
            putc((char)vm.reg[R_R0], stdout);
            fflush(stdout);
            break;
        case TRAP_PUTS:
            trap_puts();
            break;
        case TRAP_IN:
            char c = getchar();
            putc(c, stdout);
            fflush(stdout);
            vm.reg[R_R0] = (uint16_t)c;
            update_flags(R_R0);
            break;
        case TRAP_PUTSP:
            trap_putsp();
            break;
        case TRAP_HALT:
            puts("HALT");
            fflush(stdout);
            vm.running = 0;
            break;
    }
}
void rti_or_res(uint16_t instruction) {
    vm.running = 0; 
}