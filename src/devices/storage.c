#include "storage.h"
#include "../visc.h"

uint8_t *storage;
uint32_t storage_base;

bool storageEnabled = false;

uint32_t storage_read(uint32_t addr)
{
    if (!storageEnabled)
        return -1;
    if (addr >= storage_base + (0x200 * MAX_SECTORS))
    {
        printf("[VISC] Trying to go out of bounds in storage!\n");
        return 0;
    }
    return (uint32_t)storage[addr - storage_base];
}

void storage_write(uint32_t addr, uint32_t data)
{
    if (!storageEnabled)
        return;
    if (addr >= storage_base + (0x200 * MAX_SECTORS))
    {
        printf("[VISC] Trying to go out of bounds in storage!\n");
        return;
    }
    uint32_t sector_num;
    uint32_t sector_count;
    uint32_t data_ptr;
    uint32_t d;
    switch (data)
    {
    case STORAGE_READ:
        sector_num = cpu->low_plane[A1];
        sector_count = cpu->low_plane[A2];
        break;
    case STORAGE_WRITE:
        sector_num = cpu->low_plane[A1];
        sector_count = cpu->low_plane[A2];
        data_ptr = cpu->low_plane[A3];
        d = bus_read(data_ptr);
        break;
    default:
        printf("[VISC Storage] \x1B[31mERROR\x1B[0m Unknown command \"%d\"", data);
        break;
    }
}

void storage_init(uint32_t base, FILE *img)
{
    storage = (uint8_t *)malloc((0x200 * MAX_SECTORS) * sizeof(uint8_t));
    storage_base = base;
    fread(storage, sizeof(uint8_t), (0x200 * MAX_SECTORS), img);
    add_device(base, (base + (0x200 * MAX_SECTORS)), storage_read, storage_write);
    storageEnabled = true;
}