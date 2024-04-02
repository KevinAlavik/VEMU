#ifndef __STORAGE_H__
#define __STORAGE_H__

#include "../bus.h"
#include "../bridge.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define STORAGE_READ 0x01
#define STORAGE_WRITE 0x02

#define MAX_SECTORS 16

extern bool storageEnabled;

void storage_init(uint32_t base, FILE *img);

#endif // __storage_H__