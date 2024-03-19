#include "visc.h"

struct Instruction
{
    uint8_t opcode : 8;
    uint8_t reg1 : 4;
    uint8_t reg2 : 4;
    uint16_t reserved;
    uint32_t argument;
};

void switch_plane(VISC_I *cpu, int num)
{
    if (cpu != NULL)
    {
        if (cpu->planeNum != num)
            cpu->planeNum = num;
        switch (cpu->planeNum)
        {
        case 0:
            cpu->curPlane = cpu->low_plane;
            break;
        case 1:
            cpu->curPlane = cpu->high_plane;
            break;
        default:
            printf("[VISC] Invalid plane number \"%d\"", cpu->planeNum);
        }
    }
    else
    {
        printf("[VISC] NULL pointer to cpu struct was passed at %s\n", __FILE__);
        return;
    }
}

VISC_I *init_visc()
{
    VISC_I *cpu = malloc(sizeof(VISC_I));
    if (cpu == NULL)
    {
        printf("[VISC] Failed to allocate memory for VISC_I structure.\n");
        return NULL;
    }

    cpu->planeNum = 0;

    // Make a better system to write to the second plane
    switch_plane(cpu, 1);
    cpu->curPlane[PC] = 0x00000000;
    switch_plane(cpu, 0);

    return cpu;
}

void run_visc(VISC_I *visc)
{
    bool shouldRun = true;

    while (shouldRun)
    {
        int addr;
        switch_plane(visc, 1);
        addr = visc->curPlane[PC];
        switch_plane(visc, 0);

        if (addr > 0x0000FFFF)
        {
            shouldRun = false;
            return;
        }

        uint32_t val = bus_read(addr);
        (void)val;

        // printf("[PC: 0x%08X: 0x%08X]\n", addr, val);

        switch_plane(visc, 1);
        visc->curPlane[PC] += 2;
        switch_plane(visc, 0);
    }
}
