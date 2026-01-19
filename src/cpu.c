#include <stdio.h>
#include <stdlib.h>

#include "cb.h"
#include "config.h"
#include "memory.h"
#include "registers.h"
#include "rom.h"

/* 
-- operand lengths --
number of bytes following the opcode to be used as operands
0: opcode + operands = 1 byte
1: opcode + operands = 2 bytes (8-bit  operand)
2: opcode + operands = 3 bytes (16-bit operand)
invalid instructions given value of 0 so they are just skipped
*/
int operand_lengths[256] = {
//  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
	0, 2, 0, 0, 0, 0, 1, 0, 2, 0, 0, 0, 0, 0, 1, 0, // 0x0x
	1, 2, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, // 0x1x
	1, 2, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, // 0x2x
	1, 2, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, // 0x3x
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x4x
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x5x
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x6x
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x7x
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x8x
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x9x
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xAx
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xBx
	0, 0, 2, 2, 2, 0, 1, 0, 0, 0, 2, 1, 2, 2, 1, 0, // 0xCx
	0, 0, 2, 0, 2, 0, 1, 0, 0, 0, 2, 0, 2, 0, 1, 0, // 0xDx
	1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 2, 0, 0, 0, 1, 0, // 0xEx
	1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 2, 0, 0, 0, 1, 0  // 0xFx
};

// minimum lengths of each instructions in cpu cycles

int cycle_lengths[256] = {
//  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
    4,  12, 8,  8,  4,  4,  8,  4,  20, 8,  8,  8,  4,  4,  8,  4,  // 0x0x
    4,  12, 12, 8,  8,  4,  4,  8,  4,  12, 8,  8,  8,  4,  4,  8,  // 0x1x
    8,  12, 12, 8,  8,  4,  4,  8,  4,  12, 8,  8,  8,  4,  4,  8,  // 0x2x
    12, 12, 12, 8,  8,  12, 12, 12, 4,  12, 8,  8,  8,  4,  4,  8,  // 0x3x
    4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,  // 0x4x
    4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,  // 0x5x
    4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,  //0x6x
    8,  8,  8,  8,  8,  8,  4,  8,  4,  4,  4,  4,  4,  4,  8,  4,  // 0x7x
    4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,  // 0x8x
    4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,  // 0x9x
    4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,  // 0xAx
    4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,  // 0xBx
    8,  12, 12, 16, 12, 8,  20, 8,  16, 12, 4,  24, 24, 8,  16, 8,  // 0xCx
    8,  12, 12, 16, 8,  16, 20, 8,  16, 12, 8,  24, 8,  16, 16, 8,  // 0xDx
    12, 12, 8,  16, 8,  16, 16, 8,  4,  16, 8,  16, 8,  4,  16, 8,  // 0xEx
    12, 12, 8,  16, 8,  16, 8,  8,  16, 8,  12, 4,  16, 8,  16, 8 // 0xFx
};

/*
-- Flag Functions --
*/

void set_flag_z(Registers* reg, int enabled) {
    if (enabled) reg->F |= FLAG_Z;
    else         reg->F &= ~FLAG_Z;
}

// Set or clear Subtract flag
void set_flag_n(Registers* reg, int enabled) {
    if (enabled) reg->F |= FLAG_N;
    else         reg->F &= ~FLAG_N;
}

// Set or clear Half-Carry flag
void set_flag_h(Registers* reg, int enabled) {
    if (enabled) reg->F |= FLAG_H;
    else         reg->F &= ~FLAG_H;
}

// Set or clear Carry flag
void set_flag_c(Registers* reg, int enabled) {
    if (enabled) reg->F |= FLAG_C;
    else         reg->F &= ~FLAG_C;
}

int get_flag_z(Registers* reg) {
    return (reg->F & FLAG_Z) != 0;
}

int get_flag_n(Registers* reg) {
    return (reg->F & FLAG_N) != 0;
}

int get_flag_h(Registers* reg) {
    return (reg->F & FLAG_H) != 0;
}

int get_flag_c(Registers* reg) {
    return (reg->F & FLAG_C) != 0;
}

/*
-- CPU functions --
*/

// increment
uint8_t inc(Registers* reg, uint8_t value) {
    uint8_t result = value + 1;

    set_flag_z(reg, result == 0);
    set_flag_n(reg, 0);
    set_flag_h(reg, (value & 0x0F) == 0x0F);

    return result;
}

// decrement
uint8_t dec(Registers* reg, uint8_t value) {
    uint8_t result = value - 1;

    set_flag_z(reg, result == 0);
    set_flag_n(reg, 1);
    set_flag_h(reg, (value & 0x0F) == 0x00);

    return result;
}

// 8-bit add
uint8_t add(Registers* reg, uint8_t a, uint8_t b) {
    uint16_t result16 = a + b; // for carry flag
    uint8_t result = (uint8_t)result16;

    set_flag_z(reg, result == 0);
    set_flag_n(reg, 0);
    set_flag_h(reg, ((a & 0x0F) + (b & 0x0F)) > 0x0F);
    set_flag_c(reg, result16 > 0xFF);

    return result;
}

// 8-bit subtract
uint8_t sub(Registers* reg, uint8_t a, uint8_t b) {
    uint16_t result16 = a - b; // for carry flag
    uint8_t result = (uint8_t)result16;

    set_flag_z(reg, result == 0);
    set_flag_n(reg, 1);
    set_flag_h(reg, (a & 0x0F) < (b & 0x0F));
    set_flag_c(reg, a < b);

    return result;
}

// 16-bit add
uint16_t add16(Registers* reg, uint16_t a, uint16_t b) {
    uint32_t result = (uint32_t)a + (uint32_t)b; // for carry flag

    set_flag_n(reg, 0);
    set_flag_h(reg, ((a & 0x0FFF) + (b & 0x0FFF)) > 0x0FFF);
    set_flag_c(reg, result > 0xFFFF);

    return (uint16_t)result;
}

// 8-bit add with carry
uint8_t adc(Registers* reg, uint8_t a, uint8_t value) {
    uint8_t carry = get_flag_c(reg) ? 1 : 0;
    uint16_t result = (uint16_t)a + (uint16_t)value + carry;

    set_flag_z(reg, (result & 0xFF) == 0);
    set_flag_n(reg, 0);
    set_flag_h(reg, ((a & 0xF) + (value & 0xF) + carry) > 0xF);
    set_flag_c(reg, result > 0xFF);

    return (uint8_t)result;
}

// 8-bit subtract with carry
uint8_t sbc(Registers* reg, uint8_t a, uint8_t value) {
    uint8_t carry = get_flag_c(reg) ? 1 : 0;
    int16_t result = (int16_t)a - (int16_t)value - carry;

    set_flag_z(reg, (result & 0xFF) == 0);
    set_flag_n(reg, 1); 
    set_flag_h(reg, ((a & 0xF) - (value & 0xF) - carry) < 0);
    set_flag_c(reg, result < 0);

    return (uint8_t)result;
}

// and
uint8_t cpu_and(Registers* reg, uint8_t value) {
    uint8_t result = reg->A & value;

    set_flag_z(reg, result == 0);
    set_flag_n(reg, 0);
    set_flag_h(reg, 1);
    set_flag_c(reg, 0);

    return result;
}

// or 
uint8_t cpu_or(Registers* reg, uint8_t value) {
    uint8_t result = reg->A | value;

    set_flag_z(reg, result == 0);
    set_flag_n(reg, 0);
    set_flag_h(reg, 0);
    set_flag_c(reg, 0);

    return result;
}

