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
    for(int i = 0; i > PLANE_SIZE; i++) {
        cpu->curPlane[i] = 0x00000000;
    }
    
    cpu->high_plane[A_SHADOW] = 0x00000000;
    cpu->high_plane[B_SHADOW] = 0x00000000;
    cpu->high_plane[C_SHADOW] = 0x00000000;
    cpu->high_plane[D_SHADOW] = 0x00000000;
    cpu->high_plane[E_SHADOW] = 0x00000000;
    cpu->high_plane[F_SHADOW] = 0x00000000;
    cpu->high_plane[G_SHADOW] = 0x00000000;
    cpu->high_plane[H_SHADOW] = 0x00000000;
    cpu->high_plane[PC] = 0x00000000;
    return cpu;
}

Instruction extract_instruction(uint32_t val_low, uint32_t val_high) {
    Instruction instr;
    
    instr.class = val_low & 0xF;
    instr.opcode = (val_low >> 4) & 0xFF;
    instr.sr1 = (val_low >> 12) & 0xF;
    instr.sr2 = (val_low >> 16) & 0xF;
    instr.dr = (val_low >> 20) & 0xF;
    instr.data = ((uint64_t)(val_high) << 32) | val_low >> 24;

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
        
        // Avoid going into RAM
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
            uint32_t address;
            uint32_t value;

            switch(instr.opcode) 
            {
            case NOP:
                printf("[VISC Debug] NOP Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                break;
            case LD:
                address = (uint32_t)instr.data;
                visc->curPlane[instr.dr] = bus_read(address);
                printf("[VISC Debug] LD Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                break;
            case ST:
                address = (uint32_t)instr.data;
                bus_write(address, visc->curPlane[instr.sr1]);
                printf("[VISC Debug] ST Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                break;
            case LDI:
                value = (uint32_t)instr.data;
                visc->curPlane[instr.dr] = value;
                printf("[VISC Debug] LDI Instruction executed at 0x%08X\n", visc->high_plane[PC]);
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
        switch_plane(visc, 0);
    }
}
