
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "debug.h"
#include "memory.h"
#include "registers.h"

void status_init(Status* s) {

	s->running = 1;
	s->paused = 0;
	s->advance_frame = 0;

	s->print_cycle = 0;
	s->print_frame = 0;
	s->print_memory = 0;
	
	s->show_instruction = 	1;
	s->show_operands = 		1;
	s->show_cpu = 			1;
	s->show_registers = 	1;
	s->show_ppu = 			0;
	s->show_mbc = 			1;
	s->show_ime = 			1;
	s->show_cycles = 		1;
	
	s->frame_time = 1.0 / 60.0;
	
}

void print_table_header(Status* s) {
	
	char line1[256] = "";
	char line2[256] = "";
	char line3[256] = "";
	char line4[256] = "";
	
	if (s->show_instruction) {
		strcat(line1, "==================");
		strcat(line2, "instruction     | ");
		strcat(line3, "[no:opcode]     | ");
		strcat(line4, "================|=");
	}
	if (s->show_operands) {
		strcat(line1, "================");
		strcat(line2, "8bit | 16 bit | ");
		strcat(line3, "opnd | oprand | ");
		strcat(line4, "==============|=");
	}
	if (s->show_cpu) {
		strcat(line1, "========================");
		strcat(line2, "PC        | SP        | ");
		strcat(line3, "ProgCount | StackPntr | ");
		strcat(line4, "==========|===========|=");
	}
	if (s->show_ime) {
		strcat(line1, "==========================================");
		strcat(line2, "Interrupts      | Interrupt Registers   | ");
		strcat(line3, "IME | IME_delay | IF:0xFF0F | IE:0xFFFF | ");
		strcat(line4, "====|===========|===========|===========|=");
	}
	if (s->show_registers) {
		strcat(line1, "================================================");
		strcat(line2, "Register Values                               | ");
		strcat(line3, "AF        | BC        | DE        | HL        | ");
		strcat(line4, "==========|===========|===========|===========|=");
	}
	if (s->show_ppu) {
		strcat(line1, "=========================================");
		strcat(line2, "Register Values                        | ");
		strcat(line3, "LCDC        | STAT        | BGP:0xFF47 | ");
		strcat(line4, "============|=============|============|=");
	}
	if (s->show_mbc) {
		strcat(line1, "==========");
		strcat(line2, "MBC1    | ");
		strcat(line3, "bank no | ");
		strcat(line4, "========|=");
	}
	if (s->show_cycles) {
		strcat(line1, "================");
		strcat(line2, "total         | ");
		strcat(line3, "cycles passed | ");
		strcat(line4, "==============|=");
	}
	
	printf("%s\n", line1);
    printf("%s\n", line2);
    printf("%s\n", line3);
    printf("%s\n", line4);
	
}

void print_cycle(Registers* reg, Memory* m, int instruction, uint64_t cycles, Status* s) {
	
	if (s->show_instruction) printf("%.9d: 0x%02x | ",instruction, raw_read(m,reg->PC));
	if (s->show_operands) printf("0x%02x | 0x%04x | ", raw_read(m,reg->PC+1),raw_read(m,reg->PC + 1) + (raw_read(m,reg->PC + 2) << 8));
	if (s->show_cpu) printf("PC:0x%04x | SP:0x%04x | ",reg->PC, reg->SP);
	if (s->show_ime) {printf(
		"%-1d   | %-1d         | IF:0x%04x | IE:0x%04x | ",
		reg->IME, 
		reg->IME_delay,
		raw_read(m,0xFF0F),
		raw_read(m,0xFFFF)
	);}
	if (s->show_registers) {printf(
		"AF:0x%04x | BC:0x%04x | "
		"DE:0x%04x | HL:0x%04x | ",
		reg->AF,
		reg->BC,
		reg->DE,
		reg->HL
	);}
	if (s->show_ppu) {printf(
		"LCDC:0x%04x | STAT:0x%04x | "
		"BGP:0x%04x | ",
		raw_read(m,0xFF40),
		raw_read(m,0xFF41),
		raw_read(m,0xFF47)
	);}
	if (s->show_mbc) printf("%d       | ", ((m->mbc1_bank2 >> 5) + m->mbc1_bank1));
	if (s->show_cycles) printf("%-13d | ",cycles);
	printf("\n");
}


void print_memory(Memory* m) {
	
	printf("\n[MEMORY] Debug print out started!\n");

	printf("\n=== MEMORY FLAGS ===\n");
	printf("Boot ROM Enabled? :: %d\n", m->boot_rom_enabled);
	
	printf("=== DIV & TIMERS ===\n");
	printf("DIV counter: %d\n",m->div_counter);
	printf("TIMA: %d\n",m->tima);
	
	printf("=== DMA ===\n");
	printf("DMA active: %d\n",m->dma_active);
	printf("DMA pending: %d\n",m->dma_pending);
	printf("DMA source: 0x%x\n",m->dma_source);
	printf("DMA index: 0x%x\n",m->dma_index);
	printf("Cycles Remaining: %d\n",m->dma_cycles_left);
	
	printf("\n=== BOOT ROM ===\n");
	
	for (int i=0; i< 0x100; i++){
		printf("%02x ",m->boot_rom[i]);
	}
	
	printf("\n=== MEMORY ADDRESS SPACE ===\n");
	
	printf("\n[MEMORY] Debug print out ended!\n");
	
}