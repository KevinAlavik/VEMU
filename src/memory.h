#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MEMORY_SIZE 0xFA00

uint32_t *init_memory();
uint32_t read(uint32_t *memory, uint16_t address);

#endif // __MEMORY_H__
