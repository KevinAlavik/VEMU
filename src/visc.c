#include "visc.h"

struct Instruction
{
    uint8_t opcode : 8;
    uint8_t sr1 : 4;
    uint8_t sr2 : 4;
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
            break;
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
    switch_plane(cpu, cpu->planeNum);

    for (int i = 0; i > PLANE_SIZE; i++)
    {
        cpu->low_plane[i] = 0x00000000;
    }

    for (int i = 0; i > PLANE_SIZE; i++)
    {
        cpu->high_plane[i] = 0x00000000;
    }
    return cpu;
}

Instruction extract_instruction(uint32_t val_low, uint32_t val_high)
{
    Instruction instr = {0};

    instr.imm = val_low & 0xFFFFFFFF;
    instr.data = (val_high >> 24) & 0xFF;
    instr.dr = (val_high >> 20) & 0xF;
    instr.sr2 = (val_high >> 16) & 0xF;
    instr.sr1 = (val_high >> 12) & 0xF;
    instr.opcode = (val_high >> 4) & 0xF;
    instr.class = val_high & 0xF;

    printf("IMM: 0x%08X, INSTR: 0x%08X\n", val_low, val_high);
    printf("[VISC Debug] Class: %d, Opcode: %d, SR1: %d, SR2: %d, DR: %d, DATA: %d, IMM: 0x%08X\n",
           instr.class, instr.opcode, instr.sr1, instr.sr2, instr.dr, instr.data, instr.imm);

    return instr;
}

void run_visc(VISC_I *visc)
{
    bool shouldRun = true;

    while (shouldRun)
    {
        int curPlane = visc->planeNum;
        int addr;
        switch_plane(visc, 1);
        addr = visc->curPlane[PC];
        switch_plane(visc, curPlane);

        // Avoid going out of bounds
        if ((addr + 1) >= (ROM_START + ROM_END))
        {
            shouldRun = false;
            return;
        }

        uint32_t val_low = bus_read(addr);
        uint32_t val_high = bus_read(addr + 1);

        Instruction instr = extract_instruction(val_low, val_high);
        bool d, a, j, al, f = false;

        switch (instr.class)
        {
        case DATA_CLASS:
            printf("[VISC Debug] DATA_CLASS: true\n");
            d = true;
            break;
        case ALU_CLASS:
            printf("[VISC Debug] ALU_CLASS: true\n");
            a = true;
            break;
        case JUMP_CLASS:
            printf("[VISC Debug] JUMP_CLASS: true\n");
            j = true;
            break;
        case ALGORITHM_CLASS:
            printf("[VISC Debug] ALGORITHM_CLASS: true\n");
            al = true;
            break;
        case FLOAT_CLASS:
            printf("[VISC Debug] FLOAT_CLASS: true\n");
            f = true;
            break;
        default:
            printf("[VISC] Unknown opcode class \"%d\"\n", instr.class);
            break;
        }

        if (d)
        {
            uint32_t data;

            switch (instr.opcode)
            {
            case NOP:
                printf("[VISC Debug] NOP Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                break;
            case LD:
                data = (uint32_t)instr.imm;
                visc->curPlane[instr.dr] = bus_read(data);
                printf("[VISC Debug] LD Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                break;
            case ST:
                data = (uint32_t)instr.imm;
                bus_write(data, visc->curPlane[instr.sr1]);
                printf("[VISC Debug] ST Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                break;
            case LDI:
                data = (uint32_t)instr.imm;
                visc->curPlane[instr.dr] = data;
                printf("[VISC Debug] LDI Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                break;
            case LDP:
                data = visc->curPlane[instr.sr1];
                data = bus_read(data);
                visc->curPlane[instr.dr] = data;
                printf("[VISC Debug] LDP Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                break;
            case STP:
                data = visc->curPlane[instr.sr2];
                uint32_t val = bus_read(data);
                data = visc->curPlane[instr.sr1];
                bus_write(data, val);
                printf("[VISC Debug] STP Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                break;
            case SRP:
                data = (uint32_t)instr.imm;
                switch_plane(visc, (uint8_t)data);
                printf("[VISC Debug] SRP Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                break;
            case PUSH:
                data = visc->curPlane[instr.sr1];
                visc->high_plane[SP]++;
                break;
            case POP:
                break;
            default:
                printf("[VISC] Unknown DATA opcode \"%d\"!\n", instr.opcode);
                break;
            }
        }
        else if (a)
        {
            switch (instr.opcode)
            {
            case ADD:
                visc->curPlane[instr.dr] = visc->curPlane[instr.sr1] + visc->curPlane[instr.sr2];
                printf("[VISC Debug] ADD Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                break;
            case SUB:
                visc->curPlane[instr.dr] = visc->curPlane[instr.sr1] - visc->curPlane[instr.sr2];
                printf("[VISC Debug] SUB Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                break;
            case MUL:
                visc->curPlane[instr.dr] = visc->curPlane[instr.sr1] * visc->curPlane[instr.sr2];
                printf("[VISC Debug] MUL Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                break;
            case DIV:
                visc->curPlane[instr.dr] = visc->curPlane[instr.sr1] / visc->curPlane[instr.sr2];
                printf("[VISC Debug] DIV Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                break;
            case SHL:
                visc->curPlane[instr.dr] = visc->curPlane[instr.sr1] << visc->curPlane[instr.sr2];
                printf("[VISC Debug] SHL Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                break;
            case SHR:
                visc->curPlane[instr.dr] = visc->curPlane[instr.sr1] >> visc->curPlane[instr.sr2];
                printf("[VISC Debug] SHR Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                break;
            case AND:
                visc->curPlane[instr.dr] = visc->curPlane[instr.sr1] & visc->curPlane[instr.sr2];
                printf("[VISC Debug] AND Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                break;
            case OR:
                visc->curPlane[instr.dr] = visc->curPlane[instr.sr1] | visc->curPlane[instr.sr2];
                printf("[VISC Debug] OR Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                break;
            case XOR:
                visc->curPlane[instr.dr] = visc->curPlane[instr.sr1] ^ visc->curPlane[instr.sr2];
                printf("[VISC Debug] XOR Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                break;
            default:
                printf("[VISC] Unknown ALU opcode \"%d\"!\n", instr.opcode);
                break;
            }
        }

        else if (j)
        {
            printf("[VISC] The opcode class labeld \"JUMP_CLASS\" is unimpelemented!\n");
            shouldRun = false;
            return;
        }
        else if (al)
        {
            printf("[VISC] The opcode class labeld \"ALGORITHM_CLASS\" is unimpelemented!\n");
            shouldRun = false;
            return;
        }
        else if (f)
        {
            printf("[VISC] The opcode class labeld \"FLOAT_CLASS\" is unimpelemented!\n");
            shouldRun = false;
            return;
        }

        switch_plane(visc, 1);
        visc->curPlane[PC] += 2;
        switch_plane(visc, curPlane);
    }
}
