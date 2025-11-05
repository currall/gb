#include <stdio.h>
#include <stdlib.h>

#include "registers.h"
#include "rom.h"

/*
information on opcodes found at: https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html
 - name of opcode
 - corresponding byte format
 - size of opcode + operand in bytes
*/

// --- 0x0x ---

// 0x00
void nop() {;}
// 0x01
void ld_bc_xx() {;}
// 0x02
void ld_bcp_a() {;}
// 0x03
void inc_bc() {;}
// 0x04
void inc_b() {;}
// 0x05
void dec_b() {;}
// 0x06
void ld_b_x() {;}
// 0x07
void rlca() {;}
// 0x08
void ld_xxp_sp() {;}
// 0x09
void add_hl_bc() {;}
// 0x0A
void ld_a_bcp() {;}
// 0x0B
void dec_bc() {;}
// 0x0C
void inc_c() {;}
// 0x0D
void dec_c() {;}
// 0x0E
void ld_c_x() {;}
// 0x0F
void rrca() {;}

// --- 0x1x --- 

// 0x10
void stop() {;}
// 0x11
void ld_de_xx() {;}
// 0x12
void ld_dep_a() {;}
// 0x13
void inc_de() {;}
// 0x14
void inc_d() {;}
// 0x15
void dec_d() {;}
// 0x16
void ld_d_x() {;}
// 0x17
void rla() {;}
// 0x18
void jr_x() {;}
// 0x19
void add_hl_de() {;}
// 0x1A
void ld_a_dep() {;}
// 0x1B
void dec_de() {;}
// 0x1C
void inc_e() {;}
// 0x1D
void dec_e() {;}
// 0x1E
void ld_e_x() {;}
// 0x1F
void rra() {;}

// --- 0x2x --- 

// 0x20
void jr_nz_x() {;}
// 0x21
void ld_hl_xx() {;}
// 0x22
void ldi_hlp_a() {;}
// 0x23
void inc_hl() {;}
// 0x24
void inc_h() {;}
// 0x25
void dec_h() {;}
// 0x26
void ld_h_x() {;}
// 0x27
void daa() {;}
// 0x28
void jr_z_x() {;}
// 0x29
void add_hl_hl() {;}
// 0x2A
void ldi_a_hlp() {;}
// 0x2B
void dec_hl() {;}
// 0x2C
void inc_l() {;}
// 0x2D
void dec_l() {;}
// 0x2E
void ld_l_x() {;}
// 0x2F
void cpl() {;}

// --- 0x3x --- 

