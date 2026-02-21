#ifndef AUDIO_H
#define AUDIO_H

#include <stdint.h>
#include "memory.h"

typedef enum {
    // channel 1 (square) registers
    NR10 = 0x10, 
    NR11 = 0x11,
    NR12 = 0x12,
    NR13 = 0x13,
    NR14 = 0x14,
    
    // channel 2 (square) registers
    NR21 = 0x16,
    NR22 = 0x17,
    NR23 = 0x18,
    NR24 = 0x19,

    // channel 3 (wave) registers
    NR30 = 0x1A,
    NR31 = 0x1B,
    NR32 = 0x1C,
    NR33 = 0x1D,
    NR34 = 0x1E,

    // channel 4 (noise) registers
    NR41 = 0x20,
    NR42 = 0x21,
    NR43 = 0x22,
    NR44 = 0x23,

} AudioRegisters;

#define SAMPLE_RATE 44100 // 44.1KHz audio
#define CYCLES_PER_SAMPLE (4194304 / SAMPLE_RATE)
#define AUDIO_BUFFER_SIZE 1024

typedef struct {
    int volume; // 0 - 15
    int env_timer; // envelope timer
    int env_pace; // envelope 
    int env_dir; // 1 = fade in, 0 = fade out
} Envelope;

typedef struct {
    int ch_number; // channel number 1-4
    int enabled; 
    int timer; // countdown per cpu cycle
    
    int length_timer;
    int length_enabled;

    // pointers to channel registers
    uint8_t* nrx0;
    uint8_t* nrx1;
    uint8_t* nrx2;
    uint8_t* nrx3;
    uint8_t* nrx4;

    int var;
    // ch1,2:   duty step -chooses sound profile for pulse channels 1,2
    // ch3:     posiion - controls which of the 32 waves in RAM to use
    // ch4:     lfsr - 15 bit random noise

    Envelope env; // for volume fading (ch1,2,4)

} Channel;

typedef struct {
    uint32_t device; // host pc's audio device for sdl
    int sample_timer; // counts down every cycle, calculating audio sample at 0
    int frame_seq_timer; // 512hz apu clock
    int frame_seq_step; // tracks current state of apu

    float buffer[AUDIO_BUFFER_SIZE]; // save 1024 samples to a buffer to prevent stuttering
    int buffer_index; // next slot in buffer

    Channel ch1; // pulse 1
    Channel ch2; // pulse 2
    Channel ch3; // wave
    Channel ch4; // noise
} Audio;

void audio_init(Audio* a, Memory* m);
void audio_step(Audio* a, Memory* m, int cycles);

#endif