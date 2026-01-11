#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"
#include "registers.h"
#include "rom.h"

// functions

void cb_rotate_shift(uint8_t opcode, Registers* reg, uint8_t* v) {
    uint8_t sub = (opcode >> 3) & 0x07;
    uint8_t old = *v;
    uint8_t result = 0;

    switch (sub) {
        case 0: // RLC
            result = (old << 1) | (old >> 7);
            set_flag_c(reg, old >> 7);
            break;

        case 1: // RRC
            result = (old >> 1) | (old << 7);
            set_flag_c(reg, old & 1);
            break;

        case 2: // RL
            result = (old << 1) | get_flag_c(reg);
            set_flag_c(reg, old >> 7);
            break;

        case 3: // RR
            result = (old >> 1) | (get_flag_c(reg) << 7);
            set_flag_c(reg, old & 1);
            break;

        case 4: // SLA
            result = old << 1;
            set_flag_c(reg, old >> 7);
            break;

        case 5: // SRA
            result = (old >> 1) | (old & 0x80);
            set_flag_c(reg, old & 1);
            break;

        case 6: // SWAP
            result = (old << 4) | (old >> 4);
            set_flag_c(reg, 0);
            break;

        case 7: // SRL
            result = old >> 1;
            set_flag_c(reg, old & 1);
            break;
    }

    *v = result;

    set_flag_z(reg, result == 0);
    set_flag_n(reg, 0);
    set_flag_h(reg, 0);
}

void cb_bit(uint8_t bit, Registers* reg, uint8_t value) {
    set_flag_z(reg, !(value & (1 << bit)));
    set_flag_n(reg, 0);
    set_flag_h(reg, 1);
}

void cb_res(uint8_t bit, uint8_t* value) {
    *value &= ~(1 << bit);
}

void cb_set(uint8_t bit, uint8_t* value) {
    *value |= (1 << bit);
}



// find which register op is applied to
uint8_t* cb_get_reg(Registers* reg, uint8_t* m, int index) {
    switch (index) {
        case 0: return &reg->B;
        case 1: return &reg->C;
        case 2: return &reg->D;
        case 3: return &reg->E;
        case 4: return &reg->H;
        case 5: return &reg->L;
        case 6: return &m[reg->HL]; // hlp
        case 7: return &reg->A;
    }
    return 0;
}

// run cb op
void execute_cb(uint8_t opcode, Registers* reg, uint8_t* m, int* cycles) {
	
    uint8_t group = opcode >> 6; // type of operation
    uint8_t bit   = (opcode >> 3) & 0x07; 
    uint8_t r     = opcode & 0x07; // register

    uint8_t* value = cb_get_reg(reg, m, r);
    int is_hl = (r == 6);

    switch (group) {
        case 0: cb_rotate_shift(opcode, reg, value); break;
        case 1: cb_bit(bit, reg, *value); break;
        case 2: cb_res(bit, value); break;
        case 3: cb_set(bit, value); break;
    }

    *cycles += is_hl ? 16 : 8;
}