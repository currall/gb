#include <stdio.h>
#include <stdlib.h>

#include "file.h"
#include "header.h"
#include "memory.h"

// read ROM header from ROM file
Header read_header(Memory* m){
    Header h;
    
    for (int i = 0; i < 16; i++) { // 16 byte title
        h.ROMName[i] = m->rom[0x0134 + i];
    }

    // New licensee (big endian)
    h.NewLicensee = ((uint16_t)m->rom[0x0144] << 8) |
                      (uint16_t)m->rom[0x0145];

    // Single-byte fields
    h.SGBFeatures   = m->rom[0x0146];
    h.CartridgeType = m->rom[0x0147];
    h.ROMSize       = m->rom[0x0148];
    h.RAMSize       = m->rom[0x0149];
    h.CountryCode   = m->rom[0x014A];
    h.Licensee      = m->rom[0x014B];

    return h;
}

void print_header(Header h){
    printf("ROM Name:               %s\n",h.ROMName);
    printf("New Licensee:           %d\n",h.NewLicensee);
    printf("SuperGameBoy Features:  %d\n",h.SGBFeatures);
    printf("Cartridge Type:         %d\n",h.CartridgeType);
    printf("ROM Size:               %d\n",h.ROMSize);
    printf("RAM Size:               %d\n",h.RAMSize);
    printf("Country Code:           %d\n",h.CountryCode);
    printf("Licensee:               %d\n",h.Licensee);
}