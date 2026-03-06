#ifndef AUDIO_H
#define AUDIO_H

#include <stdint.h>

typedef struct Memory Memory;

typedef enum {
    // channel 1 (square) registers
    NR10 = 0xFF10, 
    NR11 = 0xFF11,
    NR12 = 0xFF12,
    NR13 = 0xFF13,
    NR14 = 0xFF14,
    
    // channel 2 (square) registers
    NR21 = 0xFF16,
    NR22 = 0xFF17,
    NR23 = 0xFF18,
    NR24 = 0xFF19,

    // channel 3 (wave) registers
    NR30 = 0xFF1A,
    NR31 = 0xFF1B,
    NR32 = 0xFF1C,
    NR33 = 0xFF1D,
    NR34 = 0xFF1E,

    // channel 4 (noise) registers
    NR41 = 0xFF20,
    NR42 = 0xFF21,
    NR43 = 0xFF22,
    NR44 = 0xFF23,

    // global control registers
    NR50 = 0xFF24,
    NR51 = 0xFF25,
    NR52 = 0xFF26,

    // wave data start
    WAVE_DATA_START = 0xFF30

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

    int sweep_timer; // channel 1 sweep

    // addresses of channel registers
    uint16_t nrx0;
    uint16_t nrx1;
    uint16_t nrx2;
    uint16_t nrx3;
    uint16_t nrx4;

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