#include "audio.h"
#include <SDL2/SDL.h>
#include <stdio.h>

// controls the waveform. per cycle - 1 is low 0 is high
static const int duty_table[4][8] = { 
    {0, 0, 0, 0, 0, 0, 0, 1}, // 12.5%
    {1, 0, 0, 0, 0, 0, 0, 1}, // 25%
    {1, 0, 0, 0, 0, 1, 1, 1}, // 50%
    {0, 1, 1, 1, 1, 1, 1, 0}  // 75% (inverted 25% - no audible difference)
};

void channels_init(Audio* a, Memory* m) {
    
    // set all channels and data to 0
    SDL_memset(&a->ch1, 0, sizeof(Channel));
    SDL_memset(&a->ch2, 0, sizeof(Channel));
    SDL_memset(&a->ch3, 0, sizeof(Channel));
    SDL_memset(&a->ch4, 0, sizeof(Channel));

    // assign channel no.
    a->ch1.ch_number = 1;
    a->ch2.ch_number = 2;
    a->ch3.ch_number = 3;
    a->ch4.ch_number = 4;

    // set channel register pointers
    a->ch1.nrx0 = NR10;
    a->ch1.nrx1 = NR11;
    a->ch1.nrx2 = NR12;
    a->ch1.nrx3 = NR13;
    a->ch1.nrx4 = NR14;

    a->ch2.nrx0 = NR21 - 1; // no nr20 reg
    a->ch2.nrx1 = NR21;
    a->ch2.nrx2 = NR22;
    a->ch2.nrx3 = NR23;
    a->ch2.nrx4 = NR24;

    a->ch3.nrx0 = NR30;
    a->ch3.nrx1 = NR31;
    a->ch3.nrx2 = NR32;
    a->ch3.nrx3 = NR33;
    a->ch3.nrx4 = NR34;

    a->ch4.nrx0 = NR41 - 1; // no nr40 reg
    a->ch4.nrx1 = NR41;
    a->ch4.nrx2 = NR42;
    a->ch4.nrx3 = NR43;
    a->ch4.nrx4 = NR44;
}

void sdl_audio_init(Audio* a) {
    SDL_InitSubSystem(SDL_INIT_AUDIO); 

    SDL_AudioSpec want, have;
    SDL_zero(want);
    want.freq = SAMPLE_RATE;
    want.format = AUDIO_F32SYS;
    want.channels = 2;
    want.samples = 512;

    a->device = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (a->device) {
        SDL_PauseAudioDevice(a->device, 0);
    }
}

void audio_init(Audio* a, Memory* m) {
    a->sample_timer = CYCLES_PER_SAMPLE;
    a->frame_seq_timer = 8192; // 4194304 / 512Hz
    a->frame_seq_step = 0;
    a->buffer_index = 0;
    
    write8(m, NR14, read8(m, NR14) & 0x7F);
    write8(m, NR24, read8(m, NR24) & 0x7F);
    write8(m, NR34, read8(m, NR34) & 0x7F);
    write8(m, NR44, read8(m, NR44) & 0x7F);

    channels_init(a, m); // init 4 channels
    sdl_audio_init(a);   // start sdl audio
    
}

// handles trigger event (nrx4 bit 7)
// nrx2 controls volume and envelope

void trigger_envelope(Envelope* env, Memory* m, uint16_t nrx2){
    env->volume = (read8(m, nrx2) >> 4) & 0x0F; // volume is set to nrx2 initial volume
    env->env_dir = (read8(m, nrx2) >> 3) & 0x01; // 1 = fade in, 0 = fade out
    env->env_pace = read8(m, nrx2) & 0x07;
    env->env_timer = env->env_pace;
}

void trigger(Channel* ch, Memory* m) {
    ch->enabled = 1;
    // channel 3
    if (ch->ch_number == 3) {ch->var = 0; return;} // restart wave playback. returns since ch3 has no envelope
    // all other channels
    if (ch->ch_number == 1) {
        int sweep_period = (read8(m, ch->nrx0) >> 4) & 0x07;
        ch->sweep_timer = (sweep_period > 0) ? sweep_period : 8;
    }
    if (ch->ch_number == 4) {ch->var = 0x7FFF;} // 15-bits of 1s, otherwise silence
    trigger_envelope(&ch->env, m, ch->nrx2);
}

// 64hz volume envelope
// automatically adjusts volume over time
// handles things like sounds fading out
void clock_envelope(Envelope* env) {
    if (env->env_pace == 0) return; // envelope disabled

    env->env_timer--;
    if (env->env_timer <= 0) {
        env->env_timer = env->env_pace;

        if (env->env_dir && env->volume < 15) {
            env->volume++;
        } else if (!env->env_dir && env->volume > 0) {
            env->volume--;
        }
    }
}

