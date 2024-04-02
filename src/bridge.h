#ifndef __BRIDGE_H__
#define __BRIDGE_H__

#include <stdio.h>
#include <stdlib.h>

#include "visc.h"
#include "bus.h"
#include "rom.h"
#include "ram.h"

// Devices
#include "devices/uart.h"
#include "devices/syscon.h"
#include "devices/storage.h"

extern VISC_I *cpu;
extern bool runEmu;
extern bool dump;

#endif // __BRIDGE_H__