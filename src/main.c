#include "visc.h"
#include "bus.h"
#include "rom.h"

int main() {
    VISC_I *cpu = init_visc();
    rom_init(0x0000000, 0xFFFFFFFF, NULL); // TODO: Make it read a file lol
    run_visc(cpu);
    
    // Register Dump
    printf("A: %d\n", cpu->registers[0]);
    printf("B: %d\n", cpu->registers[1]);
    printf("C: %d\n", cpu->registers[2]);
    printf("D: %d\n", cpu->registers[3]);
    printf("E: %d\n", cpu->registers[4]);
    printf("F: %d\n", cpu->registers[5]);
    printf("G: %d\n", cpu->registers[6]);
    printf("H: %d\n", cpu->registers[7]);
    printf("PC: %d\n", cpu->PC);
    return 0;
}
