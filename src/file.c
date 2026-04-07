#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "tinyfiledialogs.h"

#include "memory.h"
#include "ppu.h"
#include "registers.h"

uint32_t get_file_size(FILE* f) {
    // file size
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (size <= 0) {
        printf("File is empty or invalid\n");
        fclose(f);
        return 0;
    }

    uint32_t file_size = (uint32_t)size;
}

uint32_t load_file(char* file, uint8_t** m, int warn) {
    FILE* f = fopen(file, "rb");
    if (!f) {
        if (warn) perror("Failed to open file");
        return 0;
    }

    uint32_t file_size = get_file_size(f);

    *m = (uint8_t*)malloc(file_size);
    if (!*m) {
        if (warn) perror("Failed to allocate memory");
        fclose(f);
        return 0;
    }

    // read data
    size_t read_count = fread(*m, 1, file_size, f);
    fclose(f);

    if (read_count != file_size) {
        if (warn) printf("Error: Read partial file (Expected %d, got %d)\n", file_size, read_count);
        free(*m);
        *m = NULL;
        return 0;
    }

    printf("[FILE] Loaded file: %s (%d bytes)\n", file, file_size);
    return file_size;
}

char* savefile_name(char *file) {
    if (file == NULL) return NULL;

    size_t filename_len = strlen(file) + 5;
    char* savefile = (char *)malloc(filename_len);
    
    if (savefile == NULL) {
        return NULL; 
    }
    
    snprintf(savefile, filename_len, "%s.sav", file);
    return savefile;
}
char* save_state_name(char *file) {
    if (file == NULL) return NULL;

    size_t filename_len = strlen(file) + 7;
    char* savefile = (char *)malloc(filename_len);
    
    if (savefile == NULL) {
        return NULL; 
    }
    
    snprintf(savefile, filename_len, "%s.state", file);
    return savefile;
}

void save_game(char* file, uint8_t* eram, uint32_t ram_size) {
    char *savefile = savefile_name(file);

    // open file
    FILE *fp = fopen(savefile, "wb");
    if (fp == NULL) {
        perror("[FILE] Error writing save file!");
        free(savefile);
        return;
    }

    // write eram to file
    uint32_t items_written = fwrite(eram, sizeof(uint8_t), ram_size, fp);

    // close file
    fclose(fp);
    free(savefile);
    printf("[FILE] Saved to file: %s (%d / %d bytes)\n", savefile, items_written , ram_size);

}

void load_game(char* file, uint8_t* eram, uint32_t ram_size) {
    if (file == NULL || eram == NULL) return;

    char *savefile = savefile_name(file);
    if (savefile == NULL) {
        return;
    }

    FILE *fp = fopen(savefile, "rb");
    if (fp == NULL) {
        perror("[FILE] No save file detected");
        free(savefile);
        return;
    }
    
    fread(eram, sizeof(uint8_t), ram_size, fp); // read data
    printf("[FILE] Loaded save file: %s (%d bytes)\n", savefile, ram_size);
    fclose(fp);

}

