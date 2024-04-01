#include "visc.h"
VISC_I *temp;
bool shouldRun = true;
bool debug_log = false;

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
            if(debug_log)
                printf("[VISC] Switched to LPLANE!\n");
            
            break;
        case 1:
            cpu->curPlane = cpu->high_plane;
            if(debug_log)
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
char get_extension_letter(int i) {
    switch(i) {
        case BASIC_SHIT:
            return 'B';
        case MULTIPLY:
            return 'M';
        default:
            return '\0';
    }
}
char* get_extension_string(int i) {
    switch(i) {
        case BASIC_SHIT:
            return "BASIC_SHIT";
        case MULTIPLY:
            return "MULTIPLY";
        default:
            return "Unknown";
    }
}

void enable_extension(VISC_I* cpu, int i) {
    if(cpu != NULL) {
        cpu->extensions[i] = UINT8_MAX;
        if(debug_log)
            printf("[VISC] Enabled \"%s\" extension (%c)\n", get_extension_string(i), get_extension_letter(i));
    }
}

bool extension_enabled(VISC_I* cpu, int i) {
    if(cpu->extensions[i] == UINT8_MAX)
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

    for (int i = 0; i > PLANE_SIZE; i++)
    {
        cpu->low_plane[i] = 0x00000000;
    }

    for (int i = 0; i > PLANE_SIZE; i++)
    {
        cpu->high_plane[i] = 0x00000000;
    }
    
    cpu->high_plane[SP] = 0x00000100;

    enable_extension(cpu, BASIC_SHIT);
    enable_extension(cpu, MULTIPLY);
    return cpu;
}

