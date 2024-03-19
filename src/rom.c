#include "rom.h"
uint32_t *rom;
uint32_t rom_base;

uint32_t rom_read(uint32_t addr)
{
    printf("[VISC Debug] 0x%08X -> 0x%08X\n", addr, rom[(addr - rom_base) / 4]);
    return rom[(addr - rom_base) / 4];
}

void rom_write(uint32_t addr, uint32_t data) { return; }

void rom_init(uint32_t base, uint32_t size, FILE *file)
{
    rom = (uint32_t *)malloc(size);
    rom_base = base;
    fread(rom, size, 1, file);

    add_device(base, base + size - 1, rom_read, rom_write);
}