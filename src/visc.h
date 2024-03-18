#ifndef __VISC_H__
#define __VISC_H__

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "memory.h"

typedef struct
{
    // Registers
    uint32_t registers[7];

    uint16_t PC;
    uint32_t *memory;
} VISC_I;

VISC_I *init_visc(uint32_t *memory);
void run_visc(VISC_I *visc);

#endif // __VISC_H__