char* get_reg_string(VISC_I* cpu, uint8_t n) {
    if (cpu->planeNum == 0) {
        switch (n) {
            case A: return "A";
            case B: return "B";
            case C: return "C";
            case D: return "D";
            case E: return "E";
            case F: return "F";
            case G: return "G";
            case H: return "H";
            case A1: return "A1";
            case A2: return "A2";
            case A3: return "A3";
            case A4: return "A4";
            case A5: return "A5";
            case A6: return "A6";
            case A7: return "A7";
            case A8: return "A8";
            default: return NULL;
        }
    } else {
        switch (n) {
            case A_SHADOW: return "A_SHADOW";
            case B_SHADOW: return "B_SHADOW";
            case C_SHADOW: return "C_SHADOW";
            case D_SHADOW: return "D_SHADOW";
            case E_SHADOW: return "E_SHADOW";
            case F_SHADOW: return "F_SHADOW";
            case G_SHADOW: return "G_SHADOW";
            case H_SHADOW: return "H_SHADOW";
            case PC: return "PC";
            case SP: return "SP";
            case BP: return "BP";
            case RESERVED_1: return "RESERVED_1";
            case RESERVED_2: return "RESERVED_2";
            case STATUS: return "STATUS";
            case HARTID: return "HARTID";
            case FLAGS: return "FLAGS";
            default: return NULL;
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

    if(debug_log) {
        char* drs = get_reg_string(temp, instr.dr);
        char* sr1s = get_reg_string(temp, instr.sr1);
        char* sr2s = get_reg_string(temp, instr.sr2);
        printf("IMM: 0x%08X, INSTR: 0x%08X\n", val_low, val_high);
        printf("[VISC Debug] Class: %d, Opcode: %d, SR1: %s, SR2: %s, DR: %s, DATA: %d, IMM: 0x%08X\n",
                instr.class, instr.opcode, sr1s, sr2s, drs, instr.data, instr.imm);
    }
    

    return instr;
}

// Emulate a delay in Mhz
void delay_mhz(unsigned int frequency_mhz) {
    unsigned int delay_microseconds = 1000000 / (frequency_mhz*10);
    usleep(delay_microseconds);
}

void run_visc(VISC_I *visc, int clock_speed)
{
    shouldRun = true;
    temp = visc;

    while (shouldRun)
    {
        int addr;
        uint32_t data;
        
        addr = visc->high_plane[PC];

        // Avoid going out of bounds
        if ((addr + 1) >= (ROM_START + ROM_END))
        {
            if(debug_log)
                printf("[VISC] Reached end of ROM \"0x%08X\"\n", (ROM_START + ROM_END));
            
            shouldRun = false;
            return;
        }

        uint32_t val_low = bus_read(addr);
        uint32_t val_high = bus_read(addr + 1);

        Instruction instr = extract_instruction(val_low, val_high);
        bool d = false;
        bool a = false;
        bool j = false;
        bool al = false;
        bool f = false;

        
        switch (instr.class)
        {
        case DATA_CLASS:
            if(debug_log)
                printf("[VISC Debug] DATA_CLASS: true\n");
            
            d = true;
            break;
        case ALU_CLASS:
            if(debug_log)
                printf("[VISC Debug] ALU_CLASS: true\n");
            
            a = true;
            break;
        case JUMP_CLASS:
            if(debug_log)
                printf("[VISC Debug] JUMP_CLASS: true\n");
            
            j = true;
            break;
        case ALGORITHM_CLASS:
            if(debug_log)
                printf("[VISC Debug] ALGORITHM_CLASS: true\n");
            
            al = true;
            break;
        case FLOAT_CLASS:
            if(debug_log)
                printf("[VISC Debug] FLOAT_CLASS: true\n");
            
            f = true;
            break;
        default:
            printf("[VISC] Unknown opcode class \"%d\"\n", instr.class);
            break;
        }

        if (d && extension_enabled(visc, BASIC_SHIT))
        {
            switch (instr.opcode)
            {
            case NOP:
                if(debug_log)
                    printf("[VISC Debug] NOP Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                
                break;
            case LD:
                data = (uint32_t)instr.imm;
                visc->curPlane[instr.dr] = bus_read(data);
                if(debug_log)
                    printf("[VISC Debug] LD Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                
                break;
            case ST:
                data = (uint32_t)instr.imm;
                bus_write(data, visc->curPlane[instr.sr1]);
                if(debug_log)
                    printf("[VISC Debug] ST Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                
                break;
            case LDI:
                data = (uint32_t)instr.imm;
                visc->curPlane[instr.dr] = data;
                if(debug_log)
                    printf("[VISC Debug] LDI Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                
                break;
            case LDP:
                data = visc->curPlane[instr.sr1];
                data = bus_read(data);
                visc->curPlane[instr.dr] = data;
                if(debug_log)
                    printf("[VISC Debug] LDP Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                
                break;
            case STP:
                data = visc->curPlane[instr.sr2];
                uint32_t val = bus_read(data);
                data = visc->curPlane[instr.sr1];
                bus_write(data, val);
                if(debug_log)
                    printf("[VISC Debug] STP Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                
                break;
            case SRP:
                data = (uint32_t)instr.imm;
                switch_plane(visc, (uint8_t)data);
                if(debug_log)
                    printf("[VISC Debug] SRP Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                
                break;
            case PUSH:
                data = visc->curPlane[instr.sr1];
                bus_write(visc->high_plane[SP], data);
                visc->high_plane[SP]++;
                if(debug_log)
                    printf("[VISC Debug] PUSH Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                
                break;
            case POP:
                visc->high_plane[SP]--;
                visc->curPlane[instr.sr1] = bus_read(visc->high_plane[SP]);
                bus_write(visc->high_plane[SP], 0);
                if(debug_log)
                    printf("[VISC Debug] POP Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                
                break;
            default:
                printf("[VISC] Unknown DATA opcode \"%d\"!\n", instr.opcode);
                break;
            }
        }
        else if (a && extension_enabled(visc, BASIC_SHIT))
        {
            switch (instr.opcode)
            {
            case ADD:
                visc->curPlane[instr.dr] = visc->curPlane[instr.sr1] + visc->curPlane[instr.sr2];
                if(debug_log)
                    printf("[VISC Debug] ADD Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                
                break;
            case SUB:
                visc->curPlane[instr.dr] = visc->curPlane[instr.sr1] - visc->curPlane[instr.sr2];
                if(debug_log)
                    printf("[VISC Debug] SUB Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                
                break;
            case MUL:
                if(extension_enabled(visc, MULTIPLY)) {
                    visc->curPlane[instr.dr] = visc->curPlane[instr.sr1] * visc->curPlane[instr.sr2];
                    if(debug_log)
                        printf("[VISC Debug] MUL Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                } else {
                    printf("[VISC] A MUL instruction tried to be executed at 0x%08X. Need to enable MULTIPLY!\n", visc->high_plane[PC]);
                }
                break;
            case DIV:
                if(extension_enabled(visc, MULTIPLY)) {
                    visc->curPlane[instr.dr] = visc->curPlane[instr.sr1] / visc->curPlane[instr.sr2];
                    if(debug_log)
                        printf("[VISC Debug] DIV Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                } else {
                    printf("[VISC] A DIV instruction tried to be executed at 0x%08X. Need to enable MULTIPLY!\n", visc->high_plane[PC]);
                }
                break;
            case SHL:
                visc->curPlane[instr.dr] = visc->curPlane[instr.sr1] << visc->curPlane[instr.sr2];
                if(debug_log)
                    printf("[VISC Debug] SHL Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                
                break;
            case SHR:
                visc->curPlane[instr.dr] = visc->curPlane[instr.sr1] >> visc->curPlane[instr.sr2];
                if(debug_log)
                    printf("[VISC Debug] SHR Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                
                break;
            case AND:
                visc->curPlane[instr.dr] = visc->curPlane[instr.sr1] & visc->curPlane[instr.sr2];
                if(debug_log)
                    printf("[VISC Debug] AND Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                
                break;
            case OR:
                visc->curPlane[instr.dr] = visc->curPlane[instr.sr1] | visc->curPlane[instr.sr2];
                if(debug_log)
                    printf("[VISC Debug] OR Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                
                break;
            case XOR:
                visc->curPlane[instr.dr] = visc->curPlane[instr.sr1] ^ visc->curPlane[instr.sr2];
                if(debug_log)
                    printf("[VISC Debug] XOR Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                
                break;
            default:
                printf("[VISC] Unknown ALU opcode \"%d\"!\n", instr.opcode);
                break;
            }
        }

        else if (j)
        { 
            switch (instr.opcode)
            {
            case JMP:
                visc->high_plane[PC] = instr.imm;
                if(debug_log)
                    printf("[VISC Debug] JMP Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                
                break;
            case CALL:
                data = visc->high_plane[PC];
                bus_write(visc->high_plane[SP], data);
                visc->high_plane[SP]++;
                visc->high_plane[PC] = instr.imm;
                if(debug_log)
                    printf("[VISC Debug] CALL Instruction executed at 0x%08X\n", visc->high_plane[PC]);
                
                break;
            default:
                printf("[VISC] Unknown JUMP opcode \"%d\"!\n", instr.opcode);
                break;
            };
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

        visc->high_plane[PC] += 2;
        delay_mhz(clock_speed);
    }
}
