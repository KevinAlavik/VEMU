#ifndef __ROM_H__
#define __ROM_H__

#include "bus.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void rom_init(uint16_t base, uint16_t size, FILE *rom);

#endif // __ROM_H__