// xor
uint8_t cpu_xor(Registers* reg, uint8_t value) {
    uint8_t result = reg->A ^ value;

    set_flag_z(reg, result == 0);
    set_flag_n(reg, 0);
    set_flag_h(reg, 0);
    set_flag_c(reg, 0);

    return result;
}

// 8-bit compare
void cpu_cp(Registers* reg, uint8_t value) {
    uint8_t result = reg->A - value;

    set_flag_z(reg, result == 0);
    set_flag_n(reg, 1);
    set_flag_h(reg, (reg->A & 0xF) < (value & 0xF));
    set_flag_c(reg, reg->A < value);
}

// load 16-bit value into memory
void ld_uint16 (Memory* m, uint16_t operand, uint16_t value) {
	
	write8(m, operand, value & 0xFF); // low byte
	write8(m, operand + 1, (value >> 8) & 0xFF); // high byte
}

// push to stack
void push(Registers* reg, Memory* m, uint16_t value) {
    reg->SP--;
	write8(m, reg->SP, (value >> 8) & 0xFF);

    reg->SP--;
	write8(m, reg->SP, value & 0xFF);
}


// pop from stack
uint16_t pop(Registers* reg, Memory* m) {
    uint8_t low = read8(m, reg->SP);
    reg->SP++;

    uint8_t high = read8(m, reg->SP);
    reg->SP++;

    return ((uint16_t)high << 8) | low;
}

// reset
void rst(Registers* reg, Memory* m, uint16_t value) {
    push(reg, m, reg->PC);
    reg->PC = value;
}

// -- instructions --

/*
information on opcodes found at: https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html
 - name of opcode
 - corresponding byte format
 - size of opcode + operand in bytes
*/

// --- 0x0x ---

// 0x00
void nop(Registers* reg) {;}
// 0x01
void ld_bc_xx(Registers* reg, uint16_t operand) {reg->BC = operand;}
// 0x02
void ld_bcp_a(Registers* reg, Memory* m) {reg->A = read8(m,reg->BC);}
// 0x03
void inc_bc(Registers* reg) {reg->BC++;}
// 0x04
void inc_b(Registers* reg) {reg->B = inc(reg, reg->B);}
// 0x05
void dec_b(Registers* reg) {reg->B = dec(reg, reg->B);}
// 0x06
void ld_b_x(Registers* reg, uint8_t operand) {reg->B = operand;}
// 0x07
void rlca(Registers* reg) {
	// move end bit to the start
    uint8_t a = reg->A;
    int bit7 = (a & 0x80) != 0;

    reg->A = (a << 1) | bit7;

    set_flag_c(reg, bit7);
    // clear other flags
    set_flag_z(reg, 0);
    set_flag_n(reg, 0);
    set_flag_h(reg, 0);
}
// 0x08
void ld_xxp_sp(Registers* reg, Memory* m, uint16_t operand) {ld_uint16(m,operand,reg->SP);}
// 0x09
void add_hl_bc(Registers* reg) {reg->HL = add16(reg, reg->HL, reg->BC);}
// 0x0A
void ld_a_bcp(Registers* reg, Memory* m) {reg->A = read8(m,reg->BC);}
// 0x0B
void dec_bc(Registers* reg) {reg->BC--;}
// 0x0C
void inc_c(Registers* reg) {reg->C = inc(reg, reg->C);}
// 0x0D
void dec_c(Registers* reg) {reg->C = dec(reg, reg->C);}
// 0x0E
void ld_c_x(Registers* reg, uint8_t operand) {reg->C = operand;}
// 0x0F
void rrca(Registers* reg) {
	// move first bit to the end
    uint8_t a = reg->A;
    int bit0 = a & 0x01;

    reg->A = (a >> 1) | (bit0 << 7);

    set_flag_c(reg, bit0);
	// clear other flags
    set_flag_z(reg, 0);
    set_flag_n(reg, 0);
    set_flag_h(reg, 0);
}

// --- 0x1x --- 

// 0x10
void stop(Registers* reg) {reg->halted = 1;}
// 0x11
void ld_de_xx(Registers* reg, uint16_t operand) {reg->DE = operand;}
// 0x12
void ld_dep_a(Registers* reg, Memory* m) {reg->A = read8(m,reg->DE);}
// 0x13
void inc_de(Registers* reg) {reg->DE++;}
// 0x14
void inc_d(Registers* reg) {reg->D = inc(reg, reg->D);}
// 0x15
void dec_d(Registers* reg) {reg->D = dec(reg, reg->D);}
// 0x16
void ld_d_x(Registers* reg, uint8_t operand) {reg->D = operand;}
// 0x17
void rla(Registers* reg) {
	// move all bits left including carry
    uint8_t old_a = reg->A;
    int old_carry = get_flag_c(reg);

    reg->A = (old_a << 1) | old_carry;

    set_flag_c(reg, (old_a & 0x80) != 0);
    // clear other flags
	set_flag_z(reg, 0);
    set_flag_n(reg, 0);
    set_flag_h(reg, 0);
}
// 0x18
void jr_x(Registers* reg, uint8_t operand) {reg->PC += 2+(int8_t)operand;}
// 0x19
void add_hl_de(Registers* reg) {reg->HL = add16(reg, reg->HL, reg->DE);}
// 0x1A
void ld_a_dep(Registers* reg, Memory* m) {reg->A = read8(m,reg->DE);}
// 0x1B
void dec_de(Registers* reg) {reg->DE--;}
// 0x1C
void inc_e(Registers* reg) {reg->E = inc(reg, reg->E);}
// 0x1D
void dec_e(Registers* reg) {reg->E = dec(reg, reg->E);}
// 0x1E
void ld_e_x(Registers* reg, uint8_t operand) {reg->E = operand;}
// 0x1F
void rra(Registers* reg) {
	// move all bits right including carry
    uint8_t old_a = reg->A;
    int old_carry = get_flag_c(reg);

    reg->A = (old_a >> 1) | (old_carry << 7);

    set_flag_c(reg, old_a & 0x01);
    // clear other flags
    set_flag_z(reg, 0);
    set_flag_n(reg, 0);
    set_flag_h(reg, 0);
}

// --- 0x2x --- 

// 0x20
void jr_nz_x(Registers* reg, uint8_t operand, int* cycles) {
	if (!get_flag_z(reg)) {	
		reg->PC += 2+(int8_t)operand; 
		*cycles += 4; }
}
// 0x21
void ld_hl_xx(Registers* reg,uint16_t operand) {reg->HL = operand;}
// 0x22
void ldi_hlp_a(Registers* reg, Memory* m) {write8(m, reg->HL, reg->A); reg->HL++;}
// 0x23
void inc_hl(Registers* reg) {reg->HL++;}
// 0x24
void inc_h(Registers* reg) {reg->H = inc(reg, reg->H);}
// 0x25
void dec_h(Registers* reg) {reg->H = dec(reg, reg->H);}
// 0x26
void ld_h_x(Registers* reg, uint8_t operand) {reg->H = operand;}
// 0x27
void daa(Registers* reg) {
    uint8_t a = reg->A;
    int adjust = 0;
    int carry = get_flag_c(reg);
	
	// n = 0: addition
	// n = 1: subtraction
    if (!get_flag_n(reg)) {
        if (get_flag_h(reg) || (a & 0x0F) > 9)
            adjust |= 0x06;

        if (get_flag_c(reg) || a > 0x99) {
            adjust |= 0x60;
            carry = 1;
        }

        a += adjust;
    } else {
        if (get_flag_h(reg))
            adjust |= 0x06;
        if (get_flag_c(reg))
            adjust |= 0x60;

        a -= adjust;
    }

    reg->A = a;

    set_flag_z(reg, reg->A == 0);
    set_flag_h(reg, 0);
    set_flag_c(reg, carry);
}
// 0x28
void jr_z_x(Registers* reg, uint8_t operand, int* cycles) {
	if (get_flag_z(reg)) {	
		reg->PC += 2+(int8_t)operand; 
		*cycles += 4; }
}
// 0x29
void add_hl_hl(Registers* reg) {reg->HL = add16(reg, reg->HL, reg->HL);}
// 0x2A
void ldi_a_hlp(Registers* reg, Memory* m) {reg->A = read8(m,reg->HL); reg->HL++;}
// 0x2B
void dec_hl(Registers* reg) {reg->HL--;}
// 0x2C
void inc_l(Registers* reg) {reg->L = inc(reg, reg->L);}
// 0x2D
void dec_l(Registers* reg) {reg->L = dec(reg, reg->L);}
// 0x2E
void ld_l_x(Registers* reg, uint8_t operand) {reg->L = operand;}
// 0x2F
void cpl(Registers* reg) {
	// invert reg a
    reg->A = ~reg->A;

	// set flags
    set_flag_n(reg, 1);
    set_flag_h(reg, 1);
}

