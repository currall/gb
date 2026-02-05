#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "file.h"

// implement banking for larger roms here

uint32_t read_rom(char* file, uint8_t** m){ // uses pointer to rom memorys pointer
    load_rom(file, m); 
}


void read_boot_ROM(char* file, uint8_t* m){
	read_file("dmg_boot.bin", m, 0, 256); // read Boot ROM data into RAM
}


void print_ROM(uint8_t* m) {
	uint16_t MemROMSize = 32 * 1024; // 32kb
	
	for (int i=0; i< MemROMSize; i++){
		printf("%02x ",m[i]);
	}
}