// 0x30
void jp_nc_x() {;}
// 0x31
void ld_sp_xx() {printf("\nok";}
// 0x32
void ldd_hlp_a() {;}
// 0x33
void inc_sp() {;}
// 0x34
void inc_hlp() {;}
// 0x35
void dec_hlp() {;}
// 0x36
void ld_hlp_x() {;}
// 0x37
void scf() {;}
// 0x38
void jr_c_x() {;}
// 0x39
void add_hl_sp() {;}
// 0x3A
void ldd_a_hlp() {;}
// 0x3B
void dec_sp() {;}
// 0x3C
void inc_a() {;}
// 0x3D
void dec_a() {;}
// 0x3E
void ld_a_x() {;}
// 0x3F
void ccf() {;}

// --- 0x4x ---

// 0x40
void ld_b_b() {;}
// 0x41
void ld_b_c() {;}
// 0x42
void ld_b_d() {;}
// 0x43
void ld_b_e() {;}
// 0x44
void ld_b_h() {;}
// 0x45
void ld_b_l() {;}
// 0x46
void ld_b_hlp() {;}
// 0x47
void ld_b_a() {;}
// 0x48
void ld_c_b() {;}
// 0x49
void ld_c_c() {;}
// 0x4A
void ld_c_d() {;}
// 0x4B
void ld_c_e() {;}
// 0x4C
void ld_c_h() {;}
// 0x4D
void ld_c_l() {;}
// 0x4E
void ld_c_hlp() {;}
// 0x4F
void ld_c_a() {;}

// --- 0x5x ---

// 0x50
void ld_d_b() {;}
// 0x51
void ld_d_c() {;}
// 0x52
void ld_d_d() {;}
// 0x53
void ld_d_e() {;}
// 0x54
void ld_d_h() {;}
// 0x55
void ld_d_l() {;}
// 0x56
void ld_d_hlp() {;}
// 0x57
void ld_d_a() {;}
// 0x58
void ld_e_b() {;}
// 0x59
void ld_e_c() {;}
// 0x5A
void ld_e_d() {;}
// 0x5B
void ld_e_e() {;}
// 0x5C
void ld_e_h() {;}
// 0x5D
void ld_e_l() {;}
// 0x5E
void ld_e_hlp() {;}
// 0x5F
void ld_e_a() {;}

// --- 0x6x ---

// 0x60
void ld_h_b() {;}
// 0x61
void ld_h_c() {;}
// 0x62
void ld_h_d() {;}
// 0x63
void ld_h_e() {;}
// 0x64
void ld_h_h() {;}
// 0x65
void ld_h_l() {;}
// 0x66
void ld_h_hlp() {;}
// 0x67
void ld_h_a() {;}
// 0x68
void ld_l_b() {;}
// 0x69
void ld_l_c() {;}
// 0x6A
void ld_l_d() {;}
// 0x6B
void ld_l_e() {;}
// 0x6C
void ld_l_h() {;}
// 0x6D
void ld_l_l() {;}
// 0x6E
void ld_l_hlp() {;}
// 0x6F
void ld_l_a() {;}

// --- 0x7x ---

// 0x70
void ld_hlp_b() {;}
// 0x71
void ld_hlp_c() {;}
// 0x72
void ld_hlp_d() {;}
// 0x73
void ld_hlp_e() {;}
// 0x74
void ld_hlp_h() {;}
// 0x75
void ld_hlp_l() {;}
// 0x76
void halt() {;}
// 0x77
void ld_hlp_a() {;}
// 0x78
void ld_a_b() {;}
// 0x79
void ld_a_c() {;}
// 0x7A
void ld_a_d() {;}
// 0x7B
void ld_a_e() {;}
// 0x7C
void ld_a_h() {;}
// 0x7D
void ld_a_l() {;}
// 0x7E
void ld_a_hlp() {;}
// 0x7F
void ld_a_a() {;}

// --- 0x8x ---

// 0x80
void add_a_b() {;}
// 0x81
void add_a_c() {;}
// 0x82
void add_a_d() {;}
// 0x83
void add_a_e() {;}
// 0x84
void add_a_h() {;}
// 0x85
void add_a_l() {;}
// 0x86
void add_a_hlp() {;}
// 0x87
void add_a_a() {;}
// 0x88
void adc_a_b() {;}
// 0x89
void adc_a_c() {;}
// 0x8A
void adc_a_d() {;}
// 0x8B
void adc_a_e() {;}
// 0x8C
void adc_a_h() {;}
// 0x8D
void adc_a_l() {;}
// 0x8E
void adc_a_hlp() {;}
// 0x8F
void adc_a_a() {;}

// --- 0x9x ---

// 0x90
void sub_b() {;}
// 0x91
void sub_c() {;}
// 0x92
void sub_d() {;}
// 0x93
void sub_e() {;}
// 0x94
void sub_h() {;}
// 0x95
void sub_l() {;}
// 0x96
void sub_hlp() {;}
// 0x97
void sub_a() {;}
// 0x98
void sbc_a_b() {;}
// 0x99
void sbc_a_c() {;}
// 0x9A
void sbc_a_d() {;}
// 0x9B
void sbc_a_e() {;}
// 0x9C
void sbc_a_h() {;}
// 0x9D
void sbc_a_l() {;}
// 0x9E
void sbc_a_hlp() {;}
// 0x9F
void sbc_a_a() {;}

// --- 0xAx ---

// 0xA0
void and_b() {;}
// 0xA1
void and_c() {;}
// 0xA2
void and_d() {;}
// 0xA3
void and_e() {;}
// 0xA4
void and_h() {;}
// 0xA5
void and_l() {;}
// 0xA6
void and_hlp() {;}
// 0xA7
void and_a() {;}
// 0xA8
void xor_b() {;}
// 0xA9
void xor_c() {;}
// 0xAA
void xor_d() {;}
// 0xAB
void xor_e() {;}
// 0xAC
void xor_h() {;}
// 0xAD
void xor_l() {;}
// 0xAE
void xor_hlp() {;}
// 0xAF
void xor_a() {;}

// --- 0xBx ---

// 0xB0
void or_b() {;}
// 0xB1
void or_c() {;}
// 0xB2
void or_d() {;}
// 0xB3
void or_e() {;}
// 0xB4
void or_h() {;}
// 0xB5
void or_l() {;}
// 0xB6
void or_hlp() {;}
// 0xB7
void or_a() {;}
// 0xB8
void cp_b() {;}
// 0xB9
void cp_c() {;}
// 0xBA
void cp_d() {;}
// 0xBB
void cp_e() {;}
// 0xBC
void cp_h() {;}
// 0xBD
void cp_l() {;}
// 0xBE
void cp_hlp() {;}
// 0xBF
void cp_a() {;}

// --- 0xCx ---

// 0xC0
void ret_nz() {;}
// 0xC1
void pop_bc() {;}
// 0xC2
void jp_nz_xx() {;}
// 0xC3
void jp_xx() {;}
// 0xC4
void call_nz_xx() {;}
// 0xC5
void push_bc() {;}
// 0xC6
void add_a_x() {;}
// 0xC7
void rst_0() {;}
// 0xC8
void ret_z() {;}
// 0xC9
void ret() {;}
// 0xCA
void jp_z_xx() {;}
// 0xCB
void prefix_cb() {;}
// 0xCC
void call_z_xx() {;}
// 0xCD
void call_xx() {;}
// 0xCE
void adc_a_x() {;}
// 0xCF
void rst_08() {;}

// --- 0xDx ---

// 0xD0
void ret_nc() {;}
// 0xD1
void pop_de() {;}
// 0xD2
void jp_nc_xx() {;}
// 0xD4
void call_nc_xx() {;}
// 0xD5
void push_de() {;}
// 0xD6
void sub_x() {;}
// 0xD7
void rst_10() {;}
// 0xD8
void ret_c() {;}
// 0xD9
void reti() {;}
// 0xDA
void jp_c_xx() {;}
// 0xDC
void call_c_xx() {;}
// 0xDE
void sbc_a_x() {;}
// 0xDF
void rst_18() {;}

// --- 0xEx ---

// 0xE0
void ldh_x_a() {;}
// 0xE1
void pop_hl() {;}
// 0xE2
void ld_cp_a() {;}
// 0xE5
void push_hl() {;}
// 0xE6
void and_x() {;}
// 0xE7
void rst_20() {;}
// 0xE8
void add_sp_x() {;}
// 0xE9
void jp_hlp() {;}
// 0xEA
void ld_xx_a() {;}
// 0xEE
void xor_x() {;}
// 0xEF
void rst_28() {;}

// --- 0xFx ---

// 0xF0
void ldh_a_xp() {;}
// 0xF1
void pop_af() {;}
// 0xF2
void ld_a_cp() {;}
// 0xF3
void di() {;}
// 0xF5
void push_af() {;}
// 0xF6
void or_x() {;}
// 0xF7
void rst_30() {;}
// 0xF8
void ld_hl_sp_x() {;}
// 0xF9
void ld_sp_hl() {;}
// 0xFA
void ld_a_xp() {;}
// 0xFB
void ei() {;}
// 0xFE
void cp_x() {;}
// 0xFF
void rst_38() {;}

void cycle(uint8_t* m, Registers reg){

	uint8_t instruction = m[reg.PC];
	printf("\n%x",instruction);	
	
	/*
	x: 1 byte parameter
	xx: 2 byte parameter
	*p: (p)
	*/
	
	switch (instruction) {
		case (0x00):nop(); // 0x00
		case (0x01):ld_bc_xx(); // 0x01
		case (0x02):ld_bcp_a(); // 0x02
		case (0x03):inc_bc(); // 0x03
		case (0x04):inc_b(); // 0x04
		case (0x05):dec_b(); // 0x05
		case (0x06):ld_b_x(); // 0x06
		case (0x07):rlca(); // 0x07
		case (0x08):ld_xxp_sp(); // 0x08
		case (0x09):add_hl_bc(); // 0x09
		case (0x0A):ld_a_bcp(); // 0x0A
		case (0x0B):dec_bc(); // 0x0B
		case (0x0C):inc_c(); // 0x0C
		case (0x0D):dec_c(); // 0x0D
		case (0x0E):ld_c_x(); // 0x0E
		case (0x0F):rrca(); // 0x0F
		
		case (0x10):stop(); // 0x10
		case (0x11):ld_de_xx(); // 0x11
		case (0x12):ld_dep_a(); // 0x12
		case (0x13):inc_de(); // 0x13
		case (0x14):inc_d(); // 0x14
		case (0x15):dec_d(); // 0x15
		case (0x16):ld_d_x(); // 0x16
		case (0x17):rla(); // 0x17
		case (0x18):jr_x(); // 0x18
		case (0x19):add_hl_de(); // 0x19
		case (0x1A):ld_a_dep(); // 0x1A
		case (0x1B):dec_de(); // 0x1B
		case (0x1C):inc_e(); // 0x1C
		case (0x1D):dec_e(); // 0x1D
		case (0x1E):ld_e_x(); // 0x1E
		case (0x1F):rra(); // 0x1F
		
		case (0x20):jr_nz_x(); // 0x20
		case (0x21):ld_hl_xx(); // 0x21
		case (0x22):ldi_hlp_a(); // 0x22
		case (0x23):inc_hl(); // 0x23
		case (0x24):inc_h(); // 0x24
		case (0x25):dec_h(); // 0x25
		case (0x26):ld_h_x(); // 0x26
		case (0x27):daa(); // 0x27
		case (0x28):jr_z_x(); // 0x28
		case (0x29):add_hl_hl(); // 0x29
		case (0x2A):ldi_a_hlp(); // 0x2A
		case (0x2B):dec_hl(); // 0x2B
		case (0x2C):inc_l(); // 0x2C
		case (0x2D):dec_l(); // 0x2D
		case (0x2E):ld_l_x(); // 0x2E
		case (0x2F):cpl(); // 0x2F
		
		case (0x30):jp_nc_x(); // 0x30
		case (0x31):ld_sp_xx(); // 0x31
		case (0x32):ldd_hlp_a(); // 0x32
		case (0x33):inc_sp(); // 0x33
		case (0x34):inc_hlp(); // 0x34
		case (0x35):dec_hlp(); // 0x35
		case (0x36):ld_hlp_x(); // 0x36
		case (0x37):scf(); // 0x37
		case (0x38):jr_c_x(); // 0x38
		case (0x39):add_hl_sp(); // 0x39
		case (0x3A):ldd_a_hlp(); // 0x3A
		case (0x3B):dec_sp(); // 0x3B
		case (0x3C):inc_a(); // 0x3C
		case (0x3D):dec_a(); // 0x3D
		case (0x3E):ld_a_x(); // 0x3E
		case (0x3F):ccf(); // 0x3F
		
		case (0x40):ld_b_b(); // 0x40
		case (0x41):ld_b_c(); // 0x41
		case (0x42):ld_b_d(); // 0x42
		case (0x43):ld_b_e(); // 0x43
		case (0x44):ld_b_h(); // 0x44
		case (0x45):ld_b_l(); // 0x45
		case (0x46):ld_b_hlp(); // 0x46
		case (0x47):ld_b_a(); // 0x47
		case (0x48):ld_c_b(); // 0x48
		case (0x49):ld_c_c(); // 0x49
		case (0x4A):ld_c_d(); // 0x4A
		case (0x4B):ld_c_e(); // 0x4B
		case (0x4C):ld_c_h(); // 0x4C
		case (0x4D):ld_c_l(); // 0x4D
		case (0x4E):ld_c_hlp(); // 0x4E
		case (0x4F):ld_c_a(); // 0x4F
		
		case (0x50):ld_d_b(); // 0x50
		case (0x51):ld_d_c(); // 0x51
		case (0x52):ld_d_d(); // 0x52
		case (0x53):ld_d_e(); // 0x53
		case (0x54):ld_d_h(); // 0x54
		case (0x55):ld_d_l(); // 0x55
		case (0x56):ld_d_hlp(); // 0x56
		case (0x57):ld_d_a(); // 0x57
		case (0x58):ld_e_b(); // 0x58
		case (0x59):ld_e_c(); // 0x59
		case (0x5A):ld_e_d(); // 0x5A
		case (0x5B):ld_e_e(); // 0x5B
		case (0x5C):ld_e_h(); // 0x5C
		case (0x5D):ld_e_l(); // 0x5D
		case (0x5E):ld_e_hlp(); // 0x5E
		case (0x5F):ld_e_a(); // 0x5F
		
		case (0x60):ld_h_b(); // 0x60
		case (0x61):ld_h_c(); // 0x61
		case (0x62):ld_h_d(); // 0x62
		case (0x63):ld_h_e(); // 0x63
		case (0x64):ld_h_h(); // 0x64
		case (0x65):ld_h_l(); // 0x65
		case (0x66):ld_h_hlp(); // 0x66
		case (0x67):ld_h_a(); // 0x67
		case (0x68):ld_l_b(); // 0x68
		case (0x69):ld_l_c(); // 0x69
		case (0x6A):ld_l_d(); // 0x6A
		case (0x6B):ld_l_e(); // 0x6B
		case (0x6C):ld_l_h(); // 0x6C
		case (0x6D):ld_l_l(); // 0x6D
		case (0x6E):ld_l_hlp(); // 0x6E
		case (0x6F):ld_l_a(); // 0x6F
		
		case (0x70):ld_hlp_b(); // 0x70
		case (0x71):ld_hlp_c(); // 0x71
		case (0x72):ld_hlp_d(); // 0x72
		case (0x73):ld_hlp_e(); // 0x73
		case (0x74):ld_hlp_h(); // 0x74
		case (0x75):ld_hlp_l(); // 0x75
		case (0x76):halt(); // 0x76
		case (0x77):ld_hlp_a(); // 0x77
		case (0x78):ld_a_b(); // 0x78
		case (0x79):ld_a_c(); // 0x79
		case (0x7A):ld_a_d(); // 0x7A
		case (0x7B):ld_a_e(); // 0x7B
		case (0x7C):ld_a_h(); // 0x7C
		case (0x7D):ld_a_l(); // 0x7D
		case (0x7E):ld_a_hlp(); // 0x7E
		case (0x7F):ld_a_a(); // 0x7F
		
		case (0x80):add_a_b(); // 0x80
		case (0x81):add_a_c(); // 0x81
		case (0x82):add_a_d(); // 0x82
		case (0x83):add_a_e(); // 0x83
		case (0x84):add_a_h(); // 0x84
		case (0x85):add_a_l(); // 0x85
		case (0x86):add_a_hlp(); // 0x86
		case (0x87):add_a_a(); // 0x87
		case (0x88):adc_a_b(); // 0x88
		case (0x89):adc_a_c(); // 0x89
		case (0x8A):adc_a_d(); // 0x8A
		case (0x8B):adc_a_e(); // 0x8B
		case (0x8C):adc_a_h(); // 0x8C
		case (0x8D):adc_a_l(); // 0x8D
		case (0x8E):adc_a_hlp(); // 0x8E
		case (0x8F):adc_a_a(); // 0x8F
		
		case (0x90):sub_b(); // 0x90
		case (0x91):sub_c(); // 0x91
		case (0x92):sub_d(); // 0x92
		case (0x93):sub_e(); // 0x93
		case (0x94):sub_h(); // 0x94
		case (0x95):sub_l(); // 0x95
		case (0x96):sub_hlp(); // 0x96
		case (0x97):sub_a(); // 0x97
		case (0x98):sbc_a_b(); // 0x98
		case (0x99):sbc_a_c(); // 0x99
		case (0x9A):sbc_a_d(); // 0x9A
		case (0x9B):sbc_a_e(); // 0x9B
		case (0x9C):sbc_a_h(); // 0x9C
		case (0x9D):sbc_a_l(); // 0x9D
		case (0x9E):sbc_a_hlp(); // 0x9E
		case (0x9F):sbc_a_a(); // 0x9F
		
		case (0xA0):and_b(); // 0xA0
		case (0xA1):and_c(); // 0xA1
		case (0xA2):and_d(); // 0xA2
		case (0xA3):and_e(); // 0xA3
		case (0xA4):and_h(); // 0xA4
		case (0xA5):and_l(); // 0xA5
		case (0xA6):and_hlp(); // 0xA6
		case (0xA7):and_a(); // 0xA7
		case (0xA8):xor_b(); // 0xA8
		case (0xA9):xor_c(); // 0xA9
		case (0xAA):xor_d(); // 0xAA
		case (0xAB):xor_e(); // 0xAB
		case (0xAC):xor_h(); // 0xAC
		case (0xAD):xor_l(); // 0xAD
		case (0xAE):xor_hlp(); // 0xAE
		case (0xAF):xor_a(); // 0xAF
		
		case (0xB0):or_b(); // 0xB0
		case (0xB1):or_c(); // 0xB1
		case (0xB2):or_d(); // 0xB2
		case (0xB3):or_e(); // 0xB3
		case (0xB4):or_h(); // 0xB4
		case (0xB5):or_l(); // 0xB5
		case (0xB6):or_hlp(); // 0xB6
		case (0xB7):or_a(); // 0xB7
		case (0xB8):cp_b(); // 0xB8
		case (0xB9):cp_c(); // 0xB9
		case (0xBA):cp_d(); // 0xBA
		case (0xBB):cp_e(); // 0xBB
		case (0xBC):cp_h(); // 0xBC
		case (0xBD):cp_l(); // 0xBD
		case (0xBE):cp_hlp(); // 0xBE
		case (0xBF):cp_a(); // 0xBF
		
		case (0xC0):ret_nz(); // 0xC0
		case (0xC1):pop_bc(); // 0xC1
		case (0xC2):jp_nz_xx(); // 0xC2
		case (0xC3):jp_xx(); // 0xC3
		case (0xC4):call_nz_xx(); // 0xC4
		case (0xC5):push_bc(); // 0xC5
		case (0xC6):add_a_x(); // 0xC6
		case (0xC7):rst_0(); // 0xC7
		case (0xC8):ret_z(); // 0xC8
		case (0xC9):ret(); // 0xC9
		case (0xCA):jp_z_xx(); // 0xCA
		case (0xCB):prefix_cb(); // 0xCB
		case (0xCC):call_z_xx(); // 0xCC
		case (0xCD):call_xx(); // 0xCD
		case (0xCE):adc_a_x(); // 0xCE
		case (0xCF):rst_08(); // 0xCF
		
		case (0xD0):ret_nc(); // 0xD0
		case (0xD1):pop_de(); // 0xD1
		case (0xD2):jp_nc_xx(); // 0xD2
		case (0xD3):nop(); // 0xD3 blank
		case (0xD4):call_nc_xx(); // 0xD4
		case (0xD5):push_de(); // 0xD5
		case (0xD6):sub_x(); // 0xD6
		case (0xD7):rst_10(); // 0xD7
		case (0xD8):ret_c(); // 0xD8
		case (0xD9):reti(); // 0xD9
		case (0xDA):jp_c_xx(); // 0xDA
		case (0xDB):nop(); // 0xDB blank
		case (0xDC):call_c_xx(); // 0xDC
		case (0xDD):nop(); // 0xDD blank
		case (0xDE):sbc_a_x(); // 0xDE
		case (0xDF):rst_18(); // 0xDF
		
		case (0xE0):ldh_x_a(); // 0xE0
		case (0xE1):pop_hl(); // 0xE1
		case (0xE2):ld_cp_a(); // 0xE2
		case (0xE3):nop(); // 0xE3 blank
		case (0xE4):nop(); // 0xE4 blank
		case (0xE5):push_hl(); // 0xE5
		case (0xE6):and_x(); // 0xE6
		case (0xE7):rst_20(); // 0xE7
		case (0xE8):add_sp_x(); // 0xE8
		case (0xE9):jp_hlp(); // 0xE9
		case (0xEA):ld_xx_a(); // 0xEA
		case (0xEB):nop(); // 0xEB blank
		case (0xEC):nop(); // 0xEC blank
		case (0xED):nop(); // 0xED blank
		case (0xEE):xor_x(); // 0xEE
		case (0xEF):rst_28(); // 0xEF
		
		case (0xF0):ldh_a_xp(); // 0xF0
		case (0xF1):pop_af(); // 0xF1
		case (0xF2):ld_a_cp(); // 0xF2
		case (0xF3):di(); // 0xF3
		case (0xF4):nop(); // 0xF4 blank
		case (0xF5):push_af(); // 0xF5
		case (0xF6):or_x(); // 0xF6
		case (0xF7):rst_30(); // 0xF7
		case (0xF8):ld_hl_sp_x(); // 0xF8
		case (0xF9):ld_sp_hl(); // 0xF9
		case (0xFA):ld_a_xp(); // 0xFA
		case (0xFB):ei(); // 0xFB
		case (0xFC):nop(); // 0xFC blank
		case (0xFD):nop(); // 0xFD blank
		case (0xFE):cp_x(); // 0xFE
		case (0xFF):rst_38(); // 0xFF
	
	}
		
	

}
	
