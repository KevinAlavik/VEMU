#include "visc.h"

#define ADD 0
#define SUB 1
#define SL 3
#define SR 4
#define AND 5
#define OR 6
#define XOR 7

struct Instruction
{
    uint8_t opcode;
    uint8_t reg1;
    uint8_t reg2;
    uint32_t reserved;
};

VISC_I *init_visc(uint32_t *memory)
{
    VISC_I *cpu = malloc(sizeof(VISC_I));
    if (cpu == NULL)
    {
        printf("[VISC] Failed to allocate memory for VISC_I structure.\n");
        return NULL;
    }

    if (memory != NULL)
    {
        cpu->memory = memory;
    }
    else
    {
        printf("[VISC] A NULL memory pointer got passed at %s\n", __FILE__);
        free(cpu);
        return NULL;
    }

    return cpu;
}

void run_visc(VISC_I *visc)
{
    bool shouldRun = true;

    while (shouldRun)
    {
        int addr = visc->PC;
        uint32_t val = read(visc->memory, addr);
        struct Instruction curInst;

        curInst.opcode = (val >> 24) & 0xFF;
        curInst.reg1 = (val >> 20) & 0x0F;
        curInst.reg2 = (val >> 16) & 0x0F;
        curInst.reserved = val & 0xFFFFFFFF;

        switch (curInst.opcode)
        {
        case ADD:
            int oreg = curInst.reg1;
            int ireg = curInst.reg2;

                        break;
        default:
            printf("[VISC] Unknown opcode \"%d\". Halting the CPU.\n", curInst.opcode);
            shouldRun = false;
            break;
        }
        visc->PC += 2;
    }
}
