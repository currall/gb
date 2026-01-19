#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "cpu.h"
#include "memory.h"
#include "registers.h"
#include "rom.h"

// minimum lengths of each instructions in cpu cycles

int cb_cycle_lengths[256] = {
//  0  1  2  3  4  5   6  7  8  9  A  B  C  D   E  F
	8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 0x0_
	8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 0x1_
	8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 0x2_
	8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 0x3_
	8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8, // 0x4_
	8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8, // 0x5_
	8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8, // 0x6_
	8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8, // 0x7_
	8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8, // 0x8_
	8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8, // 0x9_
	8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8, // 0xA_
	8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8, // 0xB_
	8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8, // 0xC_
	8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8, // 0xD_
	8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8, // 0xE_
	8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8  // 0xF_
};

uint8_t rlc(Registers* reg, uint8_t value) {
	int carry = (value & 0x80) >> 7;
	
	if(value & 0x80) set_flag_c(reg, 1);
	else set_flag_c(reg, 0);
	
	value <<= 1;
	value += carry;
	
	if(value) set_flag_z(reg, 0);
	else set_flag_z(reg, 1);
	
	set_flag_n(reg, 0);
	set_flag_h(reg, 0);
	
	return value;
}
uint8_t rrc(Registers* reg, uint8_t value) {
	int carry = value & 0x01;
	
	value >>= 1;
	
	if(carry) {
		set_flag_c(reg, 1);
		value |= 0x80;
	}	else set_flag_c(reg, 0);
	
	if(value) set_flag_z(reg, 0);
	else set_flag_z(reg, 1);
	
	set_flag_n(reg, 0);
	set_flag_h(reg, 0);
	
	return value;
}
uint8_t rl(Registers* reg, uint8_t value) {
	int carry = get_flag_c(reg) ? 1 : 0;
	
	if(value & 0x80) set_flag_c(reg, 1);
	else set_flag_c(reg, 0);
	
	value <<= 1;
	value += carry;
	
	if(value) set_flag_z(reg, 0);
	else set_flag_z(reg, 1);
	
	set_flag_n(reg, 0);
	set_flag_h(reg, 0);
	
	return value;
}
uint8_t rr(Registers* reg, uint8_t value) {
	value >>= 1;
	if(get_flag_c(reg)) value |= 0x80;
	
	if(value & 0x01) set_flag_c(reg, 1);
	else set_flag_c(reg, 0);
	
	if(value) set_flag_z(reg, 0);
	else set_flag_z(reg, 1);
	
	set_flag_n(reg, 0);
	set_flag_h(reg, 0);
	
	return value;
}
uint8_t sla(Registers* reg, uint8_t value) {
	if(value & 0x80) set_flag_c(reg, 1);
	else set_flag_c(reg, 0);
	
	value <<= 1;
	
	if(value) set_flag_z(reg, 0);
	else set_flag_z(reg, 1);
	
	set_flag_n(reg, 0);
	set_flag_h(reg, 0);
	
	return value;
}
uint8_t sra(Registers* reg, uint8_t value) {
	if(value & 0x01) set_flag_c(reg, 1);
	else set_flag_c(reg, 0);
	
	value = (value & 0x80) | (value >> 1);
	
	if(value) set_flag_z(reg, 0);
	else set_flag_z(reg, 1);
	
	set_flag_n(reg, 0);
	set_flag_h(reg, 0);
	
	return value;
}
uint8_t swap(Registers* reg, uint8_t value) {
	value = ((value & 0xf) << 4) | ((value & 0xf0) >> 4);
	
	if(value) set_flag_z(reg, 0);
	else set_flag_z(reg, 1);
	
	set_flag_n(reg, 0);
	set_flag_h(reg, 0);
	set_flag_c(reg, 0);
	
	return value;
}
uint8_t srl(Registers* reg, uint8_t value) {
	if(value & 0x01) set_flag_c(reg, 1);
	else set_flag_c(reg, 0);
	
	value >>= 1;
	
	if(value) set_flag_z(reg, 0);
	else set_flag_z(reg, 1);
	
	set_flag_n(reg, 0);
	set_flag_h(reg, 0);
	
	return value;
}
void bit(Registers* reg, uint8_t bit, uint8_t value) {
	if(value & bit) set_flag_z(reg, 0);
	else set_flag_z(reg, 1);
	
	set_flag_n(reg, 0);
	set_flag_h(reg, 1);
}
uint8_t set(Registers* reg, uint8_t bit, uint8_t value) {
	value |= bit;
	return value;
}
// 0x00
void rlc_b(Registers* reg) {reg->B = rlc(reg, reg->B);}
// 0x01
void rlc_c(Registers* reg) {reg->C = rlc(reg, reg->C);}
// 0x02
void rlc_d(Registers* reg) {reg->D = rlc(reg, reg->D);}
// 0x03
void rlc_e(Registers* reg) {reg->E = rlc(reg, reg->E);}
// 0x04
void rlc_h(Registers* reg) {reg->H = rlc(reg, reg->H);}
// 0x05
void rlc_l(Registers* reg) {reg->L = rlc(reg, reg->L);}
// 0x06
void rlc_hlp(Registers* reg, Memory* m) {write8(m, reg->HL, rlc(reg, read8(m, reg->HL)));}
// 0x07
void rlc_a(Registers* reg) {reg->A = rlc(reg, reg->A);}
// 0x08
void rrc_b(Registers* reg) {reg->B = rrc(reg, reg->B);}
// 0x09
void rrc_c(Registers* reg) {reg->C = rrc(reg, reg->C);}
// 0x0A
void rrc_d(Registers* reg) {reg->D = rrc(reg, reg->D);}
// 0x0B
void rrc_e(Registers* reg) {reg->E = rrc(reg, reg->E);}
// 0x0C
void rrc_h(Registers* reg) {reg->H = rrc(reg, reg->H);}
// 0x0D
void rrc_l(Registers* reg) {reg->L = rrc(reg, reg->L);}
// 0x0E
void rrc_hlp(Registers* reg, Memory* m) {write8(m, reg->HL, rrc(reg, read8(m, reg->HL)));}
// 0x0F
void rrc_a(Registers* reg) {reg->A = rrc(reg, reg->A);}
// 0x10
void rl_b(Registers* reg) {reg->B = rl(reg, reg->B);}
// 0x11
void rl_c(Registers* reg) {reg->C = rl(reg, reg->C);}
// 0x12
void rl_d(Registers* reg) {reg->D = rl(reg, reg->D);}
// 0x13
void rl_e(Registers* reg) {reg->E = rl(reg, reg->E);}
// 0x14
void rl_h(Registers* reg) {reg->H = rl(reg, reg->H);}
// 0x15
void rl_l(Registers* reg) {reg->L = rl(reg, reg->L);}
// 0x16
void rl_hlp(Registers* reg, Memory* m) {write8(m, reg->HL, rl(reg, read8(m, reg->HL)));}
// 0x17
void rl_a(Registers* reg) {reg->A = rl(reg, reg->A);}
// 0x18
void rr_b(Registers* reg) {reg->B = rr(reg, reg->B);}
// 0x19
void rr_c(Registers* reg) {reg->C = rr(reg, reg->C);}
// 0x1A
void rr_d(Registers* reg) {reg->D = rr(reg, reg->D);}
// 0x1B
void rr_e(Registers* reg) {reg->E = rr(reg, reg->E);}
// 0x1C
void rr_h(Registers* reg) {reg->H = rr(reg, reg->H);}
// 0x1D
void rr_l(Registers* reg) {reg->L = rr(reg, reg->L);}
// 0x1E
void rr_hlp(Registers* reg, Memory* m) {write8(m, reg->HL, rr(reg, read8(m, reg->HL)));}
// 0x1F
void rr_a(Registers* reg) {reg->A = rr(reg, reg->A);}
// 0x20
void sla_b(Registers* reg) {reg->B = sla(reg, reg->B);}
// 0x21
void sla_c(Registers* reg) {reg->C = sla(reg, reg->C);}
// 0x22
void sla_d(Registers* reg) {reg->D = sla(reg, reg->D);}
// 0x23
void sla_e(Registers* reg) {reg->E = sla(reg, reg->E);}
// 0x24
void sla_h(Registers* reg) {reg->H = sla(reg, reg->H);}
// 0x25
void sla_l(Registers* reg) {reg->L = sla(reg, reg->L);}
// 0x26
void sla_hlp(Registers* reg, Memory* m) {write8(m, reg->HL, sla(reg, read8(m, reg->HL)));}
// 0x27
void sla_a(Registers* reg) {reg->A = sla(reg, reg->A);}
// 0x28
void sra_b(Registers* reg) {reg->B = sra(reg, reg->B);}
// 0x29
void sra_c(Registers* reg) {reg->C = sra(reg, reg->C);}
// 0x2A
void sra_d(Registers* reg) {reg->D = sra(reg, reg->D);}
// 0x2B
void sra_e(Registers* reg) {reg->E = sra(reg, reg->E);}
// 0x2C
void sra_h(Registers* reg) {reg->H = sra(reg, reg->H);}
// 0x2D
void sra_l(Registers* reg) {reg->L = sra(reg, reg->L);}
// 0x2E
void sra_hlp(Registers* reg, Memory* m) {write8(m, reg->HL, sra(reg, reg->HL));}
// 0x2F
void sra_a(Registers* reg) {reg->A = sra(reg, reg->A);}
// 0x30
void swap_b(Registers* reg) {reg->B = swap(reg, reg->B);}
// 0x31
void swap_c(Registers* reg) {reg->C = swap(reg, reg->C);}
// 0x32
void swap_d(Registers* reg) {reg->D = swap(reg, reg->D);}
// 0x33
void swap_e(Registers* reg) {reg->E = swap(reg, reg->E);}
// 0x34
void swap_h(Registers* reg) {reg->H = swap(reg, reg->H);}
// 0x35
void swap_l(Registers* reg) {reg->L = swap(reg, reg->L);}
// 0x36
void swap_hlp(Registers* reg, Memory* m) {write8(m, reg->HL, swap(reg, read8(m, reg->HL)));}
// 0x37
void swap_a(Registers* reg) {reg->A = swap(reg, reg->A);}
// 0x38
void srl_b(Registers* reg) {reg->B = srl(reg, reg->B);}
// 0x39
void srl_c(Registers* reg) {reg->C = srl(reg, reg->C);}
// 0x3A
void srl_d(Registers* reg) {reg->D = srl(reg, reg->D);}
// 0x3B
void srl_e(Registers* reg) {reg->E = srl(reg, reg->E);}
// 0x3C
void srl_h(Registers* reg) {reg->H = srl(reg, reg->H);}
// 0x3D
void srl_l(Registers* reg) {reg->L = srl(reg, reg->L);}
// 0x3E
void srl_hlp(Registers* reg, Memory* m) {write8(m, reg->HL, srl(reg, read8(m, reg->HL)));}
// 0x3F
void srl_a(Registers* reg) {
	if(reg->A & 0x01) set_flag_c(reg, 1);
	else set_flag_c(reg, 0);
	
	reg->A >>= 1;
	
	if(reg->A) set_flag_z(reg, 0);
	else set_flag_z(reg, 1);
	
	set_flag_n(reg, 0);
	set_flag_h(reg, 0);
}
// 0x40
void bit_0_b(Registers* reg) {bit(reg, 1 << 0, reg->B);}
// 0x41
void bit_0_c(Registers* reg) {bit(reg, 1 << 0, reg->C);}
// 0x42
void bit_0_d(Registers* reg) {bit(reg, 1 << 0, reg->D);}
// 0x43
void bit_0_e(Registers* reg) {bit(reg, 1 << 0, reg->E);}
// 0x44
void bit_0_h(Registers* reg) {bit(reg, 1 << 0, reg->H);}
// 0x45
void bit_0_l(Registers* reg) {bit(reg, 1 << 0, reg->L);}
// 0x46
void bit_0_hlp(Registers* reg, Memory* m) {bit(reg, 1 << 0, read8(m, reg->HL));}
// 0x47
void bit_0_a(Registers* reg) {bit(reg, 1 << 0, reg->A);}
// 0x48
void bit_1_b(Registers* reg) {bit(reg, 1 << 1, reg->B);}
// 0x49
void bit_1_c(Registers* reg) {bit(reg, 1 << 1, reg->C);}
// 0x4A
void bit_1_d(Registers* reg) {bit(reg, 1 << 1, reg->D);}
// 0x4B
void bit_1_e(Registers* reg) {bit(reg, 1 << 1, reg->E);}
// 0x4C
void bit_1_h(Registers* reg) {bit(reg, 1 << 1, reg->H);}
// 0x4D
void bit_1_l(Registers* reg) {bit(reg, 1 << 1, reg->L);}
// 0x4E
void bit_1_hlp(Registers* reg, Memory* m) {bit(reg, 1 << 1, read8(m, reg->HL));}
// 0x4F
void bit_1_a(Registers* reg) {bit(reg, 1 << 1, reg->A);}
// 0x50
void bit_2_b(Registers* reg) {bit(reg, 1 << 2, reg->B);}
// 0x51
void bit_2_c(Registers* reg) {bit(reg, 1 << 2, reg->C);}
// 0x52
void bit_2_d(Registers* reg) {bit(reg, 1 << 2, reg->D);}
// 0x53
void bit_2_e(Registers* reg) {bit(reg, 1 << 2, reg->E);}
// 0x54
void bit_2_h(Registers* reg) {bit(reg, 1 << 2, reg->H);}
// 0x55
void bit_2_l(Registers* reg) {bit(reg, 1 << 2, reg->L);}
// 0x56
void bit_2_hlp(Registers* reg, Memory* m) {bit(reg, 1 << 2, read8(m, reg->HL));}
// 0x57
void bit_2_a(Registers* reg) {bit(reg, 1 << 2, reg->A);}
// 0x58
void bit_3_b(Registers* reg) {bit(reg, 1 << 3, reg->B);}
// 0x59
void bit_3_c(Registers* reg) {bit(reg, 1 << 3, reg->C);}
// 0x5A
void bit_3_d(Registers* reg) {bit(reg, 1 << 3, reg->D);}
// 0x5B
void bit_3_e(Registers* reg) {bit(reg, 1 << 3, reg->E);}
// 0x5C
void bit_3_h(Registers* reg) {bit(reg, 1 << 3, reg->H);}
// 0x5D
void bit_3_l(Registers* reg) {bit(reg, 1 << 3, reg->L);}
// 0x5E
void bit_3_hlp(Registers* reg, Memory* m) {bit(reg, 1 << 3, read8(m, reg->HL));}
// 0x5F
void bit_3_a(Registers* reg) {bit(reg, 1 << 3, reg->A);}
// 0x60
void bit_4_b(Registers* reg) {bit(reg, 1 << 4, reg->B);}
// 0x61
void bit_4_c(Registers* reg) {bit(reg, 1 << 4, reg->C);}
// 0x62
void bit_4_d(Registers* reg) {bit(reg, 1 << 4, reg->D);}
// 0x63
void bit_4_e(Registers* reg) {bit(reg, 1 << 4, reg->E);}
// 0x64
void bit_4_h(Registers* reg) {bit(reg, 1 << 4, reg->H);}
// 0x65
void bit_4_l(Registers* reg) {bit(reg, 1 << 4, reg->L);}
// 0x66
void bit_4_hlp(Registers* reg, Memory* m) {bit(reg, 1 << 4, read8(m, reg->HL));}
// 0x67
void bit_4_a(Registers* reg) {bit(reg, 1 << 4, reg->A);}
// 0x68
void bit_5_b(Registers* reg) {bit(reg, 1 << 5, reg->B);}
// 0x69
void bit_5_c(Registers* reg) {bit(reg, 1 << 5, reg->C);}
// 0x6A
void bit_5_d(Registers* reg) {bit(reg, 1 << 5, reg->D);}
// 0x6B
void bit_5_e(Registers* reg) {bit(reg, 1 << 5, reg->E);}
// 0x6C
void bit_5_h(Registers* reg) {bit(reg, 1 << 5, reg->H);}
// 0x6D
void bit_5_l(Registers* reg) {bit(reg, 1 << 5, reg->L);}
// 0x6E
void bit_5_hlp(Registers* reg, Memory* m) {bit(reg, 1 << 5, read8(m, reg->HL));}
// 0x6F
void bit_5_a(Registers* reg) {bit(reg, 1 << 5, reg->A);}
// 0x70
void bit_6_b(Registers* reg) {bit(reg, 1 << 6, reg->B);}
// 0x71
void bit_6_c(Registers* reg) {bit(reg, 1 << 6, reg->C);}
// 0x72
void bit_6_d(Registers* reg) {bit(reg, 1 << 6, reg->D);}
// 0x73
void bit_6_e(Registers* reg) {bit(reg, 1 << 6, reg->E);}
// 0x74
void bit_6_h(Registers* reg) {bit(reg, 1 << 6, reg->H);}
// 0x75
void bit_6_l(Registers* reg) {bit(reg, 1 << 6, reg->L);}
// 0x76
void bit_6_hlp(Registers* reg, Memory* m) {bit(reg, 1 << 6, read8(m, reg->HL));}
// 0x77
void bit_6_a(Registers* reg) {bit(reg, 1 << 6, reg->A);}
// 0x78
void bit_7_b(Registers* reg) {bit(reg, 1 << 7, reg->B);}
// 0x79
void bit_7_c(Registers* reg) {bit(reg, 1 << 7, reg->C);}
// 0x7A
void bit_7_d(Registers* reg) {bit(reg, 1 << 7, reg->D);}
// 0x7B
void bit_7_e(Registers* reg) {bit(reg, 1 << 7, reg->E);}
// 0x7C
void bit_7_h(Registers* reg) {bit(reg, 1 << 7, reg->H);}
// 0x7D
void bit_7_l(Registers* reg) {bit(reg, 1 << 7, reg->L);}
// 0x7E
void bit_7_hlp(Registers* reg, Memory* m) {bit(reg, 1 << 7, read8(m, reg->HL));}
// 0x7F
void bit_7_a(Registers* reg) {bit(reg, 1 << 7, reg->A);}
// 0x80
void res_0_b(Registers* reg) {reg->B &= ~(1 << 0);}
// 0x81
void res_0_c(Registers* reg) {reg->C &= ~(1 << 0);}
// 0x82
void res_0_d(Registers* reg) {reg->D &= ~(1 << 0);}
// 0x83
void res_0_e(Registers* reg) {reg->E &= ~(1 << 0);}
// 0x84
void res_0_h(Registers* reg) {reg->H &= ~(1 << 0);}
// 0x85
void res_0_l(Registers* reg) {reg->L &= ~(1 << 0);}
// 0x86
void res_0_hlp(Registers* reg, Memory* m) {write8(m, reg->HL, read8(m, reg->HL) & ~(1 << 0));}
// 0x87
void res_0_a(Registers* reg) {reg->A &= ~(1 << 0);}
// 0x88
void res_1_b(Registers* reg) {reg->B &= ~(1 << 1);}
// 0x89
void res_1_c(Registers* reg) {reg->C &= ~(1 << 1);}
// 0x8A
void res_1_d(Registers* reg) {reg->D &= ~(1 << 1);}
// 0x8B
void res_1_e(Registers* reg) {reg->E &= ~(1 << 1);}
// 0x8C
void res_1_h(Registers* reg) {reg->H &= ~(1 << 1);}
// 0x8D
void res_1_l(Registers* reg) {reg->L &= ~(1 << 1);}
// 0x8E
void res_1_hlp(Registers* reg, Memory* m) {write8(m, reg->HL, reg->HL & ~(1 << 1));}
// 0x8F
void res_1_a(Registers* reg) {reg->A &= ~(1 << 1);}
// 0x90
void res_2_b(Registers* reg) {reg->B &= ~(1 << 2);}
// 0x91
void res_2_c(Registers* reg) {reg->C &= ~(1 << 2);}
// 0x92
void res_2_d(Registers* reg) {reg->D &= ~(1 << 2);}
// 0x93
void res_2_e(Registers* reg) {reg->E &= ~(1 << 2);}
// 0x94
void res_2_h(Registers* reg) {reg->H &= ~(1 << 2);}
// 0x95
void res_2_l(Registers* reg) {reg->L &= ~(1 << 2);}
// 0x96
void res_2_hlp(Registers* reg, Memory* m) {write8(m, reg->HL, reg->HL & ~(1 << 2));}
// 0x97
void res_2_a(Registers* reg) {reg->A &= ~(1 << 2);}
// 0x98
void res_3_b(Registers* reg) {reg->B &= ~(1 << 3);}
// 0x99
void res_3_c(Registers* reg) {reg->C &= ~(1 << 3);}
// 0x9A
void res_3_d(Registers* reg) {reg->D &= ~(1 << 3);}
// 0x9B
void res_3_e(Registers* reg) {reg->E &= ~(1 << 3);}
// 0x9C
void res_3_h(Registers* reg) {reg->H &= ~(1 << 3);}
// 0x9D
void res_3_l(Registers* reg) {reg->L &= ~(1 << 3);}
// 0x9E
void res_3_hlp(Registers* reg, Memory* m) {write8(m, reg->HL, reg->HL & ~(1 << 3));}
// 0x9F
void res_3_a(Registers* reg) {reg->A &= ~(1 << 3);}
// 0xA0
void res_4_b(Registers* reg) {reg->B &= ~(1 << 4);}
// 0xA1
void res_4_c(Registers* reg) {reg->C &= ~(1 << 4);}
// 0xA2
void res_4_d(Registers* reg) {reg->D &= ~(1 << 4);}
// 0xA3
void res_4_e(Registers* reg) {reg->E &= ~(1 << 4);}
// 0xA4
void res_4_h(Registers* reg) {reg->H &= ~(1 << 4);}
// 0xA5
void res_4_l(Registers* reg) {reg->L &= ~(1 << 4);}
// 0xA6
void res_4_hlp(Registers* reg, Memory* m) {write8(m, reg->HL, reg->HL & ~(1 << 4));}
// 0xA7
void res_4_a(Registers* reg) {reg->A &= ~(1 << 4);}
// 0xA8
void res_5_b(Registers* reg) {reg->B &= ~(1 << 5);}
// 0xA9
void res_5_c(Registers* reg) {reg->C &= ~(1 << 5);}
// 0xAA
void res_5_d(Registers* reg) {reg->D &= ~(1 << 5);}
// 0xAB
void res_5_e(Registers* reg) {reg->E &= ~(1 << 5);}
// 0xAC
void res_5_h(Registers* reg) {reg->H &= ~(1 << 5);}
// 0xAD
void res_5_l(Registers* reg) {reg->L &= ~(1 << 5);}
// 0xAE
void res_5_hlp(Registers* reg, Memory* m) {write8(m, reg->HL, reg->HL & ~(1 << 5));}
// 0xAF
void res_5_a(Registers* reg) {reg->A &= ~(1 << 5);}
// 0xB0
void res_6_b(Registers* reg) {reg->B &= ~(1 << 6);}
// 0xB1
void res_6_c(Registers* reg) {reg->C &= ~(1 << 6);}
// 0xB2
void res_6_d(Registers* reg) {reg->D &= ~(1 << 6);}
// 0xB3
void res_6_e(Registers* reg) {reg->E &= ~(1 << 6);}
// 0xB4
void res_6_h(Registers* reg) {reg->H &= ~(1 << 6);}
// 0xB5
void res_6_l(Registers* reg) {reg->L &= ~(1 << 6);}
// 0xB6
void res_6_hlp(Registers* reg, Memory* m) {write8(m, reg->HL, reg->HL & ~(1 << 6));}
// 0xB7
void res_6_a(Registers* reg) {reg->A &= ~(1 << 6);}
// 0xB8
void res_7_b(Registers* reg) {reg->B &= ~(1 << 7);}
// 0xB9
void res_7_c(Registers* reg) {reg->C &= ~(1 << 7);}
// 0xBA
void res_7_d(Registers* reg) {reg->D &= ~(1 << 7);}
// 0xBB
void res_7_e(Registers* reg) {reg->E &= ~(1 << 7);}
// 0xBC
void res_7_h(Registers* reg) {reg->H &= ~(1 << 7);}
// 0xBD
void res_7_l(Registers* reg) {reg->L &= ~(1 << 7);}
// 0xBE
void res_7_hlp(Registers* reg, Memory* m) {write8(m, reg->HL, read8(m, reg->HL) & ~(1 << 7));}
// 0xBF
void res_7_a(Registers* reg) {reg->A &= ~(1 << 7);}
// 0xC0
void set_0_b(Registers* reg) {reg->B = set(reg, 1 << 0, reg->B);}
// 0xC1
void set_0_c(Registers* reg) {reg->C = set(reg, 1 << 0, reg->C);}
// 0xC2
void set_0_d(Registers* reg) {reg->D = set(reg, 1 << 0, reg->D);}
// 0xC3
void set_0_e(Registers* reg) {reg->E = set(reg, 1 << 0, reg->E);}
// 0xC4
void set_0_h(Registers* reg) {reg->H = set(reg, 1 << 0, reg->H);}
// 0xC5
void set_0_l(Registers* reg) {reg->L = set(reg, 1 << 0, reg->L);}
// 0xC6
void set_0_hlp(Registers* reg, Memory* m) {write8(m, reg->HL, set(reg, 1 << 0, read8(m, reg->HL)));}
// 0xC7
void set_0_a(Registers* reg) {reg->A = set(reg, 1 << 0, reg->A);}
// 0xC8
void set_1_b(Registers* reg) {reg->B = set(reg, 1 << 1, reg->B);}
// 0xC9
void set_1_c(Registers* reg) {reg->C = set(reg, 1 << 1, reg->C);}
// 0xCA
void set_1_d(Registers* reg) {reg->D = set(reg, 1 << 1, reg->D);}
// 0xCB
void set_1_e(Registers* reg) {reg->E = set(reg, 1 << 1, reg->E);}
// 0xCC
void set_1_h(Registers* reg) {reg->H = set(reg, 1 << 1, reg->H);}
// 0xCD
void set_1_l(Registers* reg) {reg->L = set(reg, 1 << 1, reg->L);}
// 0xCE
void set_1_hlp(Registers* reg, Memory* m) {write8(m, reg->HL, set(reg, 1 << 1, read8(m, reg->HL)));}
// 0xCF
void set_1_a(Registers* reg) {reg->A = set(reg, 1 << 1, reg->A);}
// 0xD0
void set_2_b(Registers* reg) {reg->B = set(reg, 1 << 2, reg->B);}
// 0xD1
void set_2_c(Registers* reg) {reg->C = set(reg, 1 << 2, reg->C);}
// 0xD2
void set_2_d(Registers* reg) {reg->D = set(reg, 1 << 2, reg->D);}
// 0xD3
void set_2_e(Registers* reg) {reg->E = set(reg, 1 << 2, reg->E);}
// 0xD4
void set_2_h(Registers* reg) {reg->H = set(reg, 1 << 2, reg->H);}
// 0xD5
void set_2_l(Registers* reg) {reg->L = set(reg, 1 << 2, reg->L);}
// 0xD6
void set_2_hlp(Registers* reg, Memory* m) {write8(m, reg->HL, set(reg, 1 << 2, read8(m, reg->HL)));}
// 0xD7
void set_2_a(Registers* reg) {reg->A = set(reg, 1 << 2, reg->A);}
// 0xD8
void set_3_b(Registers* reg) {reg->B = set(reg, 1 << 3, reg->B);}
// 0xD9
void set_3_c(Registers* reg) {reg->C = set(reg, 1 << 3, reg->C);}
// 0xDA
void set_3_d(Registers* reg) {reg->D = set(reg, 1 << 3, reg->D);}
// 0xDB
void set_3_e(Registers* reg) {reg->E = set(reg, 1 << 3, reg->E);}
// 0xDC
void set_3_h(Registers* reg) {reg->H = set(reg, 1 << 3, reg->H);}
// 0xDD
void set_3_l(Registers* reg) {reg->L = set(reg, 1 << 3, reg->L);}
// 0xDE
void set_3_hlp(Registers* reg, Memory* m) {write8(m, reg->HL, set(reg, 1 << 3, read8(m, reg->HL)));}
// 0xDF
void set_3_a(Registers* reg) {reg->A = set(reg, 1 << 3, reg->A);}
// 0xE0
void set_4_b(Registers* reg) {reg->B = set(reg, 1 << 4, reg->B);}
// 0xE1
void set_4_c(Registers* reg) {reg->C = set(reg, 1 << 4, reg->C);}
// 0xE2
void set_4_d(Registers* reg) {reg->D = set(reg, 1 << 4, reg->D);}
// 0xE3
void set_4_e(Registers* reg) {reg->E = set(reg, 1 << 4, reg->E);}
// 0xE4
void set_4_h(Registers* reg) {reg->H = set(reg, 1 << 4, reg->H);}
// 0xE5
void set_4_l(Registers* reg) {reg->L = set(reg, 1 << 4, reg->L);}
// 0xE6
void set_4_hlp(Registers* reg, Memory* m) {write8(m, reg->HL, set(reg, 1 << 4, read8(m, reg->HL)));}
// 0xE7
void set_4_a(Registers* reg) {reg->A = set(reg, 1 << 4, reg->A);}
// 0xE8
void set_5_b(Registers* reg) {reg->B = set(reg, 1 << 5, reg->B);}
// 0xE9
void set_5_c(Registers* reg) {reg->C = set(reg, 1 << 5, reg->C);}
// 0xEA
void set_5_d(Registers* reg) {reg->D = set(reg, 1 << 5, reg->D);}
// 0xEB
void set_5_e(Registers* reg) {reg->E = set(reg, 1 << 5, reg->E);}
// 0xEC
void set_5_h(Registers* reg) {reg->H = set(reg, 1 << 5, reg->H);}
// 0xED
void set_5_l(Registers* reg) {reg->L = set(reg, 1 << 5, reg->L);}
// 0xEE
void set_5_hlp(Registers* reg, Memory* m) {write8(m, reg->HL, set(reg, 1 << 5, read8(m, reg->HL)));}
// 0xEF
void set_5_a(Registers* reg) {reg->A = set(reg, 1 << 5, reg->A);}
// 0xF0
void set_6_b(Registers* reg) {reg->B = set(reg, 1 << 6, reg->B);}
// 0xF1
void set_6_c(Registers* reg) {reg->C = set(reg, 1 << 6, reg->C);}
// 0xF2
void set_6_d(Registers* reg) {reg->D = set(reg, 1 << 6, reg->D);}
// 0xF3
void set_6_e(Registers* reg) {reg->E = set(reg, 1 << 6, reg->E);}
// 0xF4
void set_6_h(Registers* reg) {reg->H = set(reg, 1 << 6, reg->H);}
// 0xF5
void set_6_l(Registers* reg) {reg->L = set(reg, 1 << 6, reg->L);}
// 0xF6
void set_6_hlp(Registers* reg, Memory* m) {write8(m, reg->HL, set(reg, 1 << 6, read8(m, reg->HL)));}
// 0xF7
void set_6_a(Registers* reg) {reg->A = set(reg, 1 << 6, reg->A);}
// 0xF8
void set_7_b(Registers* reg) {reg->B = set(reg, 1 << 7, reg->B);}
// 0xF9
void set_7_c(Registers* reg) {reg->C = set(reg, 1 << 7, reg->C);}
// 0xFA
void set_7_d(Registers* reg) {reg->D = set(reg, 1 << 7, reg->D);}
// 0xFB
void set_7_e(Registers* reg) {reg->E = set(reg, 1 << 7, reg->E);}
// 0xFC
void set_7_h(Registers* reg) {reg->H = set(reg, 1 << 7, reg->H);}
// 0xFD
void set_7_l(Registers* reg) {reg->L = set(reg, 1 << 7, reg->L);}
// 0xFE
void set_7_hlp(Registers* reg, Memory* m) {write8(m, reg->HL, set(reg, 1 << 7, read8(m, reg->HL)));}
// 0xFF
void set_7_a(Registers* reg) {reg->A = set(reg, 1 << 7, reg->A);}


