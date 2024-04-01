#include "uart.h"
#include "../visc.h"
#include <stdarg.h>

uint32_t uart_base;

uint32_t uart_read(uint32_t addr)
{
    // no read fuck you
    return 0; 
}

void uart_print(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char output[512];
    vsprintf(output, fmt, args);
    va_end(args);

    const char* ptr = output;
    while (*ptr != '\0') {
        cpu->low_plane[G] = *ptr;
        bus_write(0x80000003, 0x01);
        ptr++;
    }
}


void uart_write(uint32_t addr, uint32_t data)
{
    switch(data) {
        case UART_WRITE:
            printf("%c", (char)cpu->low_plane[G]);
            break;
        case UART_CLEAR:
            printf("\033[2J");
            printf("\033[H");
            break;
        default:
            printf("[VISC - UART] Unknown command \"%d\"", data);
            break;
    }
}

void uart_init(uint32_t base, uint32_t size)
{
    uart_base = base;
    add_device(base, size, uart_read, uart_write);
}
