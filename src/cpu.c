#include <stdio.h>
#include <stdlib.h>

#include "registers.h"
#include "rom.h"

void ld(uint8_t* a, uint8_t* b) {
	
}

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
void ld_bc_xx(Registers* reg) {;}
// 0x02
void ld_bcp_a(Registers* reg) {;}
// 0x03
void inc_bc(Registers* reg) {;}
// 0x04
void inc_b(Registers* reg) {;}
// 0x05
void dec_b(Registers* reg) {;}
// 0x06
void ld_b_x(Registers* reg) {;}
// 0x07
void rlca(Registers* reg) {;}
// 0x08
void ld_xxp_sp(Registers* reg) {;}
// 0x09
void add_hl_bc(Registers* reg) {;}
// 0x0A
void ld_a_bcp(Registers* reg) {;}
// 0x0B
void dec_bc(Registers* reg) {;}
// 0x0C
void inc_c(Registers* reg) {;}
// 0x0D
void dec_c(Registers* reg) {;}
// 0x0E
void ld_c_x(Registers* reg) {;}
// 0x0F
void rrca(Registers* reg) {;}

// --- 0x1x --- 

// 0x10
void stop(Registers* reg) {;}
// 0x11
void ld_de_xx(Registers* reg) {;}
// 0x12
void ld_dep_a(Registers* reg) {;}
// 0x13
void inc_de(Registers* reg) {;}
// 0x14
void inc_d(Registers* reg) {;}
// 0x15
void dec_d(Registers* reg) {;}
// 0x16
void ld_d_x(Registers* reg) {;}
// 0x17
void rla(Registers* reg) {;}
// 0x18
void jr_x(Registers* reg) {;}
// 0x19
void add_hl_de(Registers* reg) {;}
// 0x1A
void ld_a_dep(Registers* reg) {;}
// 0x1B
void dec_de(Registers* reg) {;}
// 0x1C
void inc_e(Registers* reg) {;}
// 0x1D
void dec_e(Registers* reg) {;}
// 0x1E
void ld_e_x(Registers* reg) {;}
// 0x1F
void rra(Registers* reg) {;}

// --- 0x2x --- 

// 0x20
void jr_nz_x(Registers* reg) {;}
// 0x21
void ld_hl_xx(Registers* reg) {;}
// 0x22
void ldi_hlp_a(Registers* reg) {;}
// 0x23
void inc_hl(Registers* reg) {;}
// 0x24
void inc_h(Registers* reg) {;}
// 0x25
void dec_h(Registers* reg) {;}
// 0x26
void ld_h_x(Registers* reg) {;}
// 0x27
void daa(Registers* reg) {;}
// 0x28
void jr_z_x(Registers* reg) {;}
// 0x29
void add_hl_hl(Registers* reg) {;}
// 0x2A
void ldi_a_hlp(Registers* reg) {;}
// 0x2B
void dec_hl(Registers* reg) {;}
// 0x2C
void inc_l(Registers* reg) {;}
// 0x2D
void dec_l(Registers* reg) {;}
// 0x2E
void ld_l_x(Registers* reg) {;}
// 0x2F
void cpl(Registers* reg) {;}

// --- 0x3x --- 

// 0x30
void jp_nc_x(Registers* reg) {;}
// 0x31
void ld_sp_xx(Registers* reg) {printf("\nran instruction 0x31\n");}
// 0x32
void ldd_hlp_a(Registers* reg) {;}
// 0x33
void inc_sp(Registers* reg) {;}
// 0x34
void inc_hlp(Registers* reg) {;}
// 0x35
void dec_hlp(Registers* reg) {;}
// 0x36
void ld_hlp_x(Registers* reg) {;}
// 0x37
void scf(Registers* reg) {;}
// 0x38
void jr_c_x(Registers* reg) {;}
// 0x39
void add_hl_sp(Registers* reg) {;}
// 0x3A
void ldd_a_hlp(Registers* reg) {;}
// 0x3B
void dec_sp(Registers* reg) {;}
// 0x3C
void inc_a(Registers* reg) {;}
// 0x3D
void dec_a(Registers* reg) {;}
// 0x3E
void ld_a_x(Registers* reg) {;}
// 0x3F
void ccf(Registers* reg) {;}

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
void ld_b_hlp(Registers* reg) {;}
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
void ld_c_hlp(Registers* reg) {;}
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
void ld_d_hlp(Registers* reg) {;}
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
void ld_e_hlp(Registers* reg) {;}
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
void ld_h_hlp(Registers* reg) {;}
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
void ld_l_hlp(Registers* reg) {;}
// 0x6F
void ld_l_a(Registers* reg) {reg->L = reg->A;}