void save_state(char* f, Memory* m, Registers* reg) {

	const char* filter[] = {"*.state"};
    char* state_file = save_state_name(f);

    char* file = tinyfd_saveFileDialog(
        "Save emulator state",
        state_file,
        1,
        filter,
        "Save state files"
    );

    // open file
    FILE *fp = fopen(file, "wb");
    if (fp == NULL) {
        perror("[FILE] Error writing save state!");
        return;
    }

    // === REGISTER STATE ===
    fwrite(&reg->AF, sizeof(uint16_t), 1, fp);
    fwrite(&reg->BC, sizeof(uint16_t), 1, fp);
    fwrite(&reg->DE, sizeof(uint16_t), 1, fp);
    fwrite(&reg->HL, sizeof(uint16_t), 1, fp);
    fwrite(&reg->SP, sizeof(uint16_t), 1, fp);
    fwrite(&reg->PC, sizeof(uint16_t), 1, fp);
    // -- interrupts
    fwrite(&reg->IME, sizeof(uint8_t), 1, fp);
    fwrite(&reg->IME_delay, sizeof(uint8_t), 1, fp);
    // -- halt
    fwrite(&reg->halted, sizeof(uint8_t), 1, fp);
    fwrite(&reg->halt_bug, sizeof(uint8_t), 1, fp);
    fwrite(&reg->pc_changed, sizeof(uint8_t), 1, fp);

    // === MEMORY ===
    // -- ppu
    fwrite(m->ppu, sizeof(PPU), 1, fp);
    // -- registers
    fwrite(&m->cgb_speed, sizeof(uint8_t), 1, fp);
    fwrite(&m->vram_bank, sizeof(uint8_t), 1, fp);
    fwrite(&m->wram_bank, sizeof(uint8_t), 1, fp);

    fwrite(&m->mbc_bank1, sizeof(uint16_t), 1, fp);
    fwrite(&m->mbc_bank2, sizeof(uint8_t), 1, fp);
    fwrite(&m->mbc1_mode, sizeof(uint8_t), 1, fp);
    fwrite(&m->mbc_ram_enable, sizeof(uint8_t), 1, fp);
    
    // -- memory
    fwrite(m->vram, sizeof(uint8_t), 0x4000, fp);
    fwrite(m->eram, sizeof(uint8_t), m->ram_size, fp);
    fwrite(m->wram, sizeof(uint8_t), 0x8000, fp);
    fwrite(&m->oam, sizeof(uint8_t), 0xA0, fp);
    fwrite(&m->io, sizeof(uint8_t), 0x80, fp);
    fwrite(&m->hram, sizeof(uint8_t), 0x7F, fp);
    fwrite(&m->ie, sizeof(uint8_t), 1, fp);

    // close file
    fclose(fp);
    printf("[FILE] Saved state to file: %s\n",file);
}

void load_state(char* f, Memory* m, Registers* reg) {

	const char* filter[] = {"*.state"};
    char* state_file = save_state_name(f);

    char* file = tinyfd_openFileDialog(
        "Load emulator state",
        state_file,
        1,
        filter,
        "Save state files", 
        0
    );

    // open file
    FILE *fp = fopen(file, "rb");
    if (fp == NULL) {
        perror("[FILE] Error reading save state!");
        return;
    }

    // === REGISTER STATE ===
    fread(&reg->AF, sizeof(uint16_t), 1, fp);
    fread(&reg->BC, sizeof(uint16_t), 1, fp);
    fread(&reg->DE, sizeof(uint16_t), 1, fp);
    fread(&reg->HL, sizeof(uint16_t), 1, fp);
    fread(&reg->SP, sizeof(uint16_t), 1, fp);
    fread(&reg->PC, sizeof(uint16_t), 1, fp);
    // -- interrupts
    fread(&reg->IME, 1, 1, fp);
    fread(&reg->IME_delay, 1, 1, fp);
    // -- halt
    fread(&reg->halted, 1, 1, fp);
    fread(&reg->halt_bug, 1, 1, fp);
    fread(&reg->pc_changed, 1, 1, fp);

    // === MEMORY ===
    // -- ppu
    fread(m->ppu, sizeof(PPU), 1, fp);
    // -- registers
    fread(&m->cgb_speed, 1, 1, fp);
    fread(&m->vram_bank, 1, 1, fp);
    fread(&m->wram_bank, 1, 1, fp);
    
    fread(&m->mbc_bank1, sizeof(uint16_t), 1, fp);
    fread(&m->mbc_bank2, sizeof(uint8_t), 1, fp);
    fread(&m->mbc1_mode, sizeof(uint8_t), 1, fp);
    fread(&m->mbc_ram_enable, sizeof(uint8_t), 1, fp);

    // -- memory
    fread(m->vram, sizeof(uint8_t), 0x4000, fp);
    fread(m->eram, sizeof(uint8_t), m->ram_size, fp);
    fread(m->wram, sizeof(uint8_t), 0x8000, fp);
    fread(&m->oam, sizeof(uint8_t), 0xA0, fp);
    fread(&m->io, sizeof(uint8_t), 0x80, fp);
    fread(&m->hram, sizeof(uint8_t), 0x7F, fp);
    fread(&m->ie, sizeof(uint8_t), 1, fp);

    fclose(fp);
    printf("[FILE] Loaded state from file: %s\n", file);
}