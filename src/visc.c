#include "visc.h"

// Registers
#define A 0
#define B 1
#define C 2
#define D 3
#define E 4
#define F 5
#define G 6
#define H 7

// Instructions
#define ADD 0
#define SUB 1
#define SL 3
#define SR 4
#define AND 5
#define OR 6
#define XOR 7

struct Instruction
{
    uint8_t opcode : 8;
    uint8_t reg1 : 4;
    uint8_t reg2 : 4;
    uint16_t reserved;
    uint32_t argument;
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
        if (addr >= MEMORY_SIZE)
        {
            printf("[VISC] Program counter exceeded memory size. Halting the CPU.\n");
            shouldRun = false;
            break;
        }

        uint32_t val = read(visc->memory, addr);
        struct Instruction curInst;

        curInst.opcode = (val >> 24) & 0xFF;
        curInst.reg1 = (val >> 20) & 0x0F;
        curInst.reg2 = (val >> 16) & 0x0F;
        curInst.reserved = (val >> 8) & 0xFFFF;
        curInst.argument = read(visc->memory, addr + 1);

        switch (curInst.opcode)
        {
        case ADD:
            visc->registers[curInst.reg1] += visc->registers[curInst.reg2];
            printf("[VISC Debug] Performed ADD operation at 0x%04X\n", visc->PC);
            break;
        default:
            printf("[VISC] Unknown opcode \"%d\" (At 0x%04X). Halting the CPU.\n", curInst.opcode, visc->PC);
            shouldRun = false;
            break;
        }

        visc->PC += 2;
    }
}
