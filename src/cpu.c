#include <stdio.h>
#include <stdlib.h>

#include "rom.h"

void cycle(uint8_t* m, int pos){

	uint8_t instruction = m[pos];
	printf("\n%x",instruction);	
	
	switch (instruction) {
	
		case (0x31):printf("\nok");
	
	}
	
	// 0x31
	void ld_sp_nn
	
	
}