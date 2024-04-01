#include "bridge.h"
#include <string.h>

#define CLOCK_SPEED 10

VISC_I *cpu;
bool runEmu = true; 
bool dump = false;

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
        printf("  -h, --help           display this help and exit\n");
        printf("  -v, --version        output version information and exit\n");
        printf("  -d, --dump           dumps the register on shutdown\n");
        printf("  -l, --debug          enable debug logging in the emulator\n");
        printf("  -r, --dump-rom       dumps the memory region with the ROM\n");
        return 1;
    }

    char* filename = NULL;
    bool dumpm = false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            printf("VEMU (%s) v0.1.4\n", __DATE__);
            return 0;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printf("Usage: %s [OPTION]... [FILE]\n", argv[0]);
            printf("Emulate a VISC-I ISA CPU.\n\n");
            printf("Options:\n");
            printf("  -h, --help           display this help and exit\n");
            printf("  -v, --version        output version information and exit\n");
            printf("  -d, --dump           dumps the register on shutdown\n");
            printf("  -l, --debug          enable debug logging in the emulator\n");
            printf("  -r, --dump-rom       dumps the memory region with the ROM\n");
            return 0;
        } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--dump") == 0) {
            dump = true;
        } else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--debug") == 0) {
            debug_log = true;
        }  else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--dump-rom") == 0) {
           dumpm = true;
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


    printf("-----------------------------------------\n");
    printf("               VISC-I ISA                \n");
    printf("    Emulator written by Kevin Alavik     \n");
    printf("  Credits to lolguy91 for creating VISC  \n");
    printf("                                         \n");
    printf("  RAM:        %.0f\tKB                   \n", bytes_to_kb(RAM_END - RAM_START));
    printf("              %.0f\tMB                   \n", bytes_to_mb(RAM_END - RAM_START));
    printf("              %.0f\t\tGB                 \n", bytes_to_gb(RAM_END - RAM_START));
    printf("                                         \n");
    printf("  FULL:       %.0f\tKB                   \n", bytes_to_kb(0xFFFFFFFF));
    printf("              %.0f\tMB                   \n", bytes_to_mb(0xFFFFFFFF));
    printf("              %.0f\t\tGB                 \n", bytes_to_gb(0xFFFFFFFF));
    printf("                                         \n");
    printf("  ROM:        %dB                        \n", ROM_END - ROM_START);
    printf("  BOOT IMG:   \"%s\"                     \n", filename);
    printf("  SCREEN:     false                      \n");
    printf("  UART:       true                       \n");
    printf("                                         \n");
    printf("-----------------------------------------\n");
    
    cpu = init_visc();

    rom_init(ROM_START, ROM_END, file);
    ram_init(RAM_START, RAM_END);

    syscon_init(SYSCON_START, SYSCON_END);
    uart_init(UART_START, UART_END);

    while(runEmu) {
        run_visc(cpu, CLOCK_SPEED);
        bus_write(SYSCON_START, SYSCON_SHUTDOWN);
    }

    if(dumpm) {
        busEnable = true;
        for(uint32_t i = ROM_START; i < ROM_END; i++) {
            uint32_t v = bus_read(i);
            printf("0x%08X: 0x%08X\n", i, v);
        }
        busEnable = false;
    }

    fclose(file);

    return 0;
}