// --- 0x3x --- 

// 0x30
void jr_nc_x(Registers* reg, uint8_t operand, int* cycles) {
	if (!get_flag_c(reg)) {	
		reg->PC += 2+(int8_t)operand; 
		*cycles += 4; }
}
// 0x31
void ld_sp_xx(Registers* reg, uint16_t operand) {reg->SP = operand;}
// 0x32
void ldd_hlp_a(Registers* reg, Memory* m) {write8(m, reg->HL, reg->A); reg->HL--;}
// 0x33
void inc_sp(Registers* reg) {reg->SP++;}
// 0x34
void inc_hlp(Registers* reg, Memory* m) {write8(m, reg->HL, inc(reg, read8(m,reg->HL)));}
// 0x35
void dec_hlp(Registers* reg, Memory* m) {write8(m, reg->HL, dec(reg, read8(m,reg->HL)));}
// 0x36
void ld_hlp_x(Registers* reg, Memory* m, uint8_t operand) {write8(m, reg->HL, operand);}
// 0x37
void scf(Registers* reg) {
    set_flag_c(reg, 1);
    set_flag_n(reg, 0);
    set_flag_h(reg, 0);
}
// 0x38
void jr_c_x(Registers* reg, uint8_t operand, int* cycles) {
	if (get_flag_c(reg)) {	
		reg->PC += 2+(int8_t)operand; 
		*cycles += 4; }
}
// 0x39
void add_hl_sp(Registers* reg) {reg->HL = add16(reg, reg->HL, reg->SP);}
// 0x3A
void ldd_a_hlp(Registers* reg, Memory* m) {reg->A = read8(m, reg->HL); reg->HL--;}
// 0x3B
void dec_sp(Registers* reg) {reg->SP--;}
// 0x3C
void inc_a(Registers* reg) {reg->A = inc(reg, reg->A);}
// 0x3D
void dec_a(Registers* reg) {reg->A = dec(reg, reg->A);}
// 0x3E
void ld_a_x(Registers* reg, uint8_t operand) {reg->A = operand;}
// 0x3F
void ccf(Registers* reg) {
    set_flag_c(reg, !get_flag_c(reg)); // flips carry flag
    set_flag_n(reg, 0);
    set_flag_h(reg, 0);
}

// --- 0x4x ---

// 0x40
void ld_b_b(Registers* reg) {reg->B = reg->B;}
// 0x41
void ld_b_c(Registers* reg) {reg->B = reg->C;}
// 0x42
void ld_b_d(Registers* reg) {reg->B = reg->D;}
// 0x43
void ld_b_e(Registers* reg) {reg->B = reg->E;}
// 0x44
void ld_b_h(Registers* reg) {reg->B = reg->H;}
// 0x45
void ld_b_l(Registers* reg) {reg->B = reg->L;}
// 0x46
void ld_b_hlp(Memory* m, Registers* reg) {reg->B = read8(m,reg->HL);}
// 0x47
void ld_b_a(Registers* reg) {reg->B = reg->A;}
// 0x48
void ld_c_b(Registers* reg) {reg->C = reg->B;}
// 0x49
void ld_c_c(Registers* reg) {reg->C = reg->C;}
// 0x4A
void ld_c_d(Registers* reg) {reg->C = reg->D;}
// 0x4B
void ld_c_e(Registers* reg) {reg->C = reg->E;}
// 0x4C
void ld_c_h(Registers* reg) {reg->C = reg->H;}
// 0x4D
void ld_c_l(Registers* reg) {reg->C = reg->L;}
// 0x4E
void ld_c_hlp(Memory* m, Registers* reg) {reg->C = read8(m,reg->HL);}
// 0x4F
void ld_c_a(Registers* reg) {reg->C = reg->A;}

// --- 0x5x ---

// 0x50
void ld_d_b(Registers* reg) {reg->D = reg->B;}
// 0x51
void ld_d_c(Registers* reg) {reg->D = reg->C;}
// 0x52
void ld_d_d(Registers* reg) {reg->D = reg->D;}
// 0x53
void ld_d_e(Registers* reg) {reg->D = reg->E;}
// 0x54
void ld_d_h(Registers* reg) {reg->D = reg->H;}
// 0x55
void ld_d_l(Registers* reg) {reg->D = reg->L;}
// 0x56
void ld_d_hlp(Memory* m, Registers* reg) {reg->D = read8(m,reg->HL);}
// 0x57
void ld_d_a(Registers* reg) {reg->D = reg->A;}
// 0x58
void ld_e_b(Registers* reg) {reg->E = reg->B;}
// 0x59
void ld_e_c(Registers* reg) {reg->E = reg->C;}
// 0x5A
void ld_e_d(Registers* reg) {reg->E = reg->D;}
// 0x5B
void ld_e_e(Registers* reg) {reg->E = reg->E;}
// 0x5C
void ld_e_h(Registers* reg) {reg->E = reg->H;}
// 0x5D
void ld_e_l(Registers* reg) {reg->E = reg->L;}
// 0x5E
void ld_e_hlp(Memory* m, Registers* reg) {reg->E = read8(m,reg->HL);}
// 0x5F
void ld_e_a(Registers* reg) {reg->E = reg->A;}

// --- 0x6x ---

// 0x60
void ld_h_b(Registers* reg) {reg->H = reg->B;}
// 0x61
void ld_h_c(Registers* reg) {reg->H = reg->C;}
// 0x62
void ld_h_d(Registers* reg) {reg->H = reg->D;}
// 0x63
void ld_h_e(Registers* reg) {reg->H = reg->E;}
// 0x64
void ld_h_h(Registers* reg) {reg->H = reg->H;}
// 0x65
void ld_h_l(Registers* reg) {reg->H = reg->L;}
// 0x66
void ld_h_hlp(Memory* m, Registers* reg) {reg->H = read8(m,reg->HL);}
// 0x67
void ld_h_a(Registers* reg) {reg->H = reg->A;}
// 0x68
void ld_l_b(Registers* reg) {reg->L = reg->B;}
// 0x69
void ld_l_c(Registers* reg) {reg->L = reg->C;}
// 0x6A
void ld_l_d(Registers* reg) {reg->L = reg->D;}
// 0x6B
void ld_l_e(Registers* reg) {reg->L = reg->E;}
// 0x6C
void ld_l_h(Registers* reg) {reg->L = reg->H;}
// 0x6D
void ld_l_l(Registers* reg) {reg->L = reg->L;}
// 0x6E
void ld_l_hlp(Memory* m, Registers* reg) {reg->L = read8(m,reg->HL);}
// 0x6F
void ld_l_a(Registers* reg) {reg->L = reg->A;}

