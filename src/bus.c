#include "bus.h"
#include <stdio.h>

#define MAX_DEVICES 10

static struct device devices[MAX_DEVICES];
static int num_devices = 0;

bool add_device(uint16_t base, uint16_t limit, BusReadFunc read,
                BusWriteFunc write) {
    if (num_devices >= MAX_DEVICES) {
        printf("[VISC Debug] Maximum number of devices reached\n");
        return false;
    }
    devices[num_devices].base = base;
    devices[num_devices].limit = limit;
    devices[num_devices].read = read;
    devices[num_devices].write = write;
    printf("[VISC Debug] Added device with base: %u, limit: %u, read: %p, write: %p to the bus\n", base, limit, read, write);
    num_devices++;
    return true;
}

void bus_write(uint16_t addr, uint8_t data) {
    for (int i = 0; i < num_devices; i++) {
        if (addr >= devices[i].base && addr <= devices[i].limit && devices[i].write) {
            devices[i].write(addr, data);
            return;
        }
    }
    printf("[VISC Debug] Write address %u out of range or no write function provided\n", addr);
}

uint8_t bus_read(uint16_t addr) {
    for (int i = 0; i < num_devices; i++) {
        if (addr >= devices[i].base && addr <= devices[i].limit && devices[i].read) {
            return devices[i].read(addr);
        }
    }
    printf("[VISC Debug] Read address %u out of range or no read function provided\n", addr);
    return -1; 
}
