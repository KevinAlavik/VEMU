#ifndef __VISC_H__
#define __VISC_H__

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "bus.h"
#include "rom.h"

typedef struct
{
    uint32_t registers[8];
    uint16_t PC;
    
    int rom_size;
} VISC_I;

VISC_I *init_visc(int rom_size);
void run_visc(VISC_I *visc);

#endif // __VISC_H__
