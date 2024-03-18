#include "visc.h"
#include "memory.h"

int main()
{
    uint32_t *mem = init_memory();
    VISC_I *cpu = init_visc(mem);
    run_visc(cpu);
    return 0;
}