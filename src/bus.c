#include "bus.h"
#include "bridge.h"
#include <stdio.h>

#define MAX_DEVICES 8

bool busEnable = true;

static struct device devices[MAX_DEVICES];
static uint32_t num_devices = 0;

void disable_buss()
{
    busEnable = false;
}

bool add_device(uint32_t base, uint32_t limit, BusReadFunc read,
                BusWriteFunc write)
{
    if (!busEnable)
        return false;

    if (num_devices >= MAX_DEVICES)
    {
        printf("[VISC] \x1B[31mERROR\x1B[0m Maximum number of devices reached\n");
        return false;
    }

    // Check for overlap with existing devices
    for (uint32_t i = 0; i < num_devices; i++)
    {
        if ((base >= devices[i].base && base <= devices[i].limit) ||
            (limit >= devices[i].base && limit <= devices[i].limit))
        {
            printf("[VISC] \x1B[31mERROR\x1B[0m Device address range overlaps with existing device\n");
            return false;
        }
    }

    devices[num_devices].base = base;
    devices[num_devices].limit = limit;
    devices[num_devices].read = read;
    devices[num_devices].write = write;

    if (debug_log)
        printf("[VISC Debug] Added device with base: 0x%08X, limit: 0x%08X to the bus\n", base, limit);

    num_devices++;
    return true;
}

void bus_write(uint32_t addr, uint32_t data)
{
    if (!busEnable)
        return;

    bool addressFound = false;
    for (uint32_t i = 0; i < num_devices; i++)
    {
        if (addr >= devices[i].base && addr <= devices[i].limit && devices[i].write)
        {
            devices[i].write(addr, data);
            addressFound = true;
            break;
        }
    }
    if (!addressFound)
        printf("[VISC] \x1B[31mERROR\x1B[0m Write address %u out of range or no write function provided\n", addr);
}

uint32_t bus_read(uint32_t addr)
{
    if (!busEnable)
        return 0;

    for (uint32_t i = 0; i < num_devices; i++)
    {
        if (addr >= devices[i].base && addr <= devices[i].limit && devices[i].read)
        {
            return devices[i].read(addr);
        }
    }
    printf("[VISC] \x1B[31mERROR\x1B[0m Read address %u out of range or no read function provided\n", addr);
    return 0;
}
