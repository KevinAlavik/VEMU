#include "storage.h"
#include "../visc.h"

uint8_t *storage;
uint32_t storage_base;

bool storageEnabled = false;

uint32_t storage_read(uint32_t addr)
{
    if (!storageEnabled)
        return -1;
    // if (addr >= storage_base + (0x200 * MAX_SECTORS))
    // {
    //     printf("[VISC] Trying to go out of bounds in storage!\n");
    //     return 0;
    // }
    // return (uint32_t)storage[addr - storage_base];
    return -1;
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
    uint32_t o;
    switch (data)
    {
    case STORAGE_READ:
        sector_num = cpu->low_plane[A1];
        sector_count = cpu->low_plane[A2];
        o = cpu->low_plane[A3];

        uint32_t start = STORAGE_START + (0x200 * sector_num);
        uint32_t end = start + (0x200 * sector_count);

        for (uint32_t addr1 = start; addr1 < end; addr1++)
        {
            bus_write(o, (uint32_t)storage[addr1 - storage_base]);
            o++;
        }
        break;
    case STORAGE_WRITE:
        sector_num = cpu->low_plane[A1];
        sector_count = cpu->low_plane[A2];
        data_ptr = cpu->low_plane[A3];
        d = bus_read(data_ptr);

        uint32_t start2 = STORAGE_START + (0x200 * sector_num);
        uint32_t end2 = start2 + (0x200 * sector_count);

        for (uint32_t addr2 = start2; addr2 < end2; addr2++)
        {
            storage[addr2] = d;
        }
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
    add_device(base, base, storage_read, storage_write);
    storageEnabled = true;
}