// --- 0x7x ---

// 0x70
void ld_hlp_b(Registers* reg) {;}
// 0x71
void ld_hlp_c(Registers* reg) {;}
// 0x72
void ld_hlp_d(Registers* reg) {;}
// 0x73
void ld_hlp_e(Registers* reg) {;}
// 0x74
void ld_hlp_h(Registers* reg) {;}
// 0x75
void ld_hlp_l(Registers* reg) {;}
// 0x76
void halt(Registers* reg) {;}
// 0x77
void ld_hlp_a(Registers* reg) {;}
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
void ld_a_hlp(Registers* reg) {;}
// 0x7F
void ld_a_a(Registers* reg) {reg->A = reg->A;}

// --- 0x8x ---

// 0x80
void add_a_b(Registers* reg) {;}
// 0x81
void add_a_c(Registers* reg) {;}
// 0x82
void add_a_d(Registers* reg) {;}
// 0x83
void add_a_e(Registers* reg) {;}
// 0x84
void add_a_h(Registers* reg) {;}
// 0x85
void add_a_l(Registers* reg) {;}
// 0x86
void add_a_hlp(Registers* reg) {;}
// 0x87
void add_a_a(Registers* reg) {;}
// 0x88
void adc_a_b(Registers* reg) {;}
// 0x89
void adc_a_c(Registers* reg) {;}
// 0x8A
void adc_a_d(Registers* reg) {;}
// 0x8B
void adc_a_e(Registers* reg) {;}
// 0x8C
void adc_a_h(Registers* reg) {;}
// 0x8D
void adc_a_l(Registers* reg) {;}
// 0x8E
void adc_a_hlp(Registers* reg) {;}
// 0x8F
void adc_a_a(Registers* reg) {;}

// --- 0x9x ---

// 0x90
void sub_b(Registers* reg) {;}
// 0x91
void sub_c(Registers* reg) {;}
// 0x92
void sub_d(Registers* reg) {;}
// 0x93
void sub_e(Registers* reg) {;}
// 0x94
void sub_h(Registers* reg) {;}
// 0x95
void sub_l(Registers* reg) {;}
// 0x96
void sub_hlp(Registers* reg) {;}
// 0x97
void sub_a(Registers* reg) {;}
// 0x98
void sbc_a_b(Registers* reg) {;}
// 0x99
void sbc_a_c(Registers* reg) {;}
// 0x9A
void sbc_a_d(Registers* reg) {;}
// 0x9B
void sbc_a_e(Registers* reg) {;}
// 0x9C
void sbc_a_h(Registers* reg) {;}
// 0x9D
void sbc_a_l(Registers* reg) {;}
// 0x9E
void sbc_a_hlp(Registers* reg) {;}
// 0x9F
void sbc_a_a(Registers* reg) {;}

// --- 0xAx ---

// 0xA0
void and_b(Registers* reg) {;}
// 0xA1
void and_c(Registers* reg) {;}
// 0xA2
void and_d(Registers* reg) {;}
// 0xA3
void and_e(Registers* reg) {;}
// 0xA4
void and_h(Registers* reg) {;}
// 0xA5
void and_l(Registers* reg) {;}
// 0xA6
void and_hlp(Registers* reg) {;}
// 0xA7
void and_a(Registers* reg) {;}
// 0xA8
void xor_b(Registers* reg) {;}
// 0xA9
void xor_c(Registers* reg) {;}
// 0xAA
void xor_d(Registers* reg) {;}
// 0xAB
void xor_e(Registers* reg) {;}
// 0xAC
void xor_h(Registers* reg) {;}
// 0xAD
void xor_l(Registers* reg) {;}
// 0xAE
void xor_hlp(Registers* reg) {;}
// 0xAF
void xor_a(Registers* reg) {;}

// --- 0xBx ---