// --- 0x7x ---

// 0x70
void ld_hlp_b(Memory* m, Registers* reg) {write8(m,reg->HL,reg->B);}
// 0x71
void ld_hlp_c(Memory* m, Registers* reg) {write8(m,reg->HL,reg->C);}
// 0x72
void ld_hlp_d(Memory* m, Registers* reg) {write8(m,reg->HL,reg->D);}
// 0x73
void ld_hlp_e(Memory* m, Registers* reg) {write8(m,reg->HL,reg->E);}
// 0x74
void ld_hlp_h(Memory* m, Registers* reg) {write8(m,reg->HL,reg->H);}
// 0x75
void ld_hlp_l(Memory* m, Registers* reg) {write8(m,reg->HL,reg->L);}
// 0x76
void halt(Registers* reg) {reg->halted = 1;}
// 0x77
void ld_hlp_a(Memory* m, Registers* reg) {write8(m,reg->HL,reg->A);}
// 0x78
void ld_a_b(Registers* reg) {reg->A = reg->B;}
// 0x79
void ld_a_c(Registers* reg) {reg->A = reg->C;}
// 0x7A
void ld_a_d(Registers* reg) {reg->A = reg->D;}
// 0x7B
void ld_a_e(Registers* reg) {reg->A = reg->E;}
// 0x7C
void ld_a_h(Registers* reg) {reg->A = reg->H;}
// 0x7D
void ld_a_l(Registers* reg) {reg->A = reg->L;}
// 0x7E
void ld_a_hlp(Memory* m, Registers* reg) {reg->A = read8(m,reg->HL);}
// 0x7F
void ld_a_a(Registers* reg) {reg->A = reg->A;}

// --- 0x8x ---

// 0x80
void add_a_b(Registers* reg) {reg->A = add(reg, reg->A, reg->B);}
// 0x81
void add_a_c(Registers* reg) {reg->A = add(reg, reg->A, reg->C);}
// 0x82
void add_a_d(Registers* reg) {reg->A = add(reg, reg->A, reg->D);}
// 0x83
void add_a_e(Registers* reg) {reg->A = add(reg, reg->A, reg->E);}
// 0x84
void add_a_h(Registers* reg) {reg->A = add(reg, reg->A, reg->H);}
// 0x85
void add_a_l(Registers* reg) {reg->A = add(reg, reg->A, reg->L);}
// 0x86
void add_a_hlp(Registers* reg, Memory* m) {reg->A = add(reg, reg->A, read8(m, reg->HL));}
// 0x87
void add_a_a(Registers* reg) {reg->A = add(reg, reg->A, reg->A);}
// 0x88
void adc_a_b(Registers* reg) {reg->A = adc(reg, reg->A, reg->B);}
// 0x89
void adc_a_c(Registers* reg) {reg->A = adc(reg, reg->A, reg->C);}
// 0x8A
void adc_a_d(Registers* reg) {reg->A = adc(reg, reg->A, reg->D);}
// 0x8B
void adc_a_e(Registers* reg) {reg->A = adc(reg, reg->A, reg->E);}
// 0x8C
void adc_a_h(Registers* reg) {reg->A = adc(reg, reg->A, reg->H);}
// 0x8D
void adc_a_l(Registers* reg) {reg->A = adc(reg, reg->A, reg->L);}
// 0x8E
void adc_a_hlp(Registers* reg, Memory* m) {reg->A = adc(reg, reg->A, read8(m, reg->HL));}
// 0x8F
void adc_a_a(Registers* reg) {reg->A = adc(reg, reg->A, reg->A);}

// --- 0x9x ---

// 0x90
void sub_a_b(Registers* reg) {reg->A = sub(reg, reg->A, reg->B);}
// 0x91
void sub_a_c(Registers* reg) {reg->A = sub(reg, reg->A, reg->C);}
// 0x92
void sub_a_d(Registers* reg) {reg->A = sub(reg, reg->A, reg->D);}
// 0x93
void sub_a_e(Registers* reg) {reg->A = sub(reg, reg->A, reg->E);}
// 0x94
void sub_a_h(Registers* reg) {reg->A = sub(reg, reg->A, reg->H);}
// 0x95
void sub_a_l(Registers* reg) {reg->A = sub(reg, reg->A, reg->L);}
// 0x96
void sub_a_hlp(Registers* reg, Memory* m) {reg->A = sub(reg, reg->A, read8(m, reg->HL));}
// 0x97
void sub_a_a(Registers* reg) {reg->A = sub(reg, reg->A, reg->A);}
// 0x98
void sbc_a_b(Registers* reg) {reg->A = sbc(reg, reg->A, reg->B);}
// 0x99
void sbc_a_c(Registers* reg) {reg->A = sbc(reg, reg->A, reg->C);}
// 0x9A
void sbc_a_d(Registers* reg) {reg->A = sbc(reg, reg->A, reg->D);}
// 0x9B
void sbc_a_e(Registers* reg) {reg->A = sbc(reg, reg->A, reg->E);}
// 0x9C
void sbc_a_h(Registers* reg) {reg->A = sbc(reg, reg->A, reg->H);}
// 0x9D
void sbc_a_l(Registers* reg) {reg->A = sbc(reg, reg->A, reg->L);}
// 0x9E
void sbc_a_hlp(Registers* reg, Memory* m) {reg->A = sbc(reg, reg->A, read8(m, reg->HL));}
// 0x9F
void sbc_a_a(Registers* reg) {reg->A = sbc(reg, reg->A, reg->A);}

// --- 0xAx ---

// 0xA0
void and_b(Registers* reg) {reg->A = cpu_and(reg, reg->B);}
// 0xA1
void and_c(Registers* reg) {reg->A = cpu_and(reg, reg->C);}
// 0xA2
void and_d(Registers* reg) {reg->A = cpu_and(reg, reg->D);}
// 0xA3
void and_e(Registers* reg) {reg->A = cpu_and(reg, reg->E);}
// 0xA4
void and_h(Registers* reg) {reg->A = cpu_and(reg, reg->H);}
// 0xA5
void and_l(Registers* reg) {reg->A = cpu_and(reg, reg->L);}
// 0xA6
void and_hlp(Registers* reg, Memory* m) {reg->A = cpu_and(reg, read8(m, reg->HL));}
// 0xA7
void and_a(Registers* reg) {reg->A = cpu_and(reg, reg->A);}
// 0xA8
void xor_b(Registers* reg) {reg->A = cpu_xor(reg, reg->B);}
// 0xA9
void xor_c(Registers* reg) {reg->A = cpu_xor(reg, reg->C);}
// 0xAA
void xor_d(Registers* reg) {reg->A = cpu_xor(reg, reg->D);}
// 0xAB
void xor_e(Registers* reg) {reg->A = cpu_xor(reg, reg->E);}
// 0xAC
void xor_h(Registers* reg) {reg->A = cpu_xor(reg, reg->H);}
// 0xAD
void xor_l(Registers* reg) {reg->A = cpu_xor(reg, reg->L);}
// 0xAE
void xor_hlp(Registers* reg, Memory* m) {reg->A = cpu_xor(reg, read8(m, reg->HL));}
// 0xAF
void xor_a(Registers* reg) {reg->A = cpu_xor(reg, reg->A);}

// --- 0xBx ---

