#include "visc.h"

VISC_I *temp;
bool shouldRun = true;
bool debug_log = false;
bool debug_step = false;
uint32_t rom_size = ROM_END;

struct Instruction
{
    uint8_t opcode : 8;
    uint8_t sr1 : 4;
    uint8_t sr2 : 4;
    uint16_t reserved;
    uint32_t argument;
};

void switch_plane(VISC_I *cpu, uint32_t num)
{
    if (cpu != NULL)
    {
        if (cpu->planeNum != num)
            cpu->planeNum = num;
        switch (cpu->planeNum)
        {
        case 0:
            cpu->curPlane = cpu->low_plane;
            if (debug_log)
                printf("[VISC] Switched to LPLANE!\n");

            break;
        case 1:
            cpu->curPlane = cpu->high_plane;
            if (debug_log)
                printf("[VISC] Switched to HPLANE!\n");

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

char get_extension_letter(uint32_t i)
{
    switch (i)
    {
    case BASIC_SHIT:
        return 'B';
    case MULTIPLY:
        return 'M';
    default:
        return '\0';
    }
}

uint32_t get_extension_id(char *s)
{
    if (strcmp(s, "BASIC_SHIT") == 0)
    {
        return BASIC_SHIT;
    }
    else if (strcmp(s, "MULTIPLY") == 0)
    {
        return MULTIPLY;
    }
    else
    {
        return -1;
    }
}

char *get_extension_string(uint32_t i)
{
    switch (i)
    {
    case BASIC_SHIT:
        return "BASIC_SHIT";
    case MULTIPLY:
        return "MULTIPLY";
    default:
        return "Unknown";
    }
}

void enable_extension(VISC_I *cpu, uint32_t i)
{
    if (cpu != NULL)
    {
        cpu->extensions[i] = UINT8_MAX;
        if (debug_log)
            printf("[VISC] Enabled \"%s\" extension (%c)\n", get_extension_string(i), get_extension_letter(i));
    }
}

void disable_extension(VISC_I *cpu, uint32_t i)
{
    if (cpu != NULL)
    {
        if (cpu->extensions[i] == UINT8_MAX)
            cpu->extensions[i] = 0;
        else
            return;
        if (debug_log)
            printf("[VISC] Disabled \"%s\" extension (%c)\n", get_extension_string(i), get_extension_letter(i));
    }
}

bool extension_enabled(VISC_I *cpu, uint32_t i)
{
    if (cpu->extensions[i] == UINT8_MAX)
        return true;
    else
        return false;
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

    for (uint32_t i = 0; i > PLANE_SIZE; i++)
    {
        cpu->low_plane[i] = 0x00000000;
    }

    for (uint32_t i = 0; i > PLANE_SIZE; i++)
    {
        cpu->high_plane[i] = 0x00000000;
    }

    cpu->high_plane[SP] = DEFAULT_STACK_START;
    cpu->high_plane[BP] = DEFAULT_STACK_END;

    enable_extension(cpu, BASIC_SHIT);
    disable_extension(cpu, MULTIPLY); // Disable as default
    return cpu;
}

char *get_reg_string(VISC_I *cpu, uint8_t n)
{
    if (cpu->planeNum == 0)
    {
        switch (n)
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
        case A1:
            return "A1";
        case A2:
            return "A2";
        case A3:
            return "A3";
        case A4:
            return "A4";
        case A5:
            return "A5";
        case A6:
            return "A6";
        case A7:
            return "A7";
        case A8:
            return "A8";
        default:
            return NULL;
        }
    }
    else
    {
        switch (n)
        {
        case A_SHADOW:
            return "A_SHADOW";
        case B_SHADOW:
            return "B_SHADOW";
        case C_SHADOW:
            return "C_SHADOW";
        case D_SHADOW:
            return "D_SHADOW";
        case E_SHADOW:
            return "E_SHADOW";
        case F_SHADOW:
            return "F_SHADOW";
        case G_SHADOW:
            return "G_SHADOW";
        case H_SHADOW:
            return "H_SHADOW";
        case PC:
            return "PC";
        case SP:
            return "SP";
        case BP:
            return "BP";
        case RESERVED_1:
            return "RESERVED_1";
        case RESERVED_2:
            return "RESERVED_2";
        case STATUS:
            return "STATUS";
        case HARTID:
            return "HARTID";
        case FLAGS:
            return "FLAGS";
        default:
            return NULL;
        }
    }
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

    if (debug_log)
    {
        char *drs = get_reg_string(temp, instr.dr);
        char *sr1s = get_reg_string(temp, instr.sr1);
        char *sr2s = get_reg_string(temp, instr.sr2);
        printf("IMM: 0x%08X, INSTR: 0x%08X\n", val_low, val_high);
        printf("[VISC Debug] Class: %d, Opcode: %d, SR1: %s, SR2: %s, DR: %s, DATA: %d, IMM: 0x%08X\n",
               instr.class, instr.opcode, sr1s, sr2s, drs, instr.data, instr.imm);
    }

    return instr;
}

// Emulate a delay in Mhz
void delay_mhz(uint32_t frequency_mhz)
{
    if (frequency_mhz <= 0)
        return;
    uint32_t delay_microseconds = 1000000 / (frequency_mhz * 10);
    usleep(delay_microseconds);
}

void run_visc(VISC_I *visc, uint32_t clock_speed)
{
    shouldRun = true;
    temp = visc;

    // Reset once again after the init to make sure all registers are ready
    visc->planeNum = 0;
    switch_plane(visc, visc->planeNum);

    for (uint32_t i = 0; i > PLANE_SIZE; i++)
    {
        visc->low_plane[i] = 0x00000000;
    }

    for (uint32_t i = 0; i > PLANE_SIZE; i++)
    {
        visc->high_plane[i] = 0x00000000;
    }

    visc->high_plane[SP] = DEFAULT_STACK_START;
    visc->high_plane[BP] = DEFAULT_STACK_END;

    uint32_t addr;
    uint32_t data;
    uint32_t val_low;
    uint32_t val_high;

    while (shouldRun)
    {
        addr = visc->high_plane[PC];
        printf("0x%08X\n", addr);

        // Avoid going out of bounds
        // TODO: Fix issue where if you go past a certain point (0x0003EA6) it segfaults.
        if ((addr + 1) >= (RAM_START + RAM_END))
        {
            if (debug_log)
                printf("[VISC] Reached end of RAM \"0x%08X\"\n", (ROM_START + RAM_END));

            shouldRun = false;
            return;
        }

        val_low = bus_read(addr);
        printf("L: 0x%08X\n", val_low);
        val_high = bus_read(addr + 1);
        printf("H: 0x%08X\n", val_high);
        // printf("0x%08X: 0x%08X%08X\n", addr, val_low, val_high);

        Instruction instr = extract_instruction(val_low, val_high);
        bool d = false;
        bool a = false;
        bool j = false;
        bool al = false;
        bool f = false;

        switch (instr.class)
        {
        case DATA_CLASS:
            if (debug_log)
                printf("[VISC Debug] DATA_CLASS: true\n");

            d = true;
            break;
        case ALU_CLASS:
            if (debug_log)
                printf("[VISC Debug] ALU_CLASS: true\n");

            a = true;
            break;
        case JUMP_CLASS:
            if (debug_log)
                printf("[VISC Debug] JUMP_CLASS: true\n");

            j = true;
            break;
        case ALGORITHM_CLASS:
            if (debug_log)
                printf("[VISC Debug] ALGORITHM_CLASS: true\n");

            al = true;
            break;
        case FLOAT_CLASS:
            if (debug_log)
                printf("[VISC Debug] FLOAT_CLASS: true\n");

            f = true;
            break;
        default:
            printf("[VISC] \x1B[31mERROR\x1B[0m Unknown opcode class \"%d\" at 0x%08X\n", instr.class, visc->high_plane[PC]);
            shouldRun = false;
            break;
        }

        if (d)
        {
            switch (instr.opcode)
            {
            case NOP:
                if (extension_enabled(visc, BASIC_SHIT))
                {
                    if (debug_log)
                        printf("[VISC Debug] NOP Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                }
                else
                {
                    printf("[VISC] \x1B[31mERROR\x1B[0m Illegal instruction at \"0x%08X\" (%d:%d)\n", visc->high_plane[PC], instr.class, instr.opcode);
                    shouldRun = false;
                }
                break;
            case LD:
                if (extension_enabled(visc, BASIC_SHIT))
                {
                    data = (uint32_t)instr.imm;
                    visc->curPlane[instr.dr] = bus_read(data);
                    if (debug_log)
                        printf("[VISC Debug] LD Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                }
                else
                {
                    printf("[VISC] \x1B[31mERROR\x1B[0m Illegal instruction at \"0x%08X\" (%d:%d)\n", visc->high_plane[PC], instr.class, instr.opcode);
                    shouldRun = false;
                }
                break;
            case ST:
                if (extension_enabled(visc, BASIC_SHIT))
                {
                    data = (uint32_t)instr.imm;
                    bus_write(data, visc->curPlane[instr.sr1]);
                    if (debug_log)
                        printf("[VISC Debug] ST Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                }
                else
                {
                    printf("[VISC] \x1B[31mERROR\x1B[0m Illegal instruction at \"0x%08X\" (%d:%d)\n", visc->high_plane[PC], instr.class, instr.opcode);
                    shouldRun = false;
                }
                break;
            case LDI:
                if (extension_enabled(visc, BASIC_SHIT))
                {
                    data = (uint32_t)instr.imm;
                    visc->curPlane[instr.dr] = data;
                    if (debug_log)
                        printf("[VISC Debug] LDI Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                }
                else
                {
                    printf("[VISC] \x1B[31mERROR\x1B[0m Illegal instruction at \"0x%08X\" (%d:%d)\n", visc->high_plane[PC], instr.class, instr.opcode);
                    shouldRun = false;
                }
                break;
            case LDP:
                if (extension_enabled(visc, BASIC_SHIT))
                {
                    data = visc->curPlane[instr.sr1];
                    data = bus_read(data);
                    visc->curPlane[instr.dr] = data;
                    if (debug_log)
                        printf("[VISC Debug] LDP Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                }
                else
                {
                    printf("[VISC] \x1B[31mERROR\x1B[0m Illegal instruction at \"0x%08X\" (%d:%d)\n", visc->high_plane[PC], instr.class, instr.opcode);
                    shouldRun = false;
                }
                break;
            case STP:
                if (extension_enabled(visc, BASIC_SHIT))
                {
                    data = visc->curPlane[instr.sr2];
                    uint32_t val = bus_read(data);
                    data = visc->curPlane[instr.sr1];
                    bus_write(data, val);
                    if (debug_log)
                        printf("[VISC Debug] STP Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                }
                else
                {
                    printf("[VISC] \x1B[31mERROR\x1B[0m Illegal instruction at \"0x%08X\" (%d:%d)\n", visc->high_plane[PC], instr.class, instr.opcode);
                    shouldRun = false;
                }
                break;
            case SRP:
                if (extension_enabled(visc, BASIC_SHIT))
                {
                    data = (uint32_t)instr.imm;
                    switch_plane(visc, (uint8_t)data);
                    if (debug_log)
                        printf("[VISC Debug] SRP Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                }
                else
                {
                    printf("[VISC] \x1B[31mERROR\x1B[0m Illegal instruction at \"0x%08X\" (%d:%d)\n", visc->high_plane[PC], instr.class, instr.opcode);
                    shouldRun = false;
                }
                break;
            case PUSH:
                if (extension_enabled(visc, BASIC_SHIT))
                {
                    if (visc->high_plane[SP] + 1 >= visc->high_plane[BP])
                    {
                        printf("[VISC] \x1B[31mERROR\x1B[0m Stack overflow at 0x%08X\n", visc->high_plane[PC]);
                        shouldRun = false;
                    }
                    data = visc->curPlane[instr.sr1];
                    bus_write(visc->high_plane[SP], data);
                    visc->high_plane[SP]++;
                    if (debug_log)
                        printf("[VISC Debug] PUSH Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                }
                else
                {
                    printf("[VISC] \x1B[31mERROR\x1B[0m Illegal instruction at \"0x%08X\" (%d:%d)\n", visc->high_plane[PC], instr.class, instr.opcode);
                    shouldRun = false;
                }
                break;
            case POP:
                if (extension_enabled(visc, BASIC_SHIT))
                {

                    visc->high_plane[SP]--;
                    visc->curPlane[instr.sr1] = bus_read(visc->high_plane[SP]);
                    bus_write(visc->high_plane[SP], 0);
                    if (debug_log)
                        printf("[VISC Debug] POP Instruction executed at 0x%08X\n", visc->high_plane[PC]);

                    break;
                }
                else
                {
                    printf("[VISC] \x1B[31mERROR\x1B[0m Illegal instruction at \"0x%08X\" (%d:%d)\n", visc->high_plane[PC], instr.class, instr.opcode);
                    shouldRun = false;
                }
            default:
                printf("[VISC] \x1B[31mERROR\x1B[0m Unknown DATA opcode \"%d\" at 0x%08X!\n", instr.opcode, visc->high_plane[PC]);
                break;
            }
        }
        else if (a)
        {
            switch (instr.opcode)
            {
            case ADD:
                if (extension_enabled(visc, BASIC_SHIT))
                {
                    visc->curPlane[instr.dr] = visc->curPlane[instr.sr1] + visc->curPlane[instr.sr2];
                    if (debug_log)
                        printf("[VISC Debug] ADD Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                }
                else
                {
                    printf("[VISC] \x1B[31mERROR\x1B[0m Illegal instruction at \"0x%08X\" (%d:%d)\n", visc->high_plane[PC], instr.class, instr.opcode);
                    shouldRun = false;
                }
                break;
            case SUB:
                if (extension_enabled(visc, BASIC_SHIT))
                {
                    visc->curPlane[instr.dr] = visc->curPlane[instr.sr1] - visc->curPlane[instr.sr2];
                    if (debug_log)
                        printf("[VISC Debug] SUB Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                }
                else
                {
                    printf("[VISC] \x1B[31mERROR\x1B[0m Illegal instruction at \"0x%08X\" (%d:%d)\n", visc->high_plane[PC], instr.class, instr.opcode);
                    shouldRun = false;
                }
                break;
            case MUL:
                if (extension_enabled(visc, MULTIPLY))
                {
                    visc->curPlane[instr.dr] = visc->curPlane[instr.sr1] * visc->curPlane[instr.sr2];
                    if (debug_log)
                        printf("[VISC Debug] MUL Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                }
                else
                {
                    printf("[VISC] \x1B[31mERROR\x1B[0m Illegal instruction at \"0x%08X\" (%d:%d)\n", visc->high_plane[PC], instr.class, instr.opcode);
                    shouldRun = false;
                }
                break;
            case DIV:
                if (extension_enabled(visc, MULTIPLY))
                {
                    visc->curPlane[instr.dr] = visc->curPlane[instr.sr1] / visc->curPlane[instr.sr2];
                    if (debug_log)
                        printf("[VISC Debug] DIV Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                }
                else
                {
                    printf("[VISC] \x1B[31mERROR\x1B[0m Illegal instruction at \"0x%08X\" (%d:%d)\n", visc->high_plane[PC], instr.class, instr.opcode);
                    shouldRun = false;
                }
                break;
            case SHL:
                if (extension_enabled(visc, BASIC_SHIT))
                {
                    visc->curPlane[instr.dr] = visc->curPlane[instr.sr1] << visc->curPlane[instr.sr2];
                    if (debug_log)
                        printf("[VISC Debug] SHL Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                }
                else
                {
                    printf("[VISC] \x1B[31mERROR\x1B[0m Illegal instruction at \"0x%08X\" (%d:%d)\n", visc->high_plane[PC], instr.class, instr.opcode);
                    shouldRun = false;
                }
                break;
            case SHR:
                if (extension_enabled(visc, BASIC_SHIT))
                {
                    visc->curPlane[instr.dr] = visc->curPlane[instr.sr1] >> visc->curPlane[instr.sr2];
                    if (debug_log)
                        printf("[VISC Debug] SHR Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                }
                else
                {
                    printf("[VISC] \x1B[31mERROR\x1B[0m Illegal instruction at \"0x%08X\" (%d:%d)\n", visc->high_plane[PC], instr.class, instr.opcode);
                    shouldRun = false;
                }
                break;
            case AND:
                if (extension_enabled(visc, BASIC_SHIT))
                {
                    visc->curPlane[instr.dr] = visc->curPlane[instr.sr1] & visc->curPlane[instr.sr2];
                    if (debug_log)
                        printf("[VISC Debug] AND Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                }
                else
                {
                    printf("[VISC] \x1B[31mERROR\x1B[0m Illegal instruction at \"0x%08X\" (%d:%d)\n", visc->high_plane[PC], instr.class, instr.opcode);
                    shouldRun = false;
                }
                break;
            case OR:
                if (extension_enabled(visc, BASIC_SHIT))
                {
                    visc->curPlane[instr.dr] = visc->curPlane[instr.sr1] | visc->curPlane[instr.sr2];
                    if (debug_log)
                        printf("[VISC Debug] OR Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                }
                else
                {
                    printf("[VISC] \x1B[31mERROR\x1B[0m Illegal instruction at \"0x%08X\" (%d:%d)\n", visc->high_plane[PC], instr.class, instr.opcode);
                    shouldRun = false;
                }
                break;
            case XOR:
                if (extension_enabled(visc, BASIC_SHIT))
                {
                    visc->curPlane[instr.dr] = visc->curPlane[instr.sr1] ^ visc->curPlane[instr.sr2];
                    if (debug_log)
                        printf("[VISC Debug] XOR Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                }
                else
                {
                    printf("[VISC] \x1B[31mERROR\x1B[0m Illegal instruction at \"0x%08X\" (%d:%d)\n", visc->high_plane[PC], instr.class, instr.opcode);
                    shouldRun = false;
                }
                break;
            default:
                printf("[VISC] \x1B[31mERROR\x1B[0m Unknown ALU opcode \"%d\" at 0x%08X!\n", instr.opcode, visc->high_plane[PC]);
                break;
            }
        }

        else if (j)
        {
            switch (instr.opcode)
            {
            case JMP:
                if (extension_enabled(visc, BASIC_SHIT))
                {
                    visc->high_plane[PC] = instr.imm;
                    if (debug_log)
                        printf("[VISC Debug] JMP Instruction executed at 0x%08X\n", visc->high_plane[PC]);

                    break;
                }
                else
                {
                    printf("[VISC] \x1B[31mERROR\x1B[0m Illegal instruction at \"0x%08X\" (%d:%d)\n", visc->high_plane[PC], instr.class, instr.opcode);
                    shouldRun = false;
                }
            case CALL:
                if (extension_enabled(visc, BASIC_SHIT))
                {
                    data = visc->high_plane[PC];
                    bus_write(visc->high_plane[SP], data);
                    visc->high_plane[SP]++;
                    visc->high_plane[PC] = instr.imm;
                    if (debug_log)
                        printf("[VISC Debug] CALL Instruction executed at 0x%08X\n", visc->high_plane[PC]);

                    break;
                }
                else
                {
                    printf("[VISC] \x1B[31mERROR\x1B[0m Illegal instruction at \"0x%08X\" (%d:%d)\n", visc->high_plane[PC], instr.class, instr.opcode);
                    shouldRun = false;
                }
            case RET:
                if (extension_enabled(visc, BASIC_SHIT))
                {
                    visc->high_plane[SP]--;
                    data = visc->high_plane[SP];
                    bus_write(visc->high_plane[SP], 0);
                    visc->high_plane[PC] = data;
                    if (debug_log)
                        printf("[VISC Debug] RET Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                    break;
                }
                else
                {
                    printf("[VISC] \x1B[31mERROR\x1B[0m Illegal instruction at \"0x%08X\" (%d:%d)\n", visc->high_plane[PC], instr.class, instr.opcode);
                    shouldRun = false;
                }
            default:
                printf("[VISC] \x1B[31mERROR\x1B[0m Unknown JUMP opcode \"%d\" at 0x%08X!\n", instr.opcode, visc->high_plane[PC]);
                break;
            };
            return;
        }
        else if (al)
        {
            printf("[VISC] \x1B[31mERROR\x1B[0m The opcode class labeled \"ALGORITHM_CLASS\" is unimplemented, at 0x%08X\n", visc->high_plane[PC]);
            shouldRun = false;
            return;
        }
        else if (f)
        {
            printf("[VISC] \x1B[31mERROR\x1B[0m The opcode class labeled \"FLOAT_CLASS\" is unimplemented, at 0x%08X\n", visc->high_plane[PC]);
            shouldRun = false;
            return;
        }

        visc->high_plane[PC] += 2;
        if (debug_step)
        {
            printf("A: 0x%08X B: 0x%08X C: 0x%08X D: 0x%08X E: 0x%08X F: 0x%08X G: 0x%08X H: 0x%08X A1: 0x%08X A2: 0x%08X A3: 0x%08X A4: 0x%08X A5: 0x%08X A6: 0x%08X A7: 0x%08X A8: 0x%08X",
                   visc->low_plane[A],
                   visc->low_plane[B],
                   visc->low_plane[C],
                   visc->low_plane[D],
                   visc->low_plane[E],
                   visc->low_plane[F],
                   visc->low_plane[G],
                   visc->low_plane[H],
                   visc->low_plane[A1],
                   visc->low_plane[A2],
                   visc->low_plane[A3],
                   visc->low_plane[A4],
                   visc->low_plane[A5],
                   visc->low_plane[A6],
                   visc->low_plane[A7],
                   visc->low_plane[A8]);
            char c;
            scanf("%c", &c);
        }
        else
        {
            delay_mhz(clock_speed);
        }
    }
}