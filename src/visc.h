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
    uint32_t A;
    uint32_t B;
    uint32_t C;
    uint32_t D;
    uint32_t E;
    uint32_t F;
    uint32_t G;
    uint32_t H;

    uint16_t PC;
    uint32_t *memory;
} VISC_I;

VISC_I *init_visc(uint32_t *memory);
void run_visc(VISC_I *visc);

#endif // __VISC_H__