#include <stdio.h>
#include <stdlib.h>
#include "visc.h"
#include "bus.h"
#include "rom.h"

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

    VISC_I *cpu = init_visc();

    rom_init(ROM_START, ROM_END, file);
    run_visc(cpu);

    char *p;
    switch (cpu->planeNum)
    {
    case 0:
        p = "LOW";
        break;
    case 1:
        p = "HIGH";
        break;
    default:
        p = "Invalid";
        break;
    }

    switch_plane(cpu, 0);
    printf("A: 0x%08X\n", cpu->curPlane[A]);
    printf("B: 0x%08X\n", cpu->curPlane[B]);
    printf("C: 0x%08X\n", cpu->curPlane[C]);
    printf("D: 0x%08X\n", cpu->curPlane[D]);
    printf("E: 0x%08X\n", cpu->curPlane[E]);
    printf("F: 0x%08X\n", cpu->curPlane[F]);
    printf("G: 0x%08X\n", cpu->curPlane[G]);
    printf("H: 0x%08X\n", cpu->curPlane[H]);
    switch_plane(cpu, 1);
    printf("PC: 0x%08X\n", cpu->curPlane[PC]);
    printf("SP: 0x%08X\n", cpu->curPlane[SP]);
    printf("BP: 0x%08X\n", cpu->curPlane[BP]);
    printf("FLAGS: 0x%08X\n", cpu->curPlane[FLAGS]);
    printf("PLANE: %s\n", p);

    fclose(file);

    return 0;
}