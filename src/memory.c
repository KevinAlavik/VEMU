#include "memory.h"

uint32_t *init_memory()
{
    uint32_t *memory = malloc(MEMORY_SIZE * sizeof(uint32_t));
    if (memory == NULL)
    {
        printf("[VISC] Failed to allocate memory!\n");
        return NULL;
    }
    else
    {
        memset(memory, 0x00000000, MEMORY_SIZE * sizeof(uint32_t));
        return memory;
    }
}

uint32_t read(uint32_t *memory, uint16_t address)
{
    if (address >= MEMORY_SIZE)
    {
        printf("[VISC] Tried to read from an invalid address!\n");
        return -1;
    }
    else if (memory == NULL)
    {
        printf("[VISC] A NULL memory pointer got passed at %s\n", __FILE__);
        return -1;
    }
    else
    {
        return memory[address];
    }
}
