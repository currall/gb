#include <stdint.h>

#include "memory.h"


void dma_step(Memory *m, int cycles) {

    while (cycles > 0) {
        // wait 1 cycle after 0xff46 write
        if (m->dma_pending) {
            m->dma_pending = 0;
            m->dma_active = 1;
            m->dma_index = 0;
            m->dma_cycles_left = 640; 
            cycles--;
            continue; // hardware delay before first byte
        }

        if (!m->dma_active) return;

        // copy byte every 4 cycles
        if ((m->dma_cycles_left % 4) == 0) {
            uint16_t src = m->dma_source + m->dma_index;
            uint16_t dst = 0xFE00 + m->dma_index;

            raw_write(m,dst,raw_read(m, src));
            m->dma_index++;
        }

        m->dma_cycles_left--;
        cycles--;

        if (m->dma_cycles_left == 0) {
            m->dma_active = 0;
        }
    }
}
