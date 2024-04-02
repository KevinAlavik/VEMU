#include "rom.h"
#include "visc.h"

uint32_t *rom;
uint32_t rom_base;

uint32_t rom_read(uint32_t addr)
{
    if (addr >= rom_size)
    {
        printf("[VISC] Trying to go out of bounds in ROM!\n");
        return 0;
    }
#ifdef DEBUG
    printf("[VISC Debug] 0x%08X -> 0x%08X\n", addr, rom[addr - rom_base]);
#endif
    return rom[addr - rom_base];
}

void rom_write(uint32_t addr, uint32_t data)
{
    // Do nothing, ROM is read-only
}

void rom_init(uint32_t base, uint32_t size, FILE *file)
{
    rom = (uint32_t *)malloc(size * 4);
    rom_base = base;
    fread(rom, size, 4, file);

    add_device(base, size, rom_read, rom_write);
}
