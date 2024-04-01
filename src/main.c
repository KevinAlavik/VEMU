#include "bridge.h"
#include <string.h>

#define CLOCK_SPEED 10

VISC_I *cpu;
bool runEmu = true;
bool dump = false;

// Utility functions
double bytes_to_kb(unsigned long long bytes_value)
{
    double kb_value = bytes_value / 1024.0;
    return kb_value;
}

double bytes_to_mb(unsigned long long bytes_value)
{
    double mb_value = bytes_value / (1024.0 * 1024.0);
    return mb_value;
}

double bytes_to_gb(unsigned long long bytes_value)
{
    double gb_value = bytes_value / (1024.0 * 1024.0 * 1024.0);
    return gb_value;
}

bool isnum(const char *str)
{
    if (str == NULL || *str == '\0')
        return false;

    bool hasDigit = false;
    bool hasDecimal = false;
    bool hasSign = false;

    while (isspace(*str))
        str++;

    if (*str == '+' || *str == '-')
    {
        hasSign = true;
        str++;
    }

    if (!isdigit(*str) && *str != '.')
        return false;

    while (*str != '\0')
    {
        if (isdigit(*str))
        {
            hasDigit = true;
        }
        else if (*str == '.')
        {
            if (hasDecimal || hasSign == false)
                return false;
            hasDecimal = true;
        }
        else
        {
            return false;
        }
        str++;
    }

    return hasDigit;
}

