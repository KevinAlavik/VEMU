#include "bridge.h"

#define CLOCK_SPEED 50

VISC_I *cpu;
bool runEmu = true; 

// Utility functions
double bytes_to_kb(unsigned long long bytes_value) {
    double kb_value = bytes_value / 1024.0;
    return kb_value;
}

double bytes_to_mb(unsigned long long bytes_value) {
    double mb_value = bytes_value / (1024.0 * 1024.0);
    return mb_value;
}

double bytes_to_gb(unsigned long long bytes_value) {
    double gb_value = bytes_value / (1024.0 * 1024.0 * 1024.0);
    return gb_value;
}


// Emulator entry
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "rb");
    if (file == NULL)
    {
        perror("Error opening ROM file");
        return 1;
    }

    cpu = init_visc();

    rom_init(ROM_START, ROM_END, file);
    ram_init(RAM_START, RAM_END);

    syscon_init(0x80000001, 0x80000002);
    uart_init(0x80000003, 0x80000004);

    uart_print("-----------------------------------------\n");
    uart_print("               VISC-I ISA                \n");
    uart_print("    Emulator written by Kevin Alavik     \n");
    uart_print("  Credits to lolguy91 for creating VISC  \n");
    uart_print("                                         \n");
    uart_print("  RAM: %.0f\tKB                          \n", bytes_to_kb(RAM_END - RAM_START));
    uart_print("       %.0f\tMB                          \n", bytes_to_mb(RAM_END - RAM_START));
    uart_print("       %.0f\tGB                          \n", bytes_to_gb(RAM_END - RAM_START));
    uart_print("                                         \n");
    uart_print("  ROM: %dB                               \n", ROM_END - ROM_START);
    uart_print("                                         \n");
    uart_print("-----------------------------------------\n");

    run_visc(cpu, CLOCK_SPEED);

    cpu->low_plane[A1] = LPLANE;
    bus_write(0x80000001, SYSCON_DUMP);
    cpu->low_plane[A1] = HPLANE;
    bus_write(0x80000001, SYSCON_DUMP);

    while(runEmu) {
        bus_write(0x80000001, SYSCON_SHUTDOWN);
    }

    fclose(file);

    return 0;
}