#include "visc.h"
#include "memory.h"

int main() {
    uint32_t *mem = init_memory();
    VISC_I *cpu = init_visc(mem);

    // Simple 1+1 into B
    mem[0] = (8 & 0xFF) | ((0 & 0x0F) << 8) | ((0 & 0x0F) << 12);
    mem[1] = 0x0000001;
    mem[2] = (8 & 0xFF) | ((1 & 0x0F) << 8) | ((0 & 0x0F) << 12);
    mem[3] = 0x0000001;
    mem[4] = (0 & 0xFF) | ((1 & 0x0F) << 8) | ((0 & 0x0F) << 12);
    mem[5] = 0x00000000;
    mem[6] = 0x00000009;

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
