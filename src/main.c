#include <stdio.h>
#include <stdlib.h>
#include "visc.h"
#include "bus.h"
#include "rom.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    
    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
        perror("Error opening ROM file");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    VISC_I *cpu = init_visc(size);
    rom_init(0x0000, size, file);
    run_visc(cpu);
    
    printf("A: %d\n", cpu->registers[0]);
    printf("B: %d\n", cpu->registers[1]);
    printf("C: %d\n", cpu->registers[2]);
    printf("D: %d\n", cpu->registers[3]);
    printf("E: %d\n", cpu->registers[4]);
    printf("F: %d\n", cpu->registers[5]);
    printf("G: %d\n", cpu->registers[6]);
    printf("H: %d\n", cpu->registers[7]);
    printf("PC: %d\n", cpu->PC);
    
    fclose(file);
    
    return 0;
}