// 0xB0
void or_b(Registers* reg) {reg->A = cpu_or(reg, reg->B);}
// 0xB1
void or_c(Registers* reg) {reg->A = cpu_or(reg, reg->C);}
// 0xB2
void or_d(Registers* reg) {reg->A = cpu_or(reg, reg->D);}
// 0xB3
void or_e(Registers* reg) {reg->A = cpu_or(reg, reg->E);}
// 0xB4
void or_h(Registers* reg) {reg->A = cpu_or(reg, reg->H);}
// 0xB5
void or_l(Registers* reg) {reg->A = cpu_or(reg, reg->L);}
// 0xB6
void or_hlp(Registers* reg, Memory* m) {reg->A = cpu_or(reg, read8(m, reg->HL));}
// 0xB7
void or_a(Registers* reg) {reg->A = cpu_or(reg, reg->A);}
// 0xB8
void cp_b(Registers* reg) {cpu_cp(reg,reg->B);}
// 0xB9
void cp_c(Registers* reg) {cpu_cp(reg,reg->C);}
// 0xBA
void cp_d(Registers* reg) {cpu_cp(reg,reg->D);}
// 0xBB
void cp_e(Registers* reg) {cpu_cp(reg,reg->E);}
// 0xBC
void cp_h(Registers* reg) {cpu_cp(reg,reg->H);}
// 0xBD
void cp_l(Registers* reg) {cpu_cp(reg,reg->L);}
// 0xBE
void cp_hlp(Registers* reg, Memory* m) {cpu_cp(reg,read8(m, reg->HL));}
// 0xBF
void cp_a(Registers* reg) {cpu_cp(reg,reg->A);}

// --- 0xCx ---

// 0xC0
void ret_nz(Registers* reg, Memory* m, int* cycles) {
    if (!get_flag_z(reg)) {
        reg->PC = pop(reg, m);
        *cycles += 12;
    }
}
// 0xC1
void pop_bc(Registers* reg, Memory* m) {reg->BC = pop(reg, m);}
// 0xC2
void jp_nz_xx(Registers* reg, uint16_t operand, int* cycles) {
    if (!get_flag_z(reg)) {
        reg->PC = operand;
        *cycles += 4;
    }
}
// 0xC3
void jp_xx(Registers* reg, Memory* m, uint16_t operand) {reg->PC = operand;}
// 0xC4
void call_nz_xx(Registers* reg, Memory* m, uint16_t operand, int* cycles) {
    if (!get_flag_z(reg)) {
        push(reg, m, reg->PC+3);
        reg->PC = operand;
        *cycles += 12;
    }
}
// 0xC5
void push_bc(Registers* reg, Memory* m) {push(reg, m, reg->BC);}
// 0xC6
void add_a_x(Registers* reg, uint8_t operand) {reg->A = add(reg, reg->A, operand);}
// 0xC7
void rst_0(Registers* reg, Memory* m) { rst(reg, m, 0x00); }
// 0xC8
void ret_z(Registers* reg, Memory* m, int* cycles) {
    if (get_flag_z(reg)) {
        reg->PC = pop(reg, m);
        *cycles += 12;
    }
}
// 0xC9
void ret(Registers* reg, Memory* m) {reg->PC = pop(reg, m);}
// 0xCA
void jp_z_xx(Registers* reg, uint16_t operand) {if (get_flag_z(reg)) reg->PC = operand;}

// 0xCB
/* 
cb.c
Handles CB subset of operations
*/
void cb(Registers* reg, Memory* m, uint8_t operand, int* cycles) {
	execute_cb(operand, reg, m, cycles); // execute operands cb instruction
}

// 0xCC
void call_z_xx(Registers* reg, Memory* m, uint16_t operand, int* cycles) {
    if (get_flag_z(reg)) {
        push(reg, m, reg->PC+3);
        reg->PC = operand;
        *cycles += 12;
    }
}
// 0xCD
void call_xx(Registers* reg, Memory* m, uint16_t operand) {
    push(reg, m, reg->PC+3);
    reg->PC = operand;
}
// 0xCE
void adc_a_x(Registers* reg, uint8_t operand) {reg->A = adc(reg, reg->A, operand);}
// 0xCF
void rst_08(Registers* reg, Memory* m) { rst(reg, m, 0x08); }

// --- 0xDx ---

// 0xD0
void ret_nc(Registers* reg, Memory* m, int* cycles) {
    if (!get_flag_c(reg)) {
        reg->PC = pop(reg, m);
        *cycles += 12;
    }
}
// 0xD1
void pop_de(Registers* reg, Memory* m) {reg->DE = pop(reg, m);}
// 0xD2
void jp_nc_xx(Registers* reg, uint16_t operand, int* cycles) {
    if (!get_flag_c(reg)) {
        reg->PC = operand;
        *cycles += 4;
    }
}
// 0xD4
void call_nc_xx(Registers* reg, Memory* m, uint16_t operand, int* cycles) {
    if (!get_flag_c(reg)) {
        push(reg, m, reg->PC+3);
        reg->PC = operand;
        *cycles += 12;
    }
}
// 0xD5
void push_de(Registers* reg, Memory* m) {push(reg, m, reg->DE);}
// 0xD6
void sub_x(Registers* reg, uint8_t operand) {reg->A = sub(reg, reg->A, operand);}
// 0xD7
void rst_10(Registers* reg, Memory* m) { rst(reg, m, 0x10); }
// 0xD8
void ret_c(Registers* reg, Memory* m, int* cycles) {
    if (get_flag_c(reg)) {
        reg->PC = pop(reg, m);
        *cycles += 12;
    }
}
// 0xD9
void reti(Registers* reg, Memory* m) {
    reg->PC = pop(reg, m);
    reg->IME = 1;
}
// 0xDA
void jp_c_xx(Registers* reg, uint16_t operand) {if (get_flag_c(reg)) reg->PC = operand;}
// 0xDC
void call_c_xx(Registers* reg, Memory* m, uint16_t operand, int* cycles) {
    if (get_flag_c(reg)) {
        push(reg, m, reg->PC+3);
        reg->PC = operand;
        *cycles += 12;
    }
}
// 0xDE
void sbc_a_x(Registers* reg, uint8_t operand) {reg->A = sbc(reg, reg->A, operand);}
// 0xDF
void rst_18(Registers* reg, Memory* m) { rst(reg, m, 0x18); }

// --- 0xEx ---

// 0xE0
void ldh_x_a(Registers* reg, Memory* m, uint8_t operand) {write8(m, 0xFF00 + operand, reg->A);}
// 0xE1
void pop_hl(Registers* reg, Memory* m) {reg->HL = pop(reg, m);}
// 0xE2
void ld_cp_a(Registers* reg, Memory* m) {write8(m, 0xFF00 + reg->C, reg->A);}
// 0xE5
void push_hl(Registers* reg, Memory* m) {push(reg, m, reg->HL);}
// 0xE6
void and_x(Registers* reg, uint8_t operand) {reg->A = cpu_and(reg, operand);}
// 0xE7
void rst_20(Registers* reg, Memory* m) { rst(reg, m, 0x20); }
// 0xE8
void add_sp_x(Registers* reg, uint8_t operand) {
	// in theory - reg->SP = reg->SP + (int8_t)operand
	// need to set flags to carry bits from low byte
	
    int8_t offset = (int8_t)operand;

    uint16_t sp = reg->SP;
    uint16_t result = sp + offset;

    set_flag_z(reg, 0);
    set_flag_n(reg, 0);
    set_flag_h(reg, ((sp & 0x0F) + (operand & 0x0F)) > 0x0F);
    set_flag_c(reg, ((sp & 0xFF) + operand) > 0xFF);

    reg->SP = result;
}
// 0xE9
void jp_hlp(Registers* reg) {reg->PC = reg->HL;}
// 0xEA
void ld_xx_a(Registers* reg, Memory* m, uint16_t operand) {write8(m, operand, reg->A);}
// 0xEE
void xor_x(Registers* reg, uint8_t operand) {reg->A = cpu_xor(reg, operand);}
// 0xEF
void rst_28(Registers* reg, Memory* m) { rst(reg, m, 0x28); }

