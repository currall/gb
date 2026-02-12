#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "tinyfiledialogs.h"

#include "debug.h"
#include "file.h"

char* read_rom(char* file, Memory* m){ 

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
	
	m->rom_size = load_file(file,&m->rom);

    Header h = read_header(m);
    print_header(h);

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