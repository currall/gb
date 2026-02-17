#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "tinyfiledialogs.h"

#include "debug.h"
#include "file.h"
#include "header.h"

typedef enum {
    ID_DR_MARIO      = 0x3C,
    ID_MARIO_LAND    = 0x46,
    ID_MARIO_LAND_2  = 0xC9,
    ID_TETRIS        = 0xDB
} GameID;

uint8_t encode_title(uint8_t* title) {
    uint8_t result = 0;
    for (int i=0; i<16; i++){
        result += title[i];
    }
    return result;
}

uint32_t get_ram_size(uint8_t ram_size){
    switch (ram_size) {
        case 1: return 2 * 1024;
        case 2: return 8 * 1024;
        case 3: return 32 * 1024;
        case 4: return 128 * 1024;
        case 5: return 64 * 1024;
        default: return 0;
    }
}

char* read_rom(char* file, Memory* m, Status* s){ 

    // file type filter for file dialog
	const char* filter[] = {"*.gb","*.gbc"};
    
	if (!file) { // if no arg, use tinyfiledialogs
        file = tinyfd_openFileDialog(
        "Open ROM",
        "",
        2,
        filter,
        "Game Boy ROM files",
        0);
    }	

    if (!file) {
        printf("[ROM] load failed!\n");
        uint16_t size = 32*1024; // 32kb
        m->rom = (uint8_t*)malloc(size);
        for (size_t i = 0; i < size; i++) m->rom[i] = 0xFF;
    }

	m->rom_size = load_file(file,&m->rom); // load file to "file" and return rom size in bytes
	
    // read header info
    Header h = read_header(m);
    print_header(h);

    m->ram_size = get_ram_size(h.RAMSize); // get number of bytes in external ram
    printf("ram size (b): %d\n",m->ram_size);

    // mbc hardware handling
    m->cartridge_type = h.CartridgeType;
    printf("mbc type %d\n",m->cartridge_type);
    m->eram = (uint8_t*)malloc(m->ram_size);
    for (size_t i = 0; i < m->ram_size; i++) m->eram[i] = 0xFF;

    // gbc palette selection
    uint8_t game_id = encode_title(h.ROMName);

    switch(game_id) {
        case ID_DR_MARIO:       s->palette_no = 7; break; 
        case ID_MARIO_LAND:     s->palette_no = 5; break; 
        case ID_MARIO_LAND_2:   s->palette_no = 6; break; 
        case ID_TETRIS:         s->palette_no = 4; break; 
        default:                s->palette_no = 0; break;
    }

	return file;

}

void read_boot_ROM(char* file, uint8_t** m){
	load_file(file,m);
}


void print_ROM(uint8_t* m) {
	uint16_t MemROMSize = 32 * 1024; // 32kb
	
	for (int i=0; i< MemROMSize; i++){
		printf("%02x ",m[i]);
	}
}