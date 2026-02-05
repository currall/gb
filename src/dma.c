#include <stdint.h>

#include "memory.h"


void dma_step(Memory *m) {

    // Start DMA one T-cycle after FF46 write
    if (m->dma_pending) {
        m->dma_pending = 0;
        m->dma_active = 1;
        m->dma_index = 0;
        m->dma_cycles_left = 640;   // 160 M-cycles × 4
        return; // hardware delay before first byte
    }

    if (!m->dma_active) return;

    // Every 4 T-cycles = 1 M-cycle → copy one byte
    if ((m->dma_cycles_left % 4) == 0) {
        uint16_t src = m->dma_source + m->dma_index;
        uint16_t dst = 0xFE00 + m->dma_index;

        raw_write(m,dst,raw_read(m, src));
        m->dma_index++;
    }

    m->dma_cycles_left--;

    if (m->dma_cycles_left == 0) {
        m->dma_active = 0;
    }
}
