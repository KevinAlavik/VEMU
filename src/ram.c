#include "ram.h"
#include "visc.h"

uint32_t *ram;
uint32_t ram_base;
uint32_t ram_size;

uint32_t ram_read(uint32_t addr)
{
    if (addr < ram_base || addr >= ram_base + ram_size)
    {
        printf("[VISC] \x1B[31mERROR\x1B[0m Trying to access out of bounds in RAM!\n");
        return 0;
    }
    if (debug_log)
        printf("[VISC Debug] 0x%08X -> 0x%08X\n", addr, ram[addr - ram_base]);

    return ram[addr - ram_base];
}

void ram_write(uint32_t addr, uint32_t data)
{
    if (addr < ram_base || addr >= ram_base + ram_size)
    {
        printf("[VISC] \x1B[31mERROR\x1B[0m Trying to access out of bounds in RAM!\n");
        return;
    }

    ram[addr - ram_base] = data;
}

void ram_init(uint32_t base, uint32_t size)
{
    ram = (uint32_t *)malloc(size * sizeof(uint32_t));
    if (ram == NULL)
    {
        printf("[VISC] \x1B[31mERROR\x1B[0m Failed to allocate memory for RAM!\n");
        return;
    }

    ram_base = base;
    ram_size = size;

    add_device(base, size, ram_read, ram_write);
}
