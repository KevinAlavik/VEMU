#ifndef __BUS_H__
#define __BUS_H__

#include <stdbool.h>
#include <stdint.h>

typedef uint32_t (*BusReadFunc)(uint16_t);
typedef void (*BusWriteFunc)(uint16_t, uint32_t);

struct device {
    uint16_t base;
    uint16_t limit;
    BusReadFunc read;
    BusWriteFunc write;
};
bool add_device(uint16_t base, uint16_t limit, BusReadFunc read,
                BusWriteFunc write);
void bus_write(uint16_t addr, uint32_t data);
uint32_t bus_read(uint16_t addr);

#endif // __BUSS_H__