// mutes audio if it exceeds its length
void clock_length(Channel* ch, Memory* m) {
    if (ch->length_enabled && ch->length_timer > 0) {
        (ch->length_timer)--;
        if (ch->length_timer == 0) {
            ch->enabled = 0;

            // clear the channel bit in nr52
            uint8_t mask = ~(1 << (ch->ch_number - 1));
            write8(m, NR52, ( read8(m, NR52) & mask ));
        }
    }
}

void clock_sweep(Channel* ch, Memory* m) {
    if (ch->ch_number != 1) return;

    uint8_t nr10 = read8(m, ch->nrx0);
    int sweep_period = (nr10 >> 4) & 0x07;
    int sweep_shift = nr10 & 0x07;
    int sweep_dir = (nr10 >> 3) & 0x01;

    if (sweep_period == 0) return;

    ch->sweep_timer--;
    if (ch->sweep_timer <= 0) {
        ch->sweep_timer = sweep_period == 0 ? 8 : sweep_period;
        
        int freq = read8(m, ch->nrx3) | ((read8(m, ch->nrx4) & 0x07) << 8);
        int offset = freq >> sweep_shift;
        int new_freq = sweep_dir ? (freq - offset) : (freq + offset);
        
        if (new_freq > 2047) {
            ch->enabled = 0; 
        } else if (sweep_shift != 0) {
            write8(m, ch->nrx3, new_freq & 0xFF);
            write8(m, ch->nrx4, (read8(m, ch->nrx4) & 0xF8) | ((new_freq >> 8) & 0x07));
        }
    }
}

int get_sample(Channel* ch, Memory* m, int cycles) {
    if (!ch->enabled) return 0; // silence if channel disabled

    // channel 1 & 2 (square channel)
    if (ch->ch_number < 3) {
        if (ch->env.volume == 0) return 0;

        int freq = read8(m, ch->nrx3) | ((read8(m, ch->nrx4) & 0x07) << 8);
        int period = (2048 - freq) * 4;
        
        ch->timer -= cycles;
        
        while (ch->timer <= 0) {
            ch->timer += (period > 0) ? period : 4; 
            ch->var = (ch->var + 1) % 8;
        }

        int duty = (read8(m, ch->nrx1) >> 6) & 0x03;
        return duty_table[duty][ch->var];
    }
    // channel 3 (wave channel)
    else if (ch->ch_number == 3) {
        if (!(read8(m, ch->nrx0) & 0x80)) return 0; // silence if channel disabled

        int freq = read8(m, ch->nrx3) | ((read8(m, ch->nrx4) & 0x07) << 8);
        int period = (2048 - freq) * 2; 
        
        ch->timer -= cycles;
        while (ch->timer <= 0) {
            ch->timer += (period > 0) ? period : 2;
            ch->var = (ch->var + 1) % 32; // 32 samples in wave RAM
        }

        // 16bytes at 0xff30. 2 samples per byte
        uint8_t byte = read8(m, WAVE_DATA_START + (ch->var / 2) );
        
        // top 4 bits for even pos, bottom 4 bits for odd pos
        uint8_t sample = (ch->var % 2 == 0) ? (byte >> 4) : (byte & 0x0F);

        // volume shifting
        int vol_code = (read8(m, ch->nrx2) >> 5) & 0x03;

        switch(vol_code) {
            case 0: return 0; // volume 0%
            case 1: return sample; // volume 100%
            case 2: return sample >> 1; // volume 50%
            case 3: return sample >> 2; // volume 25%
        }
        return 0;
    }
    // channel 4 (noise channel)
    else if (ch->ch_number == 4) {
        if (ch->env.volume == 0) return 0;

        // decode nr43
        int shift = read8(m, ch->nrx3) >> 4;
        int divisor_code = read8(m, ch->nrx3) & 0x07;
        int divisor = (divisor_code == 0) ? 8 : (divisor_code * 16);
        int period = (divisor << shift); 

        ch->timer -= cycles;
        while (ch->timer <= 0) {
            ch->timer += (period > 0) ? period : 8;

            // xor bottom 2 bits
            // shift entire register right
            // put reuslt in top bit
            int xor_bit = (ch->var & 0x01) ^ ((ch->var & 0x02) >> 1);
            ch->var >>= 1; 
            ch->var |= (xor_bit << 14); 

            // short mode enabled by nr43
            if (read8(m, ch->nrx3) & 0x08) {
                ch->var &= ~(1 << 6); 
                ch->var |= (xor_bit << 6); // put result in bit 7 also
                // makes sound more robotic
            }
        }

        // Output is 0 if the lowest bit of the LFSR is 1, and 1 if it is 0.
        return (~ch->var) & 0x01; 
    }
}

