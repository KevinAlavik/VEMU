#include "rom.h"

uint32_t *rom;
uint32_t rom_base;

uint32_t rom_read(uint32_t addr)
{
    printf("[VISC Debug] 0x%08X -> 0x%08X\n", addr, rom[(addr - rom_base) / 4]);
    return rom[(addr - rom_base) / 4];
}

void rom_write(uint32_t addr, uint32_t data)
{
    // Do nothing, ROM is read-only
}

void rom_init(uint32_t base, uint32_t size, FILE *file)
{
    rom = (uint32_t *)malloc(size);
    rom_base = base;

    uint32_t value;
    size_t index = 0;
    while (fread(&value, sizeof(uint32_t), 1, file) == 1 && index < size / sizeof(uint32_t))
    {
        rom[index++] = value;
    }

    add_device(base, base + size - 1, rom_read, rom_write);
}