// Emulator entry
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s [OPTION]... [FILE]\n", argv[0]);
        printf("Emulate a VISC-I ISA CPU.\n\n");
        printf("Options:\n");
        printf("  -h,  --help                display this help and exit\n");
        printf("  -v,  --version             output version information and exit\n");
        printf("  -i,  --info                outputs the info box\n");
        printf("  -d,  --dump                dumps the register on shutdown\n");
        printf("  -l,  --debug               enable debug logging in the emulator\n");
        printf("  -r,  --dump-rom            dumps the memory region with the ROM\n");
        printf("  -ld, --devices             lists all available devices\n");
        printf("  -mm, --memory-map          outputs the memory map in a easy to read format\n");
        printf("  -le, --list-extensions     outputs all available extensions \n");
        printf("  -e,  --extensions          manage extensions (enable, disable, list)\n");
        return 1;
    }

    char *filename = NULL;
    bool dumpm = false;
    bool info = false;

    cpu = init_visc();

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
        {
            printf("VEMU (%s) v0.1.4\n", __DATE__);
            return 0;
        }
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            printf("Usage: %s [OPTION]... [FILE]\n", argv[0]);
            printf("Emulate a VISC-I ISA CPU.\n\n");
            printf("Options:\n");
            printf("  -h,  --help                display this help and exit\n");
            printf("  -v,  --version             output version information and exit\n");
            printf("  -i,  --info                outputs the info box\n");
            printf("  -d,  --dump                dumps the register on shutdown\n");
            printf("  -l,  --debug               enable debug logging in the emulator\n");
            printf("  -r,  --dump-rom            dumps the memory region with the ROM\n");
            printf("  -ld, --devices             lists all available devices\n");
            printf("  -mm, --memory-map          outputs the memory map in a easy to read format\n");
            printf("  -le, --list-extensions     outputs all available extensions \n");
            printf("  -e,  --extensions          manage extensions (enable, disable, list)\n");
            return 0;
        }
        else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--dump") == 0)
        {
            dump = true;
        }
        else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--debug") == 0)
        {
            debug_log = true;
        }
        else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--dump-rom") == 0)
        {
            dumpm = true;
        }
        else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--info") == 0)
        {
            info = true;
        }
        else if (strcmp(argv[i], "-ld") == 0 || strcmp(argv[i], "--devices") == 0)
        {
            printf("VEMU (%s) v0.1.4\n", __DATE__);
            printf("Devices:\n");
            printf("  - SYSCON:\t0x80000001\n");
            printf("  - UART:\t0x80000003\n\n");
            return 0;
        }
        else if (strcmp(argv[i], "-mm") == 0 || strcmp(argv[i], "--memory-map") == 0)
        {
            printf("VEMU (%s) v0.1.4\n", __DATE__);
            printf("Memory Map:\n");
            printf("  - ROM:\t\t0x00000000 -> 0x000000FF\n");
            printf("  - RAM:\t\t0x00000100 -> 0x80000000\n");
            printf("    - Init Stack:\t0x00000100 -> 0x00000FFF\n");
            printf("  - Devices:\t\t0x80000001 -> 0xFFFFFFFF\n\n");
            return 0;
        }
        else if (strcmp(argv[i], "-le") == 0 || strcmp(argv[i], "--list-extensions") == 0)
        {
            printf("VEMU (%s) v0.1.4\n", __DATE__);
            printf("Extensions:\n");
            printf("  - %d:BASIC_SHIT  (%c) \t%s\n", BASIC_SHIT, get_extension_letter(BASIC_SHIT), extension_enabled(cpu, BASIC_SHIT) ? "ON" : "OFF");
            printf("  - %d:MULTIPLY    (%c) \t%s\n", MULTIPLY, get_extension_letter(MULTIPLY), extension_enabled(cpu, MULTIPLY) ? "ON" : "OFF");
            return 0;
        }
        else if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--extensions") == 0)
        {
            if (i + 1 < argc)
            {
                if (strcmp(argv[i + 1], "list") == 0)
                {
                    printf("VEMU (%s) v0.1.4\n", __DATE__);
                    printf("Extensions:\n");
                    printf("  - %d:BASIC_SHIT  (%c) \t%s\n", BASIC_SHIT, get_extension_letter(BASIC_SHIT), extension_enabled(cpu, BASIC_SHIT) ? "ON" : "OFF");
                    printf("  - %d:MULTIPLY    (%c) \t%s\n", MULTIPLY, get_extension_letter(MULTIPLY), extension_enabled(cpu, MULTIPLY) ? "ON" : "OFF");
                }
                else if (strcmp(argv[i + 1], "enable") == 0)
                {
                    if (i + 2 < argc)
                    {
                        int id;
                        if (isnum(argv[i + 2]))
                            id = atoi(argv[i + 2]);
                        else
                            id = get_extension_id(argv[i + 2]);
                        enable_extension(cpu, id);
                        i++;
                    }
                    else
                    {
                        printf("Error: Missing argument for 'enable'\n");
                        return EXIT_FAILURE;
                    }
                }
                else if (strcmp(argv[i + 1], "disable") == 0)
                {
                    if (i + 2 < argc)
                    {
                        int id;
                        if (isnum(argv[i + 2]))
                            id = atoi(argv[i + 2]);
                        else
                            id = get_extension_id(argv[i + 2]);
                        disable_extension(cpu, id);
                        i++;
                    }
                    else
                    {
                        printf("Error: Missing argument for 'disable'\n");
                        return EXIT_FAILURE;
                    }
                }
                else
                {
                    printf("Error: Unknown subcommand for '%s' got '%s'\n", argv[i], argv[i + 1]);
                    return EXIT_FAILURE;
                }
            }
            else
            {
                printf("Error: Missing subcommand for '%s'. Available subcommands:\n", argv[i]);
                printf("  - enable      [id / name]\n");
                printf("  - disable     [id / name]\n");
                printf("  - list            \n");
                return EXIT_FAILURE;
            }
        }
        else
        {
            if (argv[i][0] == '-' || (argv[i][0] == '-' && argv[i][1] == '-'))
            {
                printf("Invalid option \"%s\"\n", argv[i]);
                return EXIT_FAILURE;
            }
            else
            {
                filename = argv[i];
            }
        }
    }

    if (filename == NULL)
    {
        printf("No input file specified\n");
        return EXIT_FAILURE;
    }

    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        perror("Error opening ROM file");
        return EXIT_FAILURE;
    }

    if (info)
    {
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
        printf("                                         \n");
        printf("  EXTENSIONS:                            \n");
        printf("    - [%s] BASIC_SHIT                    \n", extension_enabled(cpu, BASIC_SHIT) ? "X" : " ");
        printf("    - [%s] MULTIPLY                      \n", extension_enabled(cpu, MULTIPLY) ? "X" : " ");
        printf("                                         \n");
        printf("  DEVICES:                               \n");
        printf("    - [%s] SYSCON                        \n", busEnable ? "X" : " ");
        printf("    - [%s] UART                          \n", busEnable ? "X" : " ");
        printf("                                         \n");
        printf("-----------------------------------------\n");
    }

    rom_init(ROM_START, ROM_END, file);
    ram_init(RAM_START, RAM_END);

    syscon_init(SYSCON_START, SYSCON_END);
    uart_init(UART_START, UART_END);

    while (runEmu)
    {
        run_visc(cpu, CLOCK_SPEED);
        bus_write(SYSCON_START, SYSCON_SHUTDOWN);
    }

    if (dumpm)
    {
        busEnable = true;
        for (uint32_t i = ROM_START; i < ROM_END; i++)
        {
            uint32_t v = bus_read(i);
            printf("0x%08X: 0x%08X\n", i, v);
        }
        busEnable = false;
    }

    fclose(file);

    return 0;
}