void detect_trigger(Channel* ch, Memory* m) {

    if (read8(m, ch->nrx4) & 0x80) { // bit 7 of NRx4 triggers the channel
        
        if (ch->ch_number == 1) write8(m, NR52, ( read8(m, NR52) & 0x01 ));
        if (ch->ch_number == 2) write8(m, NR52, ( read8(m, NR52) & 0x02 ));
        if (ch->ch_number == 3) write8(m, NR52, ( read8(m, NR52) & 0x04 ));
        if (ch->ch_number == 4) write8(m, NR52, ( read8(m, NR52) & 0x08 ));

        trigger(ch, m); 

        // enable length
        if (read8(m,ch->nrx4) & 0x40) ch->length_enabled = 1;
        else ch->length_enabled = 0;

        int max_length = 64;
        int length_data = read8(m, ch->nrx1) & 0x3F;

        if (ch->ch_number == 3) {
            max_length = 256;
            length_data = read8(m, ch->nrx1); 
        }

        if (ch->length_timer == 0) ch->length_timer = max_length - length_data;

        // set reg to 1s
        write8(m, ch->nrx4, ( read8(m, ch->nrx4) & 0x7F ));

    }

}

void audio_step(Audio* a, Memory* m, int cycles) {
    if (!a->device) return; // skip if no audio device
    if (!(read8(m, NR52) & 0x80)) { // apu is off
        a->ch1.enabled = 0;
        a->ch2.enabled = 0;
        a->ch3.enabled = 0;
        a->ch4.enabled = 0;
        return;
    }

    // detect triggers
    detect_trigger(&a->ch1, m);
    detect_trigger(&a->ch2, m);
    detect_trigger(&a->ch3, m);
    detect_trigger(&a->ch4, m);

    // step the frame sequencer
    a->frame_seq_timer -= cycles;
    while (a->frame_seq_timer <= 0) {
        a->frame_seq_timer += 8192;

        // tick length counters on even steps
        if (a->frame_seq_step % 2 == 0) {
            clock_length(&a->ch1, m);
            clock_length(&a->ch2, m);
            clock_length(&a->ch3, m);
            clock_length(&a->ch4, m);
        }
        // tick frequency sweep on steps 2 and 6
        if (a->frame_seq_step == 2 || a->frame_seq_step == 6) {
            clock_sweep(&a->ch1, m);
        }

        // volume envelopes on step 7
        if (a->frame_seq_step == 7) {
            clock_envelope(&a->ch1.env);
            clock_envelope(&a->ch2.env);
            clock_envelope(&a->ch4.env);
        }
        a->frame_seq_step = (a->frame_seq_step + 1) % 8;
    }

    // generate all samples that should have occured during cycles
    a->sample_timer -= cycles;

    while (a->sample_timer <= 0) {
        a->sample_timer += CYCLES_PER_SAMPLE;

        // separate left and right samples
        float sample_l = 0.0f;
        float sample_r = 0.0f;
        uint8_t nr51 = read8(m, NR51);

        if (a->ch1.enabled && (read8(m, a->ch1.nrx2) & 0xF8) != 0) {
            float volume = (a->ch1.env.volume / 15.0f);
            if (!get_sample(&a->ch1, m, CYCLES_PER_SAMPLE)) volume = -volume;
            if (nr51 & 0x10) sample_l += volume; // Mix to Left
            if (nr51 & 0x01) sample_r += volume; // Mix to Right
        }
        if (a->ch2.enabled && (read8(m, a->ch2.nrx2) & 0xF8) != 0) {
            float volume = (a->ch2.env.volume / 15.0f);
            if (!get_sample(&a->ch2, m, CYCLES_PER_SAMPLE)) volume = -volume;
            if (nr51 & 0x20) sample_l += volume;
            if (nr51 & 0x02) sample_r += volume;
        }
        if (a->ch3.enabled && (read8(m, a->ch3.nrx0) & 0x80) != 0) {
            float wave = get_sample(&a->ch3, m, CYCLES_PER_SAMPLE);
            float ch3_out = (wave / 7.5f) - 1.0f;
            if (nr51 & 0x40) sample_l += ch3_out;
            if (nr51 & 0x04) sample_r += ch3_out;
        }
        if (a->ch4.enabled && (read8(m, a->ch4.nrx2) & 0xF8) != 0) {
            float volume = 0.1 * (a->ch4.env.volume / 15.0f);
            if (!get_sample(&a->ch4, m, CYCLES_PER_SAMPLE)) volume = -volume;
            if (nr51 & 0x80) sample_l += volume;
            if (nr51 & 0x08) sample_r += volume;
        }
        
        // scale samples
        sample_l *= 0.1f; 
        sample_r *= 0.1f;

        if (a->buffer_index > AUDIO_BUFFER_SIZE) {
            a->buffer_index = 0; // reset if buffer overflowed
        }

        a->buffer[a->buffer_index++] = sample_l;
        a->buffer[a->buffer_index++] = sample_r;

        // add samples to sdl audio queue
        if (a->buffer_index >= AUDIO_BUFFER_SIZE) {
            if (SDL_GetQueuedAudioSize(a->device) < AUDIO_BUFFER_SIZE * sizeof(float) * 2) {
                SDL_QueueAudio(a->device, a->buffer, AUDIO_BUFFER_SIZE * sizeof(float));
            }
            a->buffer_index = 0;
        }
    }
}