// --- 0xFx ---

// 0xF0
void ldh_a_xp(Registers* reg, Memory* m, uint8_t operand) {reg->A = read8(m, 0xFF00 + operand);}
// 0xF1
void pop_af(Registers* reg, Memory* m) {reg->AF = pop(reg, m);}
// 0xF2
void ld_a_cp(Registers* reg, Memory* m) {reg->A = read8(m, 0xFF00 + reg->C);}
// 0xF3
void di(Registers* reg) {reg->IME = 0; reg->IME_delay = 0;}
// 0xF5
void push_af(Registers* reg, Memory* m) {push(reg, m, reg->AF);}
// 0xF6
void or_x(Registers* reg, uint8_t operand) {reg->A = cpu_or(reg, operand);}
// 0xF7
void rst_30(Registers* reg, Memory* m) { rst(reg, m, 0x30); }
// 0xF8
void ld_hl_sp_x(Registers* reg, uint8_t operand) {
	// in theory - reg->HL = reg->SP + (int8_t)operand
	// carries similarly to 0xE8
	
    int8_t offset = (int8_t)operand;

    uint16_t sp = reg->SP;
    uint16_t result = sp + offset;

    set_flag_z(reg, 0);
    set_flag_n(reg, 0);
    set_flag_h(reg, ((sp & 0x0F) + (operand & 0x0F)) > 0x0F);
    set_flag_c(reg, ((sp & 0xFF) + operand) > 0xFF);

    reg->HL = result;
}
// 0xF9
void ld_sp_hl(Registers* reg) {reg->SP = reg->HL;}
// 0xFA
void ld_a_xp(Registers* reg, Memory* m, uint16_t operand) {reg->A = read8(m, operand);}
// 0xFB
void ei(Registers* reg) {reg->IME_delay = 2;}
// 0xFE
void cp_x(Registers* reg, uint8_t operand) {cpu_cp(reg,operand);}
// 0xFF
void rst_38(Registers* reg, Memory* m) { rst(reg, m, 0x38); }

void cpu_init(Registers* reg) {
	
	reg->BC = 0;
	reg->DE = 0;
	reg->HL = 0;
	reg->AF = 0;
	reg->PC = 0;
	reg->SP = 0xFFFE; // stack starts at top of memory
	reg->IME = 0;
	reg->IME_delay = 0;
	reg->halted = 0;
	
}

void service_interrupt(Registers* reg, Memory* m) {

    uint8_t ie = read8(m, 0xFFFF);
    uint8_t iflag = read8(m, 0xFF0F);
    uint8_t pending = ie & iflag;

    if (!pending) return;

    reg->IME = 0;

    for (int i = 0; i < 5; i++) {
        if (pending & (1 << i)) {
            write8(m, 0xFF0F, iflag & ~(1 << i));
            push(reg, m, reg->PC); 
            reg->PC = 0x40 + i * 8; // jump to interrupt
            return;
        }
    }
}