void execute_cb(uint8_t opcode, Registers* reg, Memory* m, int* cycles) {
	
	switch (opcode) {
		case (0x00): rlc_b(reg); break; // 0x00
		case (0x01): rlc_c(reg); break; // 0x01
		case (0x02): rlc_d(reg); break; // 0x02
		case (0x03): rlc_e(reg); break; // 0x03
		case (0x04): rlc_h(reg); break; // 0x04
		case (0x05): rlc_l(reg); break; // 0x05
		case (0x06): rlc_hlp(reg, m); break; // 0x06
		case (0x07): rlc_a(reg); break; // 0x07
		case (0x08): rrc_b(reg); break; // 0x08
		case (0x09): rrc_c(reg); break; // 0x09
		case (0x0A): rrc_d(reg); break; // 0x0A
		case (0x0B): rrc_e(reg); break; // 0x0B
		case (0x0C): rrc_h(reg); break; // 0x0C
		case (0x0D): rrc_l(reg); break; // 0x0D
		case (0x0E): rrc_hlp(reg, m); break; // 0x0E
		case (0x0F): rrc_a(reg); break; // 0x0F
		
		case (0x10): rl_b(reg); break; // 0x10
		case (0x11): rl_c(reg); break; // 0x11
		case (0x12): rl_d(reg); break; // 0x12
		case (0x13): rl_e(reg); break; // 0x13
		case (0x14): rl_h(reg); break; // 0x14
		case (0x15): rl_l(reg); break; // 0x15
		case (0x16): rl_hlp(reg, m); break; // 0x16
		case (0x17): rl_a(reg); break; // 0x17
		case (0x18): rr_b(reg); break; // 0x18
		case (0x19): rr_c(reg); break; // 0x19
		case (0x1A): rr_d(reg); break; // 0x1A
		case (0x1B): rr_e(reg); break; // 0x1B
		case (0x1C): rr_h(reg); break; // 0x1C
		case (0x1D): rr_l(reg); break; // 0x1D
		case (0x1E): rr_hlp(reg, m); break; // 0x1E
		case (0x1F): rr_a(reg); break; // 0x1F

		case (0x20): sla_b(reg); break; // 0x20
		case (0x21): sla_c(reg); break; // 0x21
		case (0x22): sla_d(reg); break; // 0x22
		case (0x23): sla_e(reg); break; // 0x23
		case (0x24): sla_h(reg); break; // 0x24
		case (0x25): sla_l(reg); break; // 0x25
		case (0x26): sla_hlp(reg, m); break; // 0x26
		case (0x27): sla_a(reg); break; // 0x27
		case (0x28): sra_b(reg); break; // 0x28
		case (0x29): sra_c(reg); break; // 0x29
		case (0x2A): sra_d(reg); break; // 0x2A
		case (0x2B): sra_e(reg); break; // 0x2B
		case (0x2C): sra_h(reg); break; // 0x2C
		case (0x2D): sra_l(reg); break; // 0x2D
		case (0x2E): sra_hlp(reg, m); break; // 0x2E
		case (0x2F): sra_a(reg); break; // 0x2F

		case (0x30): swap_b(reg); break; // 0x30
		case (0x31): swap_c(reg); break; // 0x31
		case (0x32): swap_d(reg); break; // 0x32
		case (0x33): swap_e(reg); break; // 0x33
		case (0x34): swap_h(reg); break; // 0x34
		case (0x35): swap_l(reg); break; // 0x35
		case (0x36): swap_hlp(reg, m); break; // 0x36
		case (0x37): swap_a(reg); break; // 0x37
		case (0x38): srl_b(reg); break; // 0x38
		case (0x39): srl_c(reg); break; // 0x39
		case (0x3A): srl_d(reg); break; // 0x3A
		case (0x3B): srl_e(reg); break; // 0x3B
		case (0x3C): srl_h(reg); break; // 0x3C
		case (0x3D): srl_l(reg); break; // 0x3D
		case (0x3E): srl_hlp(reg, m); break; // 0x3E
		case (0x3F): srl_a(reg); break; // 0x3F

		case (0x40): bit_0_b(reg); break; // 0x40
		case (0x41): bit_0_c(reg); break; // 0x41
		case (0x42): bit_0_d(reg); break; // 0x42
		case (0x43): bit_0_e(reg); break; // 0x43
		case (0x44): bit_0_h(reg); break; // 0x44
		case (0x45): bit_0_l(reg); break; // 0x45
		case (0x46): bit_0_hlp(reg, m); break; // 0x46
		case (0x47): bit_0_a(reg); break; // 0x47
		case (0x48): bit_1_b(reg); break; // 0x48
		case (0x49): bit_1_c(reg); break; // 0x49
		case (0x4A): bit_1_d(reg); break; // 0x4A
		case (0x4B): bit_1_e(reg); break; // 0x4B
		case (0x4C): bit_1_h(reg); break; // 0x4C
		case (0x4D): bit_1_l(reg); break; // 0x4D
		case (0x4E): bit_1_hlp(reg, m); break; // 0x4E
		case (0x4F): bit_1_a(reg); break; // 0x4F

		case (0x50): bit_2_b(reg); break; // 0x50
		case (0x51): bit_2_c(reg); break; // 0x51
		case (0x52): bit_2_d(reg); break; // 0x52
		case (0x53): bit_2_e(reg); break; // 0x53
		case (0x54): bit_2_h(reg); break; // 0x54
		case (0x55): bit_2_l(reg); break; // 0x55
		case (0x56): bit_2_hlp(reg, m); break; // 0x56
		case (0x57): bit_2_a(reg); break; // 0x57
		case (0x58): bit_3_b(reg); break; // 0x58
		case (0x59): bit_3_c(reg); break; // 0x59
		case (0x5A): bit_3_d(reg); break; // 0x5A
		case (0x5B): bit_3_e(reg); break; // 0x5B
		case (0x5C): bit_3_h(reg); break; // 0x5C
		case (0x5D): bit_3_l(reg); break; // 0x5D
		case (0x5E): bit_3_hlp(reg, m); break; // 0x5E
		case (0x5F): bit_3_a(reg); break; // 0x5F
		
		case (0x60): bit_4_b(reg); break; // 0x60
		case (0x61): bit_4_c(reg); break; // 0x61
		case (0x62): bit_4_d(reg); break; // 0x62
		case (0x63): bit_4_e(reg); break; // 0x63
		case (0x64): bit_4_h(reg); break; // 0x64
		case (0x65): bit_4_l(reg); break; // 0x65
		case (0x66): bit_4_hlp(reg, m); break; // 0x66
		case (0x67): bit_4_a(reg); break; // 0x67
		case (0x68): bit_5_b(reg); break; // 0x68
		case (0x69): bit_5_c(reg); break; // 0x69
		case (0x6A): bit_5_d(reg); break; // 0x6A
		case (0x6B): bit_5_e(reg); break; // 0x6B
		case (0x6C): bit_5_h(reg); break; // 0x6C
		case (0x6D): bit_5_l(reg); break; // 0x6D
		case (0x6E): bit_5_hlp(reg, m); break; // 0x6E
		case (0x6F): bit_5_a(reg); break; // 0x6F

		case (0x70): bit_6_b(reg); break; // 0x70
		case (0x71): bit_6_c(reg); break; // 0x71
		case (0x72): bit_6_d(reg); break; // 0x72
		case (0x73): bit_6_e(reg); break; // 0x73
		case (0x74): bit_6_h(reg); break; // 0x74
		case (0x75): bit_6_l(reg); break; // 0x75
		case (0x76): bit_6_hlp(reg, m); break; // 0x76
		case (0x77): bit_6_a(reg); break; // 0x77
		case (0x78): bit_7_b(reg); break; // 0x78
		case (0x79): bit_7_c(reg); break; // 0x79
		case (0x7A): bit_7_d(reg); break; // 0x7A
		case (0x7B): bit_7_e(reg); break; // 0x7B
		case (0x7C): bit_7_h(reg); break; // 0x7C
		case (0x7D): bit_7_l(reg); break; // 0x7D
		case (0x7E): bit_7_hlp(reg, m); break; // 0x7E
		case (0x7F): bit_7_a(reg); break; // 0x7F

		case (0x80): res_0_b(reg); break; // 0x80
		case (0x81): res_0_c(reg); break; // 0x81
		case (0x82): res_0_d(reg); break; // 0x82
		case (0x83): res_0_e(reg); break; // 0x83
		case (0x84): res_0_h(reg); break; // 0x84
		case (0x85): res_0_l(reg); break; // 0x85
		case (0x86): res_0_hlp(reg, m); break; // 0x86
		case (0x87): res_0_a(reg); break; // 0x87
		case (0x88): res_1_b(reg); break; // 0x88
		case (0x89): res_1_c(reg); break; // 0x89
		case (0x8A): res_1_d(reg); break; // 0x8A
		case (0x8B): res_1_e(reg); break; // 0x8B
		case (0x8C): res_1_h(reg); break; // 0x8C
		case (0x8D): res_1_l(reg); break; // 0x8D
		case (0x8E): res_1_hlp(reg, m); break; // 0x8E
		case (0x8F): res_1_a(reg); break; // 0x8F

		case (0x90): res_2_b(reg); break; // 0x90
		case (0x91): res_2_c(reg); break; // 0x91
		case (0x92): res_2_d(reg); break; // 0x92
		case (0x93): res_2_e(reg); break; // 0x93
		case (0x94): res_2_h(reg); break; // 0x94
		case (0x95): res_2_l(reg); break; // 0x95
		case (0x96): res_2_hlp(reg, m); break; // 0x96
		case (0x97): res_2_a(reg); break; // 0x97
		case (0x98): res_3_b(reg); break; // 0x98
		case (0x99): res_3_c(reg); break; // 0x99
		case (0x9A): res_3_d(reg); break; // 0x9A
		case (0x9B): res_3_e(reg); break; // 0x9B
		case (0x9C): res_3_h(reg); break; // 0x9C
		case (0x9D): res_3_l(reg); break; // 0x9D
		case (0x9E): res_3_hlp(reg, m); break; // 0x9E
		case (0x9F): res_3_a(reg); break; // 0x9F

		case (0xA0): res_4_b(reg); break; // 0xA0
		case (0xA1): res_4_c(reg); break; // 0xA1
		case (0xA2): res_4_d(reg); break; // 0xA2
		case (0xA3): res_4_e(reg); break; // 0xA3
		case (0xA4): res_4_h(reg); break; // 0xA4
		case (0xA5): res_4_l(reg); break; // 0xA5
		case (0xA6): res_4_hlp(reg, m); break; // 0xA6
		case (0xA7): res_4_a(reg); break; // 0xA7
		case (0xA8): res_5_b(reg); break; // 0xA8
		case (0xA9): res_5_c(reg); break; // 0xA9
		case (0xAA): res_5_d(reg); break; // 0xAA
		case (0xAB): res_5_e(reg); break; // 0xAB
		case (0xAC): res_5_h(reg); break; // 0xAC
		case (0xAD): res_5_l(reg); break; // 0xAD
		case (0xAE): res_5_hlp(reg, m); break; // 0xAE
		case (0xAF): res_5_a(reg); break; // 0xAF
		
		case (0xB0): res_6_b(reg); break; // 0xB0
		case (0xB1): res_6_c(reg); break; // 0xB1
		case (0xB2): res_6_d(reg); break; // 0xB2
		case (0xB3): res_6_e(reg); break; // 0xB3
		case (0xB4): res_6_h(reg); break; // 0xB4
		case (0xB5): res_6_l(reg); break; // 0xB5
		case (0xB6): res_6_hlp(reg, m); break; // 0xB6
		case (0xB7): res_6_a(reg); break; // 0xB7
		case (0xB8): res_7_b(reg); break; // 0xB8
		case (0xB9): res_7_c(reg); break; // 0xB9
		case (0xBA): res_7_d(reg); break; // 0xBA
		case (0xBB): res_7_e(reg); break; // 0xBB
		case (0xBC): res_7_h(reg); break; // 0xBC
		case (0xBD): res_7_l(reg); break; // 0xBD
		case (0xBE): res_7_hlp(reg, m); break; // 0xBE
		case (0xBF): res_7_a(reg); break; // 0xBF

		case (0xC0): set_0_b(reg); break; // 0xC0
		case (0xC1): set_0_c(reg); break; // 0xC1
		case (0xC2): set_0_d(reg); break; // 0xC2
		case (0xC3): set_0_e(reg); break; // 0xC3
		case (0xC4): set_0_h(reg); break; // 0xC4
		case (0xC5): set_0_l(reg); break; // 0xC5
		case (0xC6): set_0_hlp(reg, m); break; // 0xC6
		case (0xC7): set_0_a(reg); break; // 0xC7
		case (0xC8): set_1_b(reg); break; // 0xC8
		case (0xC9): set_1_c(reg); break; // 0xC9
		case (0xCA): set_1_d(reg); break; // 0xCA
		case (0xCB): set_1_e(reg); break; // 0xCB
		case (0xCC): set_1_h(reg); break; // 0xCC
		case (0xCD): set_1_l(reg); break; // 0xCD
		case (0xCE): set_1_hlp(reg, m); break; // 0xCE
		case (0xCF): set_1_a(reg); break; // 0xCF

		case (0xD0): set_2_b(reg); break; // 0xD0
		case (0xD1): set_2_c(reg); break; // 0xD1
		case (0xD2): set_2_d(reg); break; // 0xD2
		case (0xD3): set_2_e(reg); break; // 0xD3
		case (0xD4): set_2_h(reg); break; // 0xD4
		case (0xD5): set_2_l(reg); break; // 0xD5
		case (0xD6): set_2_hlp(reg, m); break; // 0xD6
		case (0xD7): set_2_a(reg); break; // 0xD7
		case (0xD8): set_3_b(reg); break; // 0xD8
		case (0xD9): set_3_c(reg); break; // 0xD9
		case (0xDA): set_3_d(reg); break; // 0xDA
		case (0xDB): set_3_e(reg); break; // 0xDB
		case (0xDC): set_3_h(reg); break; // 0xDC
		case (0xDD): set_3_l(reg); break; // 0xDD
		case (0xDE): set_3_hlp(reg, m); break; // 0xDE
		case (0xDF): set_3_a(reg); break; // 0xDF

		case (0xE0): set_4_b(reg); break; // 0xE0
		case (0xE1): set_4_c(reg); break; // 0xE1
		case (0xE2): set_4_d(reg); break; // 0xE2
		case (0xE3): set_4_e(reg); break; // 0xE3
		case (0xE4): set_4_h(reg); break; // 0xE4
		case (0xE5): set_4_l(reg); break; // 0xE5
		case (0xE6): set_4_hlp(reg, m); break; // 0xE6
		case (0xE7): set_4_a(reg); break; // 0xE7
		case (0xE8): set_5_b(reg); break; // 0xE8
		case (0xE9): set_5_c(reg); break; // 0xE9
		case (0xEA): set_5_d(reg); break; // 0xEA
		case (0xEB): set_5_e(reg); break; // 0xEB
		case (0xEC): set_5_h(reg); break; // 0xEC
		case (0xED): set_5_l(reg); break; // 0xED
		case (0xEE): set_5_hlp(reg, m); break; // 0xEE
		case (0xEF): set_5_a(reg); break; // 0xEF

		case (0xF0): set_6_b(reg); break; // 0xF0
		case (0xF1): set_6_c(reg); break; // 0xF1
		case (0xF2): set_6_d(reg); break; // 0xF2
		case (0xF3): set_6_e(reg); break; // 0xF3
		case (0xF4): set_6_h(reg); break; // 0xF4
		case (0xF5): set_6_l(reg); break; // 0xF5
		case (0xF6): set_6_hlp(reg, m); break; // 0xF6
		case (0xF7): set_6_a(reg); break; // 0xF7
		case (0xF8): set_7_b(reg); break; // 0xF8
		case (0xF9): set_7_c(reg); break; // 0xF9
		case (0xFA): set_7_d(reg); break; // 0xFA
		case (0xFB): set_7_e(reg); break; // 0xFB
		case (0xFC): set_7_h(reg); break; // 0xFC
		case (0xFD): set_7_l(reg); break; // 0xFD
		case (0xFE): set_7_hlp(reg, m); break; // 0xFE
		case (0xFF): set_7_a(reg); break; // 0xFF
	}
	
	*cycles += cb_cycle_lengths[opcode];

}