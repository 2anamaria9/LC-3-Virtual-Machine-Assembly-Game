#include "lc3.h"
#include <stdint.h>
#include <sys/time.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>

uint16_t sign_extend(uint16_t x, int bit_count) {
    uint16_t sign = bit_count - 1;
    if ((x >> sign) & 1) {
        x = x | (0xFFFF << bit_count);
    }
    return x;
}
uint16_t swap16(uint16_t x) {
    return (x << 8) | (x >> 8);
}
void update_flags(uint16_t r) {
    if (vm.reg[r] == 0) {
        vm.reg[R_COND] = FL_ZRO;
    } else if (vm.reg[r] >> 15) {
        vm.reg[R_COND] = FL_NEG;
    } else {
        vm.reg[R_COND] = FL_POS;
    }
}
uint16_t mem_read(uint16_t addr) {
    if (addr == 0xFE04) {
        return (uint16_t)(rand() & 0xFFFF);
    }
    if (addr == MR_KBDR) {
        uint16_t val = vm.memory[addr];
        vm.memory[MR_KBSR] &= ~0x8000; 
        return val;
    }
    return vm.memory[addr];
}
void mem_write(uint16_t addr, uint16_t value) {
    vm.memory[addr] = value;
}