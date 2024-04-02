#include "uart.h"
#include "../visc.h"
#include <stdarg.h>

uint32_t uart_base;
bool uartEnabled = false;

uint32_t uart_read(uint32_t addr)
{
    // no read fuck you
    return 0;
}

void uart_print(const char *fmt, ...)
{
    if (!uartEnabled)
        return;

    va_list args;
    va_start(args, fmt);
    char output[512];
    vsprintf(output, fmt, args);
    va_end(args);

    const char *ptr = output;
    while (*ptr != '\0')
    {
        cpu->low_plane[A1] = *ptr;
        bus_write(UART_START, UART_WRITE);
        ptr++;
    }
}

void uart_write(uint32_t addr, uint32_t data)
{
    if (!uartEnabled)
        return;

    switch (data)
    {
    case UART_WRITE:
        printf("%c", (char)cpu->low_plane[A1]);
        break;
    case UART_CLEAR:
        printf("\033[2J");
        printf("\033[H");
        break;
    default:
        printf("[VISC UART] \x1B[31mERROR\x1B[0m Unknown command \"%d\"", data);
        break;
    }
}

void uart_init(uint32_t base)
{
    uart_base = base;
    add_device(base, base, uart_read, uart_write);
    uartEnabled = true;
}
