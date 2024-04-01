#ifndef __SYSCON_H__
#define __SYSCON_H__

#include "../bus.h"
#include "../bridge.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define SYSCON_SHUTDOWN  0x01
#define SYSCON_RESET     0x02
#define SYSCON_DUMP      0x03

void syscon_init(uint32_t base, uint32_t size);

#endif // __SYSCON_H__