// 0xB0
void or_b(Registers* reg) {;}
// 0xB1
void or_c(Registers* reg) {;}
// 0xB2
void or_d(Registers* reg) {;}
// 0xB3
void or_e(Registers* reg) {;}
// 0xB4
void or_h(Registers* reg) {;}
// 0xB5
void or_l(Registers* reg) {;}
// 0xB6
void or_hlp(Registers* reg) {;}
// 0xB7
void or_a(Registers* reg) {;}
// 0xB8
void cp_b(Registers* reg) {;}
// 0xB9
void cp_c(Registers* reg) {;}
// 0xBA
void cp_d(Registers* reg) {;}
// 0xBB
void cp_e(Registers* reg) {;}
// 0xBC
void cp_h(Registers* reg) {;}
// 0xBD
void cp_l(Registers* reg) {;}
// 0xBE
void cp_hlp(Registers* reg) {;}
// 0xBF
void cp_a(Registers* reg) {;}

// --- 0xCx ---

// 0xC0
void ret_nz(Registers* reg) {;}
// 0xC1
void pop_bc(Registers* reg) {;}
// 0xC2
void jp_nz_xx(Registers* reg) {;}
// 0xC3
void jp_xx(Registers* reg) {;}
// 0xC4
void call_nz_xx(Registers* reg) {;}
// 0xC5
void push_bc(Registers* reg) {;}
// 0xC6
void add_a_x(Registers* reg) {;}
// 0xC7
void rst_0(Registers* reg) {;}
// 0xC8
void ret_z(Registers* reg) {;}
// 0xC9
void ret(Registers* reg) {;}
// 0xCA
void jp_z_xx(Registers* reg) {;}
// 0xCB
void prefix_cb(Registers* reg) {;}
// 0xCC
void call_z_xx(Registers* reg) {;}
// 0xCD
void call_xx(Registers* reg) {;}
// 0xCE
void adc_a_x(Registers* reg) {;}
// 0xCF
void rst_08(Registers* reg) {;}

// --- 0xDx ---

// 0xD0
void ret_nc(Registers* reg) {;}
// 0xD1
void pop_de(Registers* reg) {;}
// 0xD2
void jp_nc_xx(Registers* reg) {;}
// 0xD4
void call_nc_xx(Registers* reg) {;}
// 0xD5
void push_de(Registers* reg) {;}
// 0xD6
void sub_x(Registers* reg) {;}
// 0xD7
void rst_10(Registers* reg) {;}
// 0xD8
void ret_c(Registers* reg) {;}
// 0xD9
void reti(Registers* reg) {;}
// 0xDA
void jp_c_xx(Registers* reg) {;}
// 0xDC
void call_c_xx(Registers* reg) {;}
// 0xDE
void sbc_a_x(Registers* reg) {;}
// 0xDF
void rst_18(Registers* reg) {;}

// --- 0xEx ---

// 0xE0
void ldh_x_a(Registers* reg) {;}
// 0xE1
void pop_hl(Registers* reg) {;}
// 0xE2
void ld_cp_a(Registers* reg) {;}
// 0xE5
void push_hl(Registers* reg) {;}
// 0xE6
void and_x(Registers* reg) {;}
// 0xE7
void rst_20(Registers* reg) {;}
// 0xE8
void add_sp_x(Registers* reg) {;}
// 0xE9
void jp_hlp(Registers* reg) {;}
// 0xEA
void ld_xx_a(Registers* reg) {;}
// 0xEE
void xor_x(Registers* reg) {;}
// 0xEF
void rst_28(Registers* reg) {;}

// --- 0xFx ---

// 0xF0
void ldh_a_xp(Registers* reg) {;}
// 0xF1
void pop_af(Registers* reg) {;}
// 0xF2
void ld_a_cp(Registers* reg) {;}
// 0xF3
void di(Registers* reg) {;}
// 0xF5
void push_af(Registers* reg) {;}
// 0xF6
void or_x(Registers* reg) {;}
// 0xF7
void rst_30(Registers* reg) {;}
// 0xF8
void ld_hl_sp_x(Registers* reg) {;}
// 0xF9
void ld_sp_hl(Registers* reg) {;}
// 0xFA
void ld_a_xp(Registers* reg) {;}
// 0xFB
void ei(Registers* reg) {;}
// 0xFE
void cp_x(Registers* reg) {;}
// 0xFF
void rst_38(Registers* reg) {;}

