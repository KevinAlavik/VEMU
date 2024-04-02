#ifndef __UART_H__
#define __UART_H__

#include "../bus.h"
#include "../bridge.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define UART_WRITE 0x01
#define UART_CLEAR 0x02

void uart_init(uint32_t base);
void uart_print(const char *fmt, ...);

#endif // __UART_H__