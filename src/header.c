#include <stdio.h>
#include <stdlib.h>

#include "file.h"
#include "header.h"

// read ROM header from ROM file
Header read_header(char* file){
    Header h;
    read_data(file, &h, 0x0134, sizeof(Header));
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