
#include <stdint.h>
#include <stdio.h>

#include "memory.h"
#include "timer.h"

void timer_step2(Memory* m) {
	
	uint16_t old_div = m->div_counter;
	m->div_counter++;
	m->memory[DIV] = m->div_counter >> 8;
	
	int bit;
	switch (read8(m, TAC) & 0x03) {
		case 0: bit = 9; break;
		case 1: bit = 3; break;
		case 2: bit = 5; break;
		case 3: bit = 7; break;
	}
	
	int old_bit = (old_div >> bit) & 1;
	int new_bit = (m->div_counter >> bit) & 1;
	
	if ((read8(m, TAC) & 0x04) && old_bit == 1 && new_bit == 0) {
		m->tima++;
	}
	
	// div
    m->div_counter ++;
	m->memory[DIV] = m->div_counter >> 8;

	// tac
    if (read8(m, TAC) & 0x04) { // check bit 2 of tac
        int period = 0;
        switch (read8(m, TAC) & 0x03) { // timer speed
            case 0x0: period = 1024; break;
            case 0x1: period = 16; break;
            case 0x2: period = 64; break;
            case 0x3: period = 256; break;
        }
		
		// tima

        if (m->tima >= period) {
            m->tima -= period;
            if (read8(m, TIMA) == 0xFF) {
				write8(m, TIMA, read8(m, TMA));
                m->memory[0xFF0F] |= 0x04;
            } else {
                m->memory[TIMA]++;
            }
        }
    }
}
void timer_step(Memory* m) {
    uint16_t old_div = m->div_counter;
    m->div_counter++;
    m->memory[DIV] = m->div_counter >> 8;

    // tima reload
    if (m->tima >= 0) {
        m->tima--;
        if (m->tima == 0) {
            m->memory[TIMA] = m->memory[TMA];
            m->memory[0xFF0F] |= 0x04;   // Timer interrupt
            m->tima = -1;
        }
    }

    // timer enabled
    if (!(m->memory[TAC] & 0x04))
        return;

    int bit;
    switch (m->memory[TAC] & 0x03) {
        case 0: bit = 9; break; // 4096 Hz
        case 1: bit = 3; break; // 262144 Hz
        case 2: bit = 5; break; // 65536 Hz
        case 3: bit = 7; break; // 16384 Hz
    }

    int old_bit = (old_div >> bit) & 1;
    int new_bit = (m->div_counter >> bit) & 1;

    // tima increment
    if (old_bit == 1 && new_bit == 0) {
        if (m->memory[TIMA] == 0xFF) {
            m->memory[TIMA] = 0x00;
            m->tima = 4;
        } else {
            m->memory[TIMA]++;
        }
    }
}
