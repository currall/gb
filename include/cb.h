// cb.h
#ifndef CB_H
#define CB_H

typedef struct Memory Memory;
typedef struct Registers Registers;

void execute_cb(uint8_t opcode, Registers* reg, Memory* m, int* cycles);

#endif