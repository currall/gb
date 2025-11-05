#include <stdio.h>
#include <stdlib.h>

#include "file.h"
#include "header.h"
#include "memory.h"

// implement banking for larger roms here

void readROM(char* file, Header h, uint8_t* m){
    uint16_t ROMSizeB = 32 * 1024 * (1 << h.ROMSize); // rom size in bytes
	uint16_t MemROMSize = 32 * 1024; // 32kb
	uint16_t BootROMSize = 256; // 256 bytes
	
    readData(file, m, 0, MemROMSize); // read ROM data into RAM
	readData("dmg_boot.bin", m, 0, BootROMSize); // read Boot ROM data into RAM
}

void printROM(uint8_t* m) {
	uint16_t MemROMSize = 32 * 1024; // 32kb
	
	for (int i=0; i< MemROMSize; i++){
		printf("%x ",m[i]);
	}
}