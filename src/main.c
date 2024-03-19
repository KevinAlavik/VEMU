#include <stdio.h>
#include <stdlib.h>
#include "visc.h"
#include "bus.h"
#include "rom.h"

#define ROM_SIZE 7

int main() {
    unsigned int rom[ROM_SIZE] = {
        (8 & 0xFF) | ((0 & 0x0F) << 8) | ((0 & 0x0F) << 12),
        0x0000001,
        (8 & 0xFF) | ((1 & 0x0F) << 8) | ((0 & 0x0F) << 12),
        0x0000001,
        (0 & 0xFF) | ((1 & 0x0F) << 8) | ((0 & 0x0F) << 12),
        0x00000000,
        0x00000009
    };

    FILE *file = tmpfile();
    if (file == NULL) {
        perror("[VEMU] Error creating temporary file");
        return 1;
    }

    for (int i = 0; i < ROM_SIZE; i++) {
        fwrite(&rom[i], sizeof(unsigned int), 1, file);
    }
    fseek(file, 0, SEEK_SET);

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