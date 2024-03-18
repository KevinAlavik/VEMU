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
#define LDI 8
#define HLT 9

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

    cpu->PC = 0x00000000; // Make sure to start at addres 0x00000000

    return cpu;
}

const char* get_register_label(int reg_num)
{
    switch (reg_num)
    {
        case A:
            return "A";
        case B:
            return "B";
        case C:
            return "C";
        case D:
            return "D";
        case E:
            return "E";
        case F:
            return "F";
        case G:
            return "G";
        case H:
            return "H";
        default:
            return "Invalid Register Number";
    }
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
            return;
        }

        uint32_t val = read(visc->memory, addr);
        struct Instruction curInst;
        curInst.opcode = val & 0xFF;
        curInst.reg1 = (val >> 8) & 0x0F;
        curInst.reg2 = (val >> 12) & 0x0F;
        curInst.reserved = (val >> 16) & 0xFFFF;
        const char* reg1_label = get_register_label(curInst.reg1);
        const char* reg2_label = get_register_label(curInst.reg2);

        switch (curInst.opcode)
        {
            case ADD:
                visc->registers[curInst.reg1] += visc->registers[curInst.reg2];
                printf("[VISC Debug] Performed ADD operation at 0x%04X (%s -> %s)\n", visc->PC, reg2_label, reg1_label);
                break;
            case SUB:
                visc->registers[curInst.reg1] -= visc->registers[curInst.reg2];
                printf("[VISC Debug] Performed SUB operation at 0x%04X (%s -> %s)\n", visc->PC, reg2_label, reg1_label);
                break;
            case SL:
                visc->registers[curInst.reg1] <<= visc->registers[curInst.reg2];
                printf("[VISC Debug] Performed SL operation at 0x%04X (%s -> %s)\n", visc->PC, reg2_label, reg1_label);
                break;
            case SR:
                visc->registers[curInst.reg1] >>= visc->registers[curInst.reg2];
                printf("[VISC Debug] Performed SR operation at 0x%04X (%s -> %s)\n", visc->PC, reg2_label, reg1_label);
                break;
            case AND:
                visc->registers[curInst.reg1] &= visc->registers[curInst.reg2];
                printf("[VISC Debug] Performed AND operation at 0x%04X (%s -> %s)\n", visc->PC, reg2_label, reg1_label);
                break;
            case OR:
                visc->registers[curInst.reg1] |= visc->registers[curInst.reg2];
                printf("[VISC Debug] Performed OR operation at 0x%04X (%s -> %s)\n", visc->PC, reg2_label, reg1_label);
                break;
            case XOR:
                visc->registers[curInst.reg1] ^= visc->registers[curInst.reg2];
                printf("[VISC Debug] Performed XOR operation at 0x%04X (%s -> %s)\n", visc->PC, reg2_label, reg1_label);
                break;
            case LDI:
                visc->registers[curInst.reg1] = curInst.argument;
                printf("[VISC Debug] Performed LDI operation at 0x%04X (Loaded 0x%08X into %s)\n", visc->PC, curInst.argument, reg1_label);
                break;
            case HLT:
                printf("[VISC Debug] Preformed HLT operation at 0x%04X\n", addr);
                shouldRun = false;
                break;
            default:
                printf("[VISC] Unknown opcode \"%d\" (At 0x%04X). Halting the CPU.\n", curInst.opcode, visc->PC);
                shouldRun = false;
                break;
        }

        visc->PC += 2;
    }
}
