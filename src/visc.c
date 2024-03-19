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

Instruction extract_instruction(uint32_t val_low, uint32_t val_high) {
    Instruction instr;
    
    instr.class = (val_low >> 28) & 0xF;
    instr.opcode = (val_low >> 24) & 0xFF;
    instr.sr1 = (val_low >> 20) & 0xF;
    instr.sr2 = (val_low >> 16) & 0xF;
    instr.dr = (val_low >> 12) & 0xF;
    instr.data = ((uint64_t)val_low & 0xFFF) << 32;

    instr.data |= (uint64_t)(val_high & 0xFFFFFFFF);

    return instr;
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

        if ((addr + 1) >= 0x0000FFFF)
        {
            shouldRun = false;
            return;
        }

        uint32_t val_low = bus_read(addr);
        uint32_t val_high = bus_read(addr + 1);
        printf("L: %d, H: %d\n", val_low, val_high);

        Instruction instr = extract_instruction(val_low, val_high);
        bool d, a, j, al, f = false;

        switch (instr.class)
        {
        case DATA_CLASS:
            d = true;
            break;
        case ALU_CLASS:
            a = true;
            break;
        case JUMP_CLASS:
            j = true;
            break;
        case ALGORITHM_CLASS:
            al = true;
            break;
        case FLOAT_CLASS:
            f = true;
            break;
        default:
            printf("[VISC] Unknown opcode class \"%d\"\n", instr.class);
            break;
        }

        if (d)
        {
            printf("[VISC] The opcode class labeld \"DATA_CLASS\" is unimpelemented!\n");
        }
        else if (a)
        {
            printf("[VISC] The opcode class labeld \"ALU_CLASS\" is unimpelemented!\n");
        }
        else if (j)
        {
            printf("[VISC] The opcode class labeld \"JUMP_CLASS\" is unimpelemented!\n");
        }
        else if (al)
        {
            printf("[VISC] The opcode class labeld \"ALGORITHM_CLASS\" is unimpelemented!\n");
        }
        else if (f)
        {
            printf("[VISC] The opcode class labeld \"FLOAT_CLASS\" is unimpelemented!\n");
        }

        switch_plane(visc, 1);
        visc->curPlane[PC] += 2;
        switch_plane(visc, 0);
    }
}
