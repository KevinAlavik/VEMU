#include "bridge.h"

VISC_I *cpu;
bool runEmu = true; 

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

    run_visc(cpu);

    cpu->low_plane[G] = LPLANE;
    bus_write(0x80000001, SYSCON_DUMP);

    while(runEmu) {
        bus_write(0x80000001, SYSCON_SHUTDOWN);
    }

    fclose(file);

    return 0;
}