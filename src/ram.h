#ifndef __RAM_H__
#define __RAM_H__

#include "bus.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void ram_init(uint32_t base, uint32_t size);

#endif // __RAM_H__