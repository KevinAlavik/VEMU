#ifndef __ROM_H__
#define __ROM_H__

#include "bus.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void ram_init(uint32_t base, uint32_t size, FILE *rom);

#endif // __ROM_H__