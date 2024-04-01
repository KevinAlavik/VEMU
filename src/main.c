#include "bridge.h"
#include <string.h>

#define CLOCK_SPEED 10

VISC_I *cpu;
bool runEmu = true; 

// Utility functions
double bytes_to_kb(unsigned long long bytes_value) {
    double kb_value = bytes_value / 1024.0;
    return kb_value;
}

double bytes_to_mb(unsigned long long bytes_value) {
    double mb_value = bytes_value / (1024.0 * 1024.0);
    return mb_value;
}

double bytes_to_gb(unsigned long long bytes_value) {
    double gb_value = bytes_value / (1024.0 * 1024.0 * 1024.0);
    return gb_value;
}

// Emulator entry
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s [OPTION]... [FILE]\n", argv[0]);
        printf("Emulate a VISC-I ISA CPU.\n\n");
        printf("Options:\n");
        printf("  -h, --help     display this help and exit\n");
        printf("  -v, --version  output version information and exit\n");
        printf("  -d, --dump     dumps the register on shutdown\n");
        return 1;
    }

    char* filename = NULL;
    bool dump = false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            printf("VEMU (%s) v0.1.4\n", __DATE__);
            return 0;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printf("Usage: %s [OPTION]... [FILE]\n", argv[0]);
            printf("Emulate a VISC-I ISA CPU.\n\n");
            printf("Options:\n");
            printf("  -h, --help     display this help and exit\n");
            printf("  -v, --version  output version information and exit\n");
            printf("  -d, --dump     dumps the register on shutdown\n");
            return 0;
        } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--dump") == 0) {
            dump = true;
        } else {
            if (argv[i][0] == '-' || (argv[i][0] == '-' && argv[i][1] == '-')) {
                printf("Invalid option \"%s\"\n", argv[i]);
                return EXIT_FAILURE;
            } else {
                filename = argv[i];
            }
        }
    }

    if (filename == NULL) {
        printf("No input file specified\n");
        return EXIT_FAILURE;
    }

    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening ROM file");
        return EXIT_FAILURE;
    }

    cpu = init_visc();

    rom_init(ROM_START, ROM_END, file);
    ram_init(RAM_START, RAM_END);

    syscon_init(SYSCON_START, SYSCON_END);
    uart_init(UART_START, UART_END);

    printf("-----------------------------------------\n");
    printf("               VISC-I ISA                \n");
    printf("    Emulator written by Kevin Alavik     \n");
    printf("  Credits to lolguy91 for creating VISC  \n");
    printf("                                         \n");
    printf("  RAM: %.0f\tKB                          \n", bytes_to_kb(RAM_END - RAM_START));
    printf("       %.0f\tMB                          \n", bytes_to_mb(RAM_END - RAM_START));
    printf("       %.0f\tGB                          \n", bytes_to_gb(RAM_END - RAM_START));
    printf("                                         \n");
    printf("  ROM: %dB                               \n", ROM_END - ROM_START);
    printf("  BOOT IMG: \"%s\"                       \n", filename);
    printf("                                         \n");
    printf("-----------------------------------------\n");

    while(runEmu) {
        run_visc(cpu, CLOCK_SPEED);
        bus_write(SYSCON_START, SYSCON_SHUTDOWN);
    }

    if(dump) {
        switch_plane(cpu, 0);
        printf("A: 0x%08X\n", cpu->curPlane[A]);
        printf("B: 0x%08X\n", cpu->curPlane[B]);
        printf("C: 0x%08X\n", cpu->curPlane[C]);
        printf("D: 0x%08X\n", cpu->curPlane[D]);
        printf("E: 0x%08X\n", cpu->curPlane[E]);
        printf("F: 0x%08X\n", cpu->curPlane[F]);
        printf("G: 0x%08X\n", cpu->curPlane[G]);
        printf("H: 0x%08X\n", cpu->curPlane[H]);
        printf("A1: 0x%08X\n", cpu->curPlane[A1]);
        printf("A2: 0x%08X\n", cpu->curPlane[A2]);
        printf("A3: 0x%08X\n", cpu->curPlane[A3]);
        printf("A4: 0x%08X\n", cpu->curPlane[A4]);
        printf("A5: 0x%08X\n", cpu->curPlane[A5]);
        printf("A6: 0x%08X\n", cpu->curPlane[A6]);
        printf("A7: 0x%08X\n", cpu->curPlane[A7]);
        printf("A8: 0x%08X\n", cpu->curPlane[A8]);
        switch_plane(cpu, 1);
        printf("A_SHADOW: 0x%08X\n", cpu->curPlane[A_SHADOW]);
        printf("B_SHADOW: 0x%08X\n", cpu->curPlane[B_SHADOW]);
        printf("C_SHADOW: 0x%08X\n", cpu->curPlane[C_SHADOW]);
        printf("D_SHADOW: 0x%08X\n", cpu->curPlane[D_SHADOW]);
        printf("E_SHADOW: 0x%08X\n", cpu->curPlane[E_SHADOW]);
        printf("F_SHADOW: 0x%08X\n", cpu->curPlane[F_SHADOW]);
        printf("G_SHADOW: 0x%08X\n", cpu->curPlane[G_SHADOW]);
        printf("H_SHADOW: 0x%08X\n", cpu->curPlane[H_SHADOW]);
        printf("PC: 0x%08X\n", cpu->curPlane[PC]);
        printf("SP: 0x%08X\n", cpu->curPlane[SP]);
        printf("BP: 0x%08X\n", cpu->curPlane[BP]);
        printf("RESERVED: 0x%08X\n", cpu->curPlane[RESERVED_1]);
        printf("RESERVED: 0x%08X\n", cpu->curPlane[RESERVED_2]);
        printf("STATUS: 0x%08X\n", cpu->curPlane[STATUS]);
        printf("HARTID: 0x%08X\n", cpu->curPlane[HARTID]);
        printf("FLAGS: 0x%08X\n", cpu->curPlane[FLAGS]);
    }

    fclose(file);

    return 0;
}
