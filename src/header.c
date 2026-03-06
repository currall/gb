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
    h.CGBMode       = m->rom[0x0143];
    h.SGBFeatures   = m->rom[0x0146];
    h.CartridgeType = m->rom[0x0147];
    h.ROMSize       = m->rom[0x0148];
    h.RAMSize       = m->rom[0x0149];
    h.CountryCode   = m->rom[0x014A];
    h.Licensee      = m->rom[0x014B];

    return h;
}

void print_header(Header h){
    printf("+-------------------------+------------------+\n");
    printf("| Property                | Value            |\n");
    printf("+-------------------------+------------------+\n");
    printf("| ROM Name:               | %-16s |\n",h.ROMName);
    printf("| New Licensee:           | %-16d |\n",h.NewLicensee);
    printf("| GameBoy Color Support:  | %-16x |\n",h.CGBMode);
    printf("| SuperGameBoy Features:  | %-16d |\n",h.SGBFeatures);
    printf("| Cartridge Type:         | %-16d |\n",h.CartridgeType);
    printf("| ROM Size:               | %-16d |\n",h.ROMSize);
    printf("| RAM Size:               | %-16d |\n",h.RAMSize);
    printf("| Country Code:           | %-16d |\n",h.CountryCode);
    printf("| Licensee:               | %-16d |\n",h.Licensee);    
    printf("+-------------------------+------------------+\n");
}