#include "visc.h"
#include "memory.h"

int main() {
    uint32_t *mem = init_memory();
    mem[0] = 0x08000001;
    mem[1] = 0x00000001;
    mem[2] = 0x08010001;
    mem[3] = 0x00000001;
    mem[4] = 0x00100100;
    mem[5] = 0x90000000;
    VISC_I *cpu = init_visc(mem);
    run_visc(cpu);
    printf("A: %d\n", cpu->registers[0]);
    printf("B: %d\n", cpu->registers[1]);
    printf("C: %d\n", cpu->registers[2]);
    printf("D: %d\n", cpu->registers[3]);
    printf("E: %d\n", cpu->registers[4]);
    printf("F: %d\n", cpu->registers[5]);
    printf("G: %d\n", cpu->registers[6]);
    printf("H: %d\n", cpu->registers[7]);
    return 0;
}
