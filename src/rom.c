#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "tinyfiledialogs.h"

#include "debug.h"
#include "file.h"
#include "header.h"
#include "memory.h"

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

	m->rom_size = load_file(file,&m->rom,1); // load file to "file" and return rom size in bytes
	
    // read header info
    Header h = read_header(m);
    print_header(h);

    m->ram_size = get_ram_size(h.RAMSize); // get number of bytes in external ram

    // mbc hardware handling
    m->cartridge_type = h.CartridgeType;
    m->eram = (uint8_t*)malloc(m->ram_size);
    for (size_t i = 0; i < m->ram_size; i++) m->eram[i] = 0xFF;

    // --- GBC ---
    if (h.CGBMode == 0x80 || h.CGBMode == 0xC0) m->cgb_mode = 1;
    else m->cgb_mode = 0;

    // gbc palette selection
    uint8_t game_id = encode_title(h.ROMName);
    printf("Game ID: %x\n",game_id);

	return file;

}

int read_boot_ROM(uint8_t** m){
	uint32_t boot_rom_size = load_file("cgb_boot.bin",m,0); // default to cgb bios
	if (!boot_rom_size) boot_rom_size = load_file("dmg_boot.bin",m,0); // attempt to load dmg bios on fail

    uint8_t boot_rom_type = 0;
    if (boot_rom_size == 0x100) boot_rom_type = 1; // dmg boot rom
    if (boot_rom_size == 0x900) boot_rom_type = 2; // cgb boot rom

    return boot_rom_type;
}


void print_ROM(uint8_t* m) {
	uint16_t MemROMSize = 32 * 1024; // 32kb
	
	for (int i=0; i< MemROMSize; i++){
		printf("%02x ",m[i]);
	}
}