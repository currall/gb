//timer.h
#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#include "registers.h"

#define DIV 0xFF04
#define TIMA 0xFF05
#define TMA 0xFF06
#define TAC 0xFF07

void timer_step(Memory* m);

#endif