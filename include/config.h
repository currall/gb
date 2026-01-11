// config.h
#ifndef CONFIG_H
#define CONFIG_H

#define TRUE 1
#define FALSE 0

#define PRINT_ROM_HEADER 	TRUE
#define PRINT_MEMORY 		FALSE
#define PRINT_CYCLE 		FALSE

#define SHOW_INSTRUCTION	TRUE
#define SHOW_OPERANDS 		TRUE
#define SHOW_CPU		 	TRUE
#define SHOW_REGISTERS	 	FALSE
#define SHOW_IME			TRUE
#define SHOW_CYCLES		 	TRUE

#define CPU_HZ 		4194304
#define FPS			60
#define FRAME_TIME 	(1.0 / FPS)

#endif