void cycle(uint8_t* m, Registers* reg){

	uint8_t instruction = m[reg->PC];
	printf("\n%x\n",instruction);
	printf("2. A = %d, B = %d\n",reg->A,reg->B);
	reg->A = reg->B;
	printf("3. A = %d, B = %d\n",reg->A,reg->B);
	
	/*
	x: 1 byte parameter
	xx: 2 byte parameter
	*p: (p)
	*/
	
	switch (instruction) {
		case (0x00):nop(reg); break; // 0x00
		case (0x01):ld_bc_xx(reg); break; // 0x01
		case (0x02):ld_bcp_a(reg); break; // 0x02
		case (0x03):inc_bc(reg); break; // 0x03
		case (0x04):inc_b(reg); break; // 0x04
		case (0x05):dec_b(reg); break; // 0x05
		case (0x06):ld_b_x(reg); break; // 0x06
		case (0x07):rlca(reg); break; // 0x07
		case (0x08):ld_xxp_sp(reg); break; // 0x08
		case (0x09):add_hl_bc(reg); break; // 0x09
		case (0x0A):ld_a_bcp(reg); break; // 0x0A
		case (0x0B):dec_bc(reg); break; // 0x0B
		case (0x0C):inc_c(reg); break; // 0x0C
		case (0x0D):dec_c(reg); break; // 0x0D
		case (0x0E):ld_c_x(reg); break; // 0x0E
		case (0x0F):rrca(reg); break; // 0x0F
		
		case (0x10):stop(reg); break; // 0x10
		case (0x11):ld_de_xx(reg); break; // 0x11
		case (0x12):ld_dep_a(reg); break; // 0x12
		case (0x13):inc_de(reg); break; // 0x13
		case (0x14):inc_d(reg); break; // 0x14
		case (0x15):dec_d(reg); break; // 0x15
		case (0x16):ld_d_x(reg); break; // 0x16
		case (0x17):rla(reg); break; // 0x17
		case (0x18):jr_x(reg); break; // 0x18
		case (0x19):add_hl_de(reg); break; // 0x19
		case (0x1A):ld_a_dep(reg); break; // 0x1A
		case (0x1B):dec_de(reg); break; // 0x1B
		case (0x1C):inc_e(reg); break; // 0x1C
		case (0x1D):dec_e(reg); break; // 0x1D
		case (0x1E):ld_e_x(reg); break; // 0x1E
		case (0x1F):rra(reg); break; // 0x1F
		
		case (0x20):jr_nz_x(reg); break; // 0x20
		case (0x21):ld_hl_xx(reg); break; // 0x21
		case (0x22):ldi_hlp_a(reg); break; // 0x22
		case (0x23):inc_hl(reg); break; // 0x23
		case (0x24):inc_h(reg); break; // 0x24
		case (0x25):dec_h(reg); break; // 0x25
		case (0x26):ld_h_x(reg); break; // 0x26
		case (0x27):daa(reg); break; // 0x27
		case (0x28):jr_z_x(reg); break; // 0x28
		case (0x29):add_hl_hl(reg); break; // 0x29
		case (0x2A):ldi_a_hlp(reg); break; // 0x2A
		case (0x2B):dec_hl(reg); break; // 0x2B
		case (0x2C):inc_l(reg); break; // 0x2C
		case (0x2D):dec_l(reg); break; // 0x2D
		case (0x2E):ld_l_x(reg); break; // 0x2E
		case (0x2F):cpl(reg); break; // 0x2F
		
		case (0x30):jp_nc_x(reg); break; // 0x30
		case (0x31):ld_sp_xx(reg); break; // 0x31
		case (0x32):ldd_hlp_a(reg); break; // 0x32
		case (0x33):inc_sp(reg); break; // 0x33
		case (0x34):inc_hlp(reg); break; // 0x34
		case (0x35):dec_hlp(reg); break; // 0x35
		case (0x36):ld_hlp_x(reg); break; // 0x36
		case (0x37):scf(reg); break; // 0x37
		case (0x38):jr_c_x(reg); break; // 0x38
		case (0x39):add_hl_sp(reg); break; // 0x39
		case (0x3A):ldd_a_hlp(reg); break; // 0x3A
		case (0x3B):dec_sp(reg); break; // 0x3B
		case (0x3C):inc_a(reg); break; // 0x3C
		case (0x3D):dec_a(reg); break; // 0x3D
		case (0x3E):ld_a_x(reg); break; // 0x3E
		case (0x3F):ccf(reg); break; // 0x3F
		
		case (0x40):ld_b_b(reg); break; // 0x40
		case (0x41):ld_b_c(reg); break; // 0x41
		case (0x42):ld_b_d(reg); break; // 0x42
		case (0x43):ld_b_e(reg); break; // 0x43
		case (0x44):ld_b_h(reg); break; // 0x44
		case (0x45):ld_b_l(reg); break; // 0x45
		case (0x46):ld_b_hlp(reg); break; // 0x46
		case (0x47):ld_b_a(reg); break; // 0x47
		case (0x48):ld_c_b(reg); break; // 0x48
		case (0x49):ld_c_c(reg); break; // 0x49
		case (0x4A):ld_c_d(reg); break; // 0x4A
		case (0x4B):ld_c_e(reg); break; // 0x4B
		case (0x4C):ld_c_h(reg); break; // 0x4C
		case (0x4D):ld_c_l(reg); break; // 0x4D
		case (0x4E):ld_c_hlp(reg); break; // 0x4E
		case (0x4F):ld_c_a(reg); break; // 0x4F
		
		case (0x50):ld_d_b(reg); break; // 0x50
		case (0x51):ld_d_c(reg); break; // 0x51
		case (0x52):ld_d_d(reg); break; // 0x52
		case (0x53):ld_d_e(reg); break; // 0x53
		case (0x54):ld_d_h(reg); break; // 0x54
		case (0x55):ld_d_l(reg); break; // 0x55
		case (0x56):ld_d_hlp(reg); break; // 0x56
		case (0x57):ld_d_a(reg); break; // 0x57
		case (0x58):ld_e_b(reg); break; // 0x58
		case (0x59):ld_e_c(reg); break; // 0x59
		case (0x5A):ld_e_d(reg); break; // 0x5A
		case (0x5B):ld_e_e(reg); break; // 0x5B
		case (0x5C):ld_e_h(reg); break; // 0x5C
		case (0x5D):ld_e_l(reg); break; // 0x5D
		case (0x5E):ld_e_hlp(reg); break; // 0x5E
		case (0x5F):ld_e_a(reg); break; // 0x5F
		
		case (0x60):ld_h_b(reg); break; // 0x60
		case (0x61):ld_h_c(reg); break; // 0x61
		case (0x62):ld_h_d(reg); break; // 0x62
		case (0x63):ld_h_e(reg); break; // 0x63
		case (0x64):ld_h_h(reg); break; // 0x64
		case (0x65):ld_h_l(reg); break; // 0x65
		case (0x66):ld_h_hlp(reg); break; // 0x66
		case (0x67):ld_h_a(reg); break; // 0x67
		case (0x68):ld_l_b(reg); break; // 0x68
		case (0x69):ld_l_c(reg); break; // 0x69
		case (0x6A):ld_l_d(reg); break; // 0x6A
		case (0x6B):ld_l_e(reg); break; // 0x6B
		case (0x6C):ld_l_h(reg); break; // 0x6C
		case (0x6D):ld_l_l(reg); break; // 0x6D
		case (0x6E):ld_l_hlp(reg); break; // 0x6E
		case (0x6F):ld_l_a(reg); break; // 0x6F
		
		case (0x70):ld_hlp_b(reg); break; // 0x70
		case (0x71):ld_hlp_c(reg); break; // 0x71
		case (0x72):ld_hlp_d(reg); break; // 0x72
		case (0x73):ld_hlp_e(reg); break; // 0x73
		case (0x74):ld_hlp_h(reg); break; // 0x74
		case (0x75):ld_hlp_l(reg); break; // 0x75
		case (0x76):halt(reg); break; // 0x76
		case (0x77):ld_hlp_a(reg); break; // 0x77
		case (0x78):ld_a_b(reg); break; // 0x78
		case (0x79):ld_a_c(reg); break; // 0x79
		case (0x7A):ld_a_d(reg); break; // 0x7A
		case (0x7B):ld_a_e(reg); break; // 0x7B
		case (0x7C):ld_a_h(reg); break; // 0x7C
		case (0x7D):ld_a_l(reg); break; // 0x7D
		case (0x7E):ld_a_hlp(reg); break; // 0x7E
		case (0x7F):ld_a_a(reg); break; // 0x7F
		
		case (0x80):add_a_b(reg); break; // 0x80
		case (0x81):add_a_c(reg); break; // 0x81
		case (0x82):add_a_d(reg); break; // 0x82
		case (0x83):add_a_e(reg); break; // 0x83
		case (0x84):add_a_h(reg); break; // 0x84
		case (0x85):add_a_l(reg); break; // 0x85
		case (0x86):add_a_hlp(reg); break; // 0x86
		case (0x87):add_a_a(reg); break; // 0x87
		case (0x88):adc_a_b(reg); break; // 0x88
		case (0x89):adc_a_c(reg); break; // 0x89
		case (0x8A):adc_a_d(reg); break; // 0x8A
		case (0x8B):adc_a_e(reg); break; // 0x8B
		case (0x8C):adc_a_h(reg); break; // 0x8C
		case (0x8D):adc_a_l(reg); break; // 0x8D
		case (0x8E):adc_a_hlp(reg); break; // 0x8E
		case (0x8F):adc_a_a(reg); break; // 0x8F
		
		case (0x90):sub_b(reg); break; // 0x90
		case (0x91):sub_c(reg); break; // 0x91
		case (0x92):sub_d(reg); break; // 0x92
		case (0x93):sub_e(reg); break; // 0x93
		case (0x94):sub_h(reg); break; // 0x94
		case (0x95):sub_l(reg); break; // 0x95
		case (0x96):sub_hlp(reg); break; // 0x96
		case (0x97):sub_a(reg); break; // 0x97
		case (0x98):sbc_a_b(reg); break; // 0x98
		case (0x99):sbc_a_c(reg); break; // 0x99
		case (0x9A):sbc_a_d(reg); break; // 0x9A
		case (0x9B):sbc_a_e(reg); break; // 0x9B
		case (0x9C):sbc_a_h(reg); break; // 0x9C
		case (0x9D):sbc_a_l(reg); break; // 0x9D
		case (0x9E):sbc_a_hlp(reg); break; // 0x9E
		case (0x9F):sbc_a_a(reg); break; // 0x9F
		
		case (0xA0):and_b(reg); break; // 0xA0
		case (0xA1):and_c(reg); break; // 0xA1
		case (0xA2):and_d(reg); break; // 0xA2
		case (0xA3):and_e(reg); break; // 0xA3
		case (0xA4):and_h(reg); break; // 0xA4
		case (0xA5):and_l(reg); break; // 0xA5
		case (0xA6):and_hlp(reg); break; // 0xA6
		case (0xA7):and_a(reg); break; // 0xA7
		case (0xA8):xor_b(reg); break; // 0xA8
		case (0xA9):xor_c(reg); break; // 0xA9
		case (0xAA):xor_d(reg); break; // 0xAA
		case (0xAB):xor_e(reg); break; // 0xAB
		case (0xAC):xor_h(reg); break; // 0xAC
		case (0xAD):xor_l(reg); break; // 0xAD
		case (0xAE):xor_hlp(reg); break; // 0xAE
		case (0xAF):xor_a(reg); break; // 0xAF
		
		case (0xB0):or_b(reg); break; // 0xB0
		case (0xB1):or_c(reg); break; // 0xB1
		case (0xB2):or_d(reg); break; // 0xB2
		case (0xB3):or_e(reg); break; // 0xB3
		case (0xB4):or_h(reg); break; // 0xB4
		case (0xB5):or_l(reg); break; // 0xB5
		case (0xB6):or_hlp(reg); break; // 0xB6
		case (0xB7):or_a(reg); break; // 0xB7
		case (0xB8):cp_b(reg); break; // 0xB8
		case (0xB9):cp_c(reg); break; // 0xB9
		case (0xBA):cp_d(reg); break; // 0xBA
		case (0xBB):cp_e(reg); break; // 0xBB
		case (0xBC):cp_h(reg); break; // 0xBC
		case (0xBD):cp_l(reg); break; // 0xBD
		case (0xBE):cp_hlp(reg); break; // 0xBE
		case (0xBF):cp_a(reg); break; // 0xBF
		
		case (0xC0):ret_nz(reg); break; // 0xC0
		case (0xC1):pop_bc(reg); break; // 0xC1
		case (0xC2):jp_nz_xx(reg); break; // 0xC2
		case (0xC3):jp_xx(reg); break; // 0xC3
		case (0xC4):call_nz_xx(reg); break; // 0xC4
		case (0xC5):push_bc(reg); break; // 0xC5
		case (0xC6):add_a_x(reg); break; // 0xC6
		case (0xC7):rst_0(reg); break; // 0xC7
		case (0xC8):ret_z(reg); break; // 0xC8
		case (0xC9):ret(reg); break; // 0xC9
		case (0xCA):jp_z_xx(reg); break; // 0xCA
		case (0xCB):prefix_cb(reg); break; // 0xCB
		case (0xCC):call_z_xx(reg); break; // 0xCC
		case (0xCD):call_xx(reg); break; // 0xCD
		case (0xCE):adc_a_x(reg); break; // 0xCE
		case (0xCF):rst_08(reg); break; // 0xCF
		
		case (0xD0):ret_nc(reg); break; // 0xD0
		case (0xD1):pop_de(reg); break; // 0xD1
		case (0xD2):jp_nc_xx(reg); break; // 0xD2
		case (0xD3):nop(reg); break; // 0xD3 blank
		case (0xD4):call_nc_xx(reg); break; // 0xD4
		case (0xD5):push_de(reg); break; // 0xD5
		case (0xD6):sub_x(reg); break; // 0xD6
		case (0xD7):rst_10(reg); break; // 0xD7
		case (0xD8):ret_c(reg); break; // 0xD8
		case (0xD9):reti(reg); break; // 0xD9
		case (0xDA):jp_c_xx(reg); break; // 0xDA
		case (0xDB):nop(reg); break; // 0xDB blank
		case (0xDC):call_c_xx(reg); break; // 0xDC
		case (0xDD):nop(reg); break; // 0xDD blank
		case (0xDE):sbc_a_x(reg); break; // 0xDE
		case (0xDF):rst_18(reg); break; // 0xDF
		
		case (0xE0):ldh_x_a(reg); break; // 0xE0
		case (0xE1):pop_hl(reg); break; // 0xE1
		case (0xE2):ld_cp_a(reg); break; // 0xE2
		case (0xE3):nop(reg); break; // 0xE3 blank
		case (0xE4):nop(reg); break; // 0xE4 blank
		case (0xE5):push_hl(reg); break; // 0xE5
		case (0xE6):and_x(reg); break; // 0xE6
		case (0xE7):rst_20(reg); break; // 0xE7
		case (0xE8):add_sp_x(reg); break; // 0xE8
		case (0xE9):jp_hlp(reg); break; // 0xE9
		case (0xEA):ld_xx_a(reg); break; // 0xEA
		case (0xEB):nop(reg); break; // 0xEB blank
		case (0xEC):nop(reg); break; // 0xEC blank
		case (0xED):nop(reg); break; // 0xED blank
		case (0xEE):xor_x(reg); break; // 0xEE
		case (0xEF):rst_28(reg); break; // 0xEF
		
		case (0xF0):ldh_a_xp(reg); break; // 0xF0
		case (0xF1):pop_af(reg); break; // 0xF1
		case (0xF2):ld_a_cp(reg); break; // 0xF2
		case (0xF3):di(reg); break; // 0xF3
		case (0xF4):nop(reg); break; // 0xF4 blank
		case (0xF5):push_af(reg); break; // 0xF5
		case (0xF6):or_x(reg); break; // 0xF6
		case (0xF7):rst_30(reg); break; // 0xF7
		case (0xF8):ld_hl_sp_x(reg); break; // 0xF8
		case (0xF9):ld_sp_hl(reg); break; // 0xF9
		case (0xFA):ld_a_xp(reg); break; // 0xFA
		case (0xFB):ei(reg); break; // 0xFB
		case (0xFC):nop(reg); break; // 0xFC blank
		case (0xFD):nop(reg); break; // 0xFD blank
		case (0xFE):cp_x(reg); break; // 0xFE
		case (0xFF):rst_38(reg); break; // 0xFF
	}
}
	
