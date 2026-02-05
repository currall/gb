
#include <stdint.h>
#include <stdio.h>

#include "memory.h"
#include "timer.h"

void timer_step(Memory* m) {
    uint16_t old_div = m->div_counter;
    m->div_counter++;
    raw_write(m,DIV,m->div_counter >> 8);

    // tima reload
    if (m->tima >= 0) {
        m->tima--;
        if (m->tima == 0) {
            raw_write(m, TIMA, raw_read(m,TMA));
            uint8_t iflags = raw_read(m, 0xFF0F);
            iflags |= 0x04;
            raw_write(m, 0xFF0F, iflags);
            m->tima = -1;
        }
    }

    int timer_enabled = (raw_read(m,TAC) & 0x04);

    int bit = 0;
    switch (raw_read(m,TAC) & 0x03) {
        case 0: bit = 9; break; // 4096 Hz
        case 1: bit = 3; break; // 262144 Hz
        case 2: bit = 5; break; // 65536 Hz
        case 3: bit = 7; break; // 16384 Hz
    }
    
    int old_bit = (old_div >> bit) & 1;
    int new_bit = (m->div_counter >> bit) & 1;
    
    int old_signal = old_bit && timer_enabled;
    int new_signal = new_bit && timer_enabled;

    if (old_signal && !new_signal) {
        if (raw_read(m,TIMA) == 0xFF) {
            raw_write(m,TIMA,0x00);
            m->tima = 4; 
        } else {
            uint8_t temp_tima = raw_read(m,TIMA);
            temp_tima++;
            raw_write(m,TIMA,temp_tima);
        }
    }
}