#ifndef __BUS_H__
#define __BUS_H__

#include <stdbool.h>
#include <stdint.h>

typedef uint32_t (*BusReadFunc)(uint32_t);
typedef void (*BusWriteFunc)(uint32_t, uint32_t);

struct device
{
    uint32_t base;
    uint32_t limit;
    BusReadFunc read;
    BusWriteFunc write;
};

bool add_device(uint32_t base, uint32_t limit, BusReadFunc read,
                BusWriteFunc write);
void bus_write(uint32_t addr, uint32_t data);
uint32_t bus_read(uint32_t addr);

#endif // __BUS_H__