int cycle(Memory* m, Registers* reg){
	
// step 1: read opcode and operands from memory

	uint16_t reg_pc = reg->PC; // backup of PC to detect jumps

	uint8_t opcode = read8(m, reg->PC);
	uint8_t operand_x = read8(m, reg->PC + 1);
	uint16_t operand_xx = read8(m, reg->PC + 1) + (read8(m, reg->PC + 2) << 8);
	
	if (PRINT_CYCLE){
		if (SHOW_INSTRUCTION) printf("0x%02x | ", opcode);
		if (SHOW_OPERANDS) printf("0x%02x | 0x%04x | ", operand_x,operand_xx);
		if (SHOW_CPU) printf("PC:0x%02x | SP:0x%04x | ",reg->PC, reg->SP);
		if (SHOW_IME) printf("%-1d   | %-1d         | ",reg->IME, reg->IME_delay);
		if (SHOW_REGISTERS) {printf(
			"AF:0x%04x | BC:0x%04x | "
			"DE:0x%04x | HL:0x%04x | ",
			reg->AF,
			reg->BC,
			reg->DE,
			reg->HL
		);}
	}
	
	int cycles = cycle_lengths[opcode];
	
// step 2: handle halt and interrupts
	
	// interrupt handling

	if (reg->IME) {
		uint8_t pending = read8(m, 0xFF0F) & read8(m, 0xFFFF);
		
		if (SHOW_IME) printf("INTERRUPT");
		
		if (pending) { // only calculate pending if IME is on
			int c = 20;
			if (reg->halted) {
				reg->halted = 0;
				c += 4;
			}
			service_interrupt(reg, m);
			return c;
		}
	}
	
// step 3: decode and execute instruction
	
	/*
	x: 1 byte parameter
	xx: 2 byte parameter
	*p: (p)
	
	ldd: ld then decrement
	ldi: ld then increment
	
	z
	nc: carry flag is 0
	c: carry flag is 1
	*/
	
	switch (opcode) {
		case (0x00):nop(reg); break; // 0x00 
		case (0x01):ld_bc_xx(reg, operand_xx); break; // 0x01 
		case (0x02):ld_bcp_a(reg, m); break; // 0x02
		case (0x03):inc_bc(reg); break; // 0x03
		case (0x04):inc_b(reg); break; // 0x04
		case (0x05):dec_b(reg); break; // 0x05
		case (0x06):ld_b_x(reg, operand_x); break; // 0x06
		case (0x07):rlca(reg); break; // 0x07
		case (0x08):ld_xxp_sp(reg,m,operand_xx); break; // 0x08
		case (0x09):add_hl_bc(reg); break; // 0x09
		case (0x0A):ld_a_bcp(reg, m); break; // 0x0A
		case (0x0B):dec_bc(reg); break; // 0x0B
		case (0x0C):inc_c(reg); break; // 0x0C
		case (0x0D):dec_c(reg); break; // 0x0D
		case (0x0E):ld_c_x(reg, operand_x); break; // 0x0E
		case (0x0F):rrca(reg); break; // 0x0F
		
		case (0x10):stop(reg); break; // 0x10
		case (0x11):ld_de_xx(reg, operand_xx); break; // 0x11
		case (0x12):ld_dep_a(reg, m); break; // 0x12
		case (0x13):inc_de(reg); break; // 0x13
		case (0x14):inc_d(reg); break; // 0x14
		case (0x15):dec_d(reg); break; // 0x15
		case (0x16):ld_d_x(reg, operand_x); break; // 0x16
		case (0x17):rla(reg); break; // 0x17
		case (0x18):jr_x(reg, operand_x); break; // 0x18
		case (0x19):add_hl_de(reg); break; // 0x19
		case (0x1A):ld_a_dep(reg,m); break; // 0x1A
		case (0x1B):dec_de(reg); break; // 0x1B
		case (0x1C):inc_e(reg); break; // 0x1C
		case (0x1D):dec_e(reg); break; // 0x1D
		case (0x1E):ld_e_x(reg, operand_x); break; // 0x1E
		case (0x1F):rra(reg); break; // 0x1F
		
		case (0x20):jr_nz_x(reg, operand_x, &cycles); break; // 0x20
		case (0x21):ld_hl_xx(reg, operand_xx); break; // 0x21
		case (0x22):ldi_hlp_a(reg, m); break; // 0x22
		case (0x23):inc_hl(reg); break; // 0x23
		case (0x24):inc_h(reg); break; // 0x24
		case (0x25):dec_h(reg); break; // 0x25
		case (0x26):ld_h_x(reg, operand_x); break; // 0x26
		case (0x27):daa(reg); break; // 0x27
		case (0x28):jr_z_x(reg, operand_x, &cycles); break; // 0x28
		case (0x29):add_hl_hl(reg); break; // 0x29
		case (0x2A):ldi_a_hlp(reg, m); break; // 0x2A
		case (0x2B):dec_hl(reg); break; // 0x2B
		case (0x2C):inc_l(reg); break; // 0x2C
		case (0x2D):dec_l(reg); break; // 0x2D
		case (0x2E):ld_l_x(reg, operand_x); break; // 0x2E
		case (0x2F):cpl(reg); break; // 0x2F
		
		case (0x30):jr_nc_x(reg, operand_x, &cycles); break; // 0x30
		case (0x31):ld_sp_xx(reg, operand_xx); break; // 0x31
		case (0x32):ldd_hlp_a(reg, m); break; // 0x32
		case (0x33):inc_sp(reg); break; // 0x33
		case (0x34):inc_hlp(reg, m); break; // 0x34
		case (0x35):dec_hlp(reg, m); break; // 0x35
		case (0x36):ld_hlp_x(reg, m, operand_x); break; // 0x36
		case (0x37):scf(reg); break; // 0x37
		case (0x38):jr_c_x(reg, operand_x, &cycles); break; // 0x38
		case (0x39):add_hl_sp(reg); break; // 0x39
		case (0x3A):ldd_a_hlp(reg, m); break; // 0x3A
		case (0x3B):dec_sp(reg); break; // 0x3B
		case (0x3C):inc_a(reg); break; // 0x3C
		case (0x3D):dec_a(reg); break; // 0x3D
		case (0x3E):ld_a_x(reg, operand_x); break; // 0x3E
		case (0x3F):ccf(reg); break; // 0x3F
		
		case (0x40):ld_b_b(reg); break; // 0x40
		case (0x41):ld_b_c(reg); break; // 0x41
		case (0x42):ld_b_d(reg); break; // 0x42
		case (0x43):ld_b_e(reg); break; // 0x43
		case (0x44):ld_b_h(reg); break; // 0x44
		case (0x45):ld_b_l(reg); break; // 0x45
		case (0x46):ld_b_hlp(m,reg); break; // 0x46
		case (0x47):ld_b_a(reg); break; // 0x47
		case (0x48):ld_c_b(reg); break; // 0x48
		case (0x49):ld_c_c(reg); break; // 0x49
		case (0x4A):ld_c_d(reg); break; // 0x4A
		case (0x4B):ld_c_e(reg); break; // 0x4B
		case (0x4C):ld_c_h(reg); break; // 0x4C
		case (0x4D):ld_c_l(reg); break; // 0x4D
		case (0x4E):ld_c_hlp(m,reg); break; // 0x4E
		case (0x4F):ld_c_a(reg); break; // 0x4F
		
		case (0x50):ld_d_b(reg); break; // 0x50
		case (0x51):ld_d_c(reg); break; // 0x51
		case (0x52):ld_d_d(reg); break; // 0x52
		case (0x53):ld_d_e(reg); break; // 0x53
		case (0x54):ld_d_h(reg); break; // 0x54
		case (0x55):ld_d_l(reg); break; // 0x55
		case (0x56):ld_d_hlp(m,reg); break; // 0x56
		case (0x57):ld_d_a(reg); break; // 0x57
		case (0x58):ld_e_b(reg); break; // 0x58
		case (0x59):ld_e_c(reg); break; // 0x59
		case (0x5A):ld_e_d(reg); break; // 0x5A
		case (0x5B):ld_e_e(reg); break; // 0x5B
		case (0x5C):ld_e_h(reg); break; // 0x5C
		case (0x5D):ld_e_l(reg); break; // 0x5D
		case (0x5E):ld_e_hlp(m,reg); break; // 0x5E
		case (0x5F):ld_e_a(reg); break; // 0x5F
		
		case (0x60):ld_h_b(reg); break; // 0x60
		case (0x61):ld_h_c(reg); break; // 0x61
		case (0x62):ld_h_d(reg); break; // 0x62
		case (0x63):ld_h_e(reg); break; // 0x63
		case (0x64):ld_h_h(reg); break; // 0x64
		case (0x65):ld_h_l(reg); break; // 0x65
		case (0x66):ld_h_hlp(m,reg); break; // 0x66
		case (0x67):ld_h_a(reg); break; // 0x67
		case (0x68):ld_l_b(reg); break; // 0x68
		case (0x69):ld_l_c(reg); break; // 0x69
		case (0x6A):ld_l_d(reg); break; // 0x6A
		case (0x6B):ld_l_e(reg); break; // 0x6B
		case (0x6C):ld_l_h(reg); break; // 0x6C
		case (0x6D):ld_l_l(reg); break; // 0x6D
		case (0x6E):ld_l_hlp(m,reg); break; // 0x6E
		case (0x6F):ld_l_a(reg); break; // 0x6F
		
		case (0x70):ld_hlp_b(m,reg); break; // 0x70
		case (0x71):ld_hlp_c(m,reg); break; // 0x71
		case (0x72):ld_hlp_d(m,reg); break; // 0x72
		case (0x73):ld_hlp_e(m,reg); break; // 0x73
		case (0x74):ld_hlp_h(m,reg); break; // 0x74
		case (0x75):ld_hlp_l(m,reg); break; // 0x75
		case (0x76):halt(reg); break; // 0x76
		case (0x77):ld_hlp_a(m,reg); break; // 0x77
		case (0x78):ld_a_b(reg); break; // 0x78
		case (0x79):ld_a_c(reg); break; // 0x79
		case (0x7A):ld_a_d(reg); break; // 0x7A
		case (0x7B):ld_a_e(reg); break; // 0x7B
		case (0x7C):ld_a_h(reg); break; // 0x7C
		case (0x7D):ld_a_l(reg); break; // 0x7D
		case (0x7E):ld_a_hlp(m,reg); break; // 0x7E
		case (0x7F):ld_a_a(reg); break; // 0x7F
		
		case (0x80):add_a_b(reg); break; // 0x80
		case (0x81):add_a_c(reg); break; // 0x81
		case (0x82):add_a_d(reg); break; // 0x82
		case (0x83):add_a_e(reg); break; // 0x83
		case (0x84):add_a_h(reg); break; // 0x84
		case (0x85):add_a_l(reg); break; // 0x85
		case (0x86):add_a_hlp(reg,m); break; // 0x86
		case (0x87):add_a_a(reg); break; // 0x87
		case (0x88):adc_a_b(reg); break; // 0x88
		case (0x89):adc_a_c(reg); break; // 0x89
		case (0x8A):adc_a_d(reg); break; // 0x8A
		case (0x8B):adc_a_e(reg); break; // 0x8B
		case (0x8C):adc_a_h(reg); break; // 0x8C
		case (0x8D):adc_a_l(reg); break; // 0x8D
		case (0x8E):adc_a_hlp(reg,m); break; // 0x8E
		case (0x8F):adc_a_a(reg); break; // 0x8F
		
		case (0x90):sub_a_b(reg); break; // 0x90
		case (0x91):sub_a_c(reg); break; // 0x91
		case (0x92):sub_a_d(reg); break; // 0x92
		case (0x93):sub_a_e(reg); break; // 0x93
		case (0x94):sub_a_h(reg); break; // 0x94
		case (0x95):sub_a_l(reg); break; // 0x95
		case (0x96):sub_a_hlp(reg,m); break; // 0x96
		case (0x97):sub_a_a(reg); break; // 0x97
		case (0x98):sbc_a_b(reg); break; // 0x98
		case (0x99):sbc_a_c(reg); break; // 0x99
		case (0x9A):sbc_a_d(reg); break; // 0x9A
		case (0x9B):sbc_a_e(reg); break; // 0x9B
		case (0x9C):sbc_a_h(reg); break; // 0x9C
		case (0x9D):sbc_a_l(reg); break; // 0x9D
		case (0x9E):sbc_a_hlp(reg,m); break; // 0x9E
		case (0x9F):sbc_a_a(reg); break; // 0x9F
		
		case (0xA0):and_b(reg); break; // 0xA0
		case (0xA1):and_c(reg); break; // 0xA1
		case (0xA2):and_d(reg); break; // 0xA2
		case (0xA3):and_e(reg); break; // 0xA3
		case (0xA4):and_h(reg); break; // 0xA4
		case (0xA5):and_l(reg); break; // 0xA5
		case (0xA6):and_hlp(reg,m); break; // 0xA6
		case (0xA7):and_a(reg); break; // 0xA7
		case (0xA8):xor_b(reg); break; // 0xA8
		case (0xA9):xor_c(reg); break; // 0xA9
		case (0xAA):xor_d(reg); break; // 0xAA
		case (0xAB):xor_e(reg); break; // 0xAB
		case (0xAC):xor_h(reg); break; // 0xAC
		case (0xAD):xor_l(reg); break; // 0xAD
		case (0xAE):xor_hlp(reg,m); break; // 0xAE
		case (0xAF):xor_a(reg); break; // 0xAF
		
		case (0xB0):or_b(reg); break; // 0xB0
		case (0xB1):or_c(reg); break; // 0xB1
		case (0xB2):or_d(reg); break; // 0xB2
		case (0xB3):or_e(reg); break; // 0xB3
		case (0xB4):or_h(reg); break; // 0xB4
		case (0xB5):or_l(reg); break; // 0xB5
		case (0xB6):or_hlp(reg,m); break; // 0xB6
		case (0xB7):or_a(reg); break; // 0xB7
		case (0xB8):cp_b(reg); break; // 0xB8
		case (0xB9):cp_c(reg); break; // 0xB9
		case (0xBA):cp_d(reg); break; // 0xBA
		case (0xBB):cp_e(reg); break; // 0xBB
		case (0xBC):cp_h(reg); break; // 0xBC
		case (0xBD):cp_l(reg); break; // 0xBD
		case (0xBE):cp_hlp(reg,m); break; // 0xBE
		case (0xBF):cp_a(reg); break; // 0xBF
		
		case (0xC0):ret_nz(reg,m,&cycles); break; // 0xC0
		case (0xC1):pop_bc(reg,m); break; // 0xC1
		case (0xC2):jp_nz_xx(reg, operand_xx, &cycles); break; // 0xC2
		case (0xC3):jp_xx(reg,m,operand_xx); break; // 0xC3
		case (0xC4):call_nz_xx(reg,m,operand_xx,&cycles); break; // 0xC4
		case (0xC5):push_bc(reg, m); break; // 0xC5
		case (0xC6):add_a_x(reg, operand_x); break; // 0xC6
		case (0xC7):rst_0(reg,m); break; // 0xC7
		case (0xC8):ret_z(reg,m,&cycles); break; // 0xC8
		case (0xC9):ret(reg,m); break; // 0xC9
		case (0xCA):jp_z_xx(reg, operand_xx); break; // 0xCA
		case (0xCB):cb(reg, m, operand_x, &cycles); break; // 0xCB
		case (0xCC):call_z_xx(reg,m,operand_xx,&cycles); break; // 0xCC
		case (0xCD):call_xx(reg,m,operand_xx); break; // 0xCD
		case (0xCE):adc_a_x(reg, operand_x); break; // 0xCE
		case (0xCF):rst_08(reg,m); break; // 0xCF
		
		case (0xD0):ret_nc(reg,m,&cycles); break; // 0xD0
		case (0xD1):pop_de(reg,m); break; // 0xD1
		case (0xD2):jp_nc_xx(reg, operand_xx, &cycles); break; // 0xD2
		case (0xD3):nop(reg); break; // 0xD3 blank
		case (0xD4):call_nc_xx(reg,m,operand_xx,&cycles); break; // 0xD4
		case (0xD5):push_de(reg, m); break; // 0xD5
		case (0xD6):sub_x(reg, operand_x); break; // 0xD6
		case (0xD7):rst_10(reg,m); break; // 0xD7
		case (0xD8):ret_c(reg,m,&cycles); break; // 0xD8
		case (0xD9):reti(reg,m); break; // 0xD9
		case (0xDA):jp_c_xx(reg, operand_xx); break; // 0xDA
		case (0xDB):nop(reg); break; // 0xDB blank
		case (0xDC):call_c_xx(reg,m,operand_xx,&cycles); break; // 0xDC
		case (0xDD):nop(reg); break; // 0xDD blank
		case (0xDE):sbc_a_x(reg, operand_x); break; // 0xDE
		case (0xDF):rst_18(reg,m); break; // 0xDF
		
		case (0xE0):ldh_x_a(reg, m, operand_x); break; // 0xE0
		case (0xE1):pop_hl(reg,m); break; // 0xE1
		case (0xE2):ld_cp_a(reg, m); break; // 0xE2
		case (0xE3):nop(reg); break; // 0xE3 blank
		case (0xE4):nop(reg); break; // 0xE4 blank
		case (0xE5):push_hl(reg, m); break; // 0xE5
		case (0xE6):and_x(reg, operand_x); break; // 0xE6
		case (0xE7):rst_20(reg,m); break; // 0xE7
		case (0xE8):add_sp_x(reg,operand_x); break; // 0xE8
		case (0xE9):jp_hlp(reg); break; // 0xE9
		case (0xEA):ld_xx_a(reg,m,operand_x); break; // 0xEA
		case (0xEB):nop(reg); break; // 0xEB blank
		case (0xEC):nop(reg); break; // 0xEC blank
		case (0xED):nop(reg); break; // 0xED blank
		case (0xEE):xor_x(reg, operand_x); break; // 0xEE
		case (0xEF):rst_28(reg,m); break; // 0xEF
		
		case (0xF0):ldh_a_xp(reg, m, operand_x); break; // 0xF0
		case (0xF1):pop_af(reg,m); break; // 0xF1
		case (0xF2):ld_a_cp(reg, m); break; // 0xF2
		case (0xF3):di(reg); break; // 0xF3
		case (0xF4):nop(reg); break; // 0xF4 blank
		case (0xF5):push_af(reg, m); break; // 0xF5
		case (0xF6):or_x(reg, operand_x); break; // 0xF6
		case (0xF7):rst_30(reg,m); break; // 0xF7
		case (0xF8):ld_hl_sp_x(reg,operand_x); break; // 0xF8
		case (0xF9):ld_sp_hl(reg); break; // 0xF9
		case (0xFA):ld_a_xp(reg, m, operand_xx); break; // 0xFA
		case (0xFB):ei(reg); break; // 0xFB
		case (0xFC):nop(reg); break; // 0xFC blank
		case (0xFD):nop(reg); break; // 0xFD blank
		case (0xFE):cp_x(reg, operand_x); break; // 0xFE
		case (0xFF):rst_38(reg,m); break; // 0xFF
	}

// step 4: increment PC, next instruction
	
	if (reg->PC == reg_pc)
		reg->PC += operand_lengths[opcode]+1;
	
// step 5: handle interrupt delay
	if (reg->IME_delay > 0 && !reg->halted) {
		reg->IME_delay--;
		if (!reg->IME_delay)
			reg->IME = 1;
	}
	
	
	return cycles;
}
	
