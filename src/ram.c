#include "ram.h"
#include "visc.h"

uint32_t *ram;
uint32_t ram_base;

uint32_t ram_read(uint32_t addr)
{
    if (addr >= RAM_END)
    {
        printf("[VISC] Trying to go out of bounds in RAM!\n");
        return 0;
    }
    #ifdef DEBUG
        printf("[VISC Debug] 0x%08X -> 0x%08X\n", addr, ram[addr - ram_base]);
    #endif
    return ram[addr - ram_base];
}

void ram_write(uint32_t addr, uint32_t data)
{
   ram[addr - ram_base] = data;
}

void ram_init(uint32_t base, uint32_t size)
{
    ram = (uint32_t *)malloc(size * 4);
    ram_base = base;

    add_device(base, size, ram_read, ram_write);
}
