#include "bridge.h"
#include <string.h>

#define VERSION_STR "v0.1.4"
int clock_speed = 10;

VISC_I *cpu;
bool runEmu = true;
bool dump = false;
int rom_size = ROM_END;

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

// Option shit
void list_extensions()
{
    printf("VEMU (%s) %s\n", __DATE__, VERSION_STR);
    printf("Extensions:\n");
    printf("  - %d:BASIC_SHIT  (%c) \t%s\n", BASIC_SHIT, get_extension_letter(BASIC_SHIT), extension_enabled(cpu, BASIC_SHIT) ? "ON" : "OFF");
    printf("  - %d:MULTIPLY    (%c) \t%s\n", MULTIPLY, get_extension_letter(MULTIPLY), extension_enabled(cpu, MULTIPLY) ? "ON" : "OFF");
}

void list_devices()
{
    printf("VEMU (%s) %s\n", __DATE__, VERSION_STR);
    printf("Devices:\n");
    printf("  - [%s] SYSCON:\t0x80000001\n", sysconEnabled ? "X" : " ");
    printf("  - [%s] UART:\t0x80000003\n\n", uartEnabled ? "X" : " ");
}

void usage(char *s)
{
    printf("Usage: %s [OPTION]... [FILE]\n", s);
    printf("Emulate a VISC-I ISA CPU.\n\n");
    printf("Options:\n");
    printf("  -h,   --help                display this help and exit\n");
    printf("  -v,   --version             output version information and exit\n");
    printf("  -i,   --info                outputs the info box\n");
    printf("  -d,   --dump                dumps the register on shutdown\n");
    printf("  -l,   --debug               enable debug logging in the emulator\n");
    printf("  -s,   --step                enable step mode\n");
    printf("  -r,   --dump-rom            dumps the memory region with the ROM\n");
    printf("  -ld,  --devices             lists all available devices\n");
    printf("  -mm,  --memory-map          outputs the memory map in a easy to read format\n");
    printf("  -le,  --list-extensions     outputs all available extensions \n");
    printf("  -e,   --extensions          manage extensions (enable, disable, list)\n");
    printf("  -ud,  --uart-disable        disable the builtin UART device\n");
    printf("  -ue,  --uart-enable         enable the builtin UART device (enabled by default)\n");
    printf("  -sd,  --syscon-disable      disable the builtin SYSCON device\n");
    printf("  -se,  --syscon-enable       enable the builtin SYSCON device (enabled by default)\n");
    printf("  -rs,  --rom-size            sets the ROM size (0xFF by default)\n");
    printf("  -c,   --clock-speed         sets the clock speed (10 by default. 0 for as fast as possible)\n");
}

// Emulator entry
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        usage(argv[0]);
        return 1;
    }

    char *filename = NULL;
    bool dumpm = false;
    bool info = false;

    cpu = init_visc();

    ram_init(RAM_START, RAM_END);
    syscon_init(SYSCON_START);
    uart_init(UART_START);

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
        {
            printf("VEMU (%s) %s\n", __DATE__, VERSION_STR);
            return 0;
        }
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            usage(argv[0]);
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
        else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--step") == 0)
        {
            debug_step = true;
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
            list_devices();
            return 0;
        }
        else if (strcmp(argv[i], "-mm") == 0 || strcmp(argv[i], "--memory-map") == 0)
        {
            printf("VEMU (%s) %s\n", __DATE__, VERSION_STR);
            printf("Memory Map:\n");
            printf("  - ROM:\t\t0x00000000 -> 0x000000FF\n");
            printf("  - RAM:\t\t0x00000100 -> 0x80000000\n");
            printf("    - Init Stack:\t0x00000100 -> 0x00000FFF\n");
            printf("  - Devices:\t\t0x80000001 -> 0xFFFFFFFF\n\n");
            return 0;
        }
        else if (strcmp(argv[i], "-le") == 0 || strcmp(argv[i], "--list-extensions") == 0)
        {
            list_extensions();
            return 0;
        }
        else if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--extensions") == 0)
        {
            if (i + 1 < argc)
            {
                if (strcmp(argv[i + 1], "list") == 0)
                {
                    list_extensions();
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
                        printf("[VISC] \x1B[0;33mWARNING!\x1B[0m Disabling some of the CPU extensions may cause unexpected/broken behaviour!\n");
                        int id;
                        if (isnum(argv[i + 2]))
                            id = atoi(argv[i + 2]);
                        else
                            id = get_extension_id(argv[i + 2]);

                        if (id == BASIC_SHIT)
                            printf("[VISC] \x1B[0;33mWARNING!\x1B[0m Disabling the \"BASIC_SHIT\" (%d) extension. Will make CPU useless. It will disable all basic instructions (LD, ST, PUSH, POP, etc.)!\n", id);

                        if (id == MULTIPLY)
                            printf("[VISC] \x1B[0;33mWARNING!\x1B[0m Disabling the \"MULTIPLY\" (%d) extension. Will disable the MUL and DIV instructions!\n", id);

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
        else if (strcmp(argv[i], "-ud") == 0 || strcmp(argv[i], "--uart-disable") == 0)
        {
            printf("[VISC] \x1B[0;33mWARNING!\x1B[0m Disabling the UART makes the CPU unable to print out text via the emulator!\n");
            uartEnabled = false;
        }
        else if (strcmp(argv[i], "-ue") == 0 || strcmp(argv[i], "--uart-enable") == 0)
        {
            uartEnabled = true;
        }
        else if (strcmp(argv[i], "-sd") == 0 || strcmp(argv[i], "--syscon-disable") == 0)
        {
            printf("[VISC] \x1B[0;33mWARNING!\x1B[0m Disabling the SYSCON makes the CPU unable to reive shutdown/reset/dump interrupts!\n");
            sysconEnabled = false;
        }
        else if (strcmp(argv[i], "-se") == 0 || strcmp(argv[i], "--syscon-enable") == 0)
        {
            sysconEnabled = true;
        }
        else if (strcmp(argv[i], "-rs") == 0 || strcmp(argv[i], "--rom-size") == 0)
        {
            if (i + 1 < argc)
            {
                printf("[VISC] \x1B[0;33mWARNING!\x1B[0m Setting ROM size may cause issue since the other devices does'nt dynamicly get there size. So if you go over 0xFF (256) it might override ROM (or init stack since thats first)!\n");
                rom_size = atoi(argv[i + i]);
            }
            else
            {

                printf("%s expected a argument. Usage %s %s [size]\n", argv[i], argv[0], argv[i]);
            }
        }
        else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--clock-speed") == 0)
        {
            if (i + 1 < argc)
            {
                clock_speed = atoi(argv[i + 1]);
            }
            else
            {

                printf("%s expected a argument. Usage %s %s [speed (Mhz)]\n", argv[i], argv[0], argv[i]);
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

    rom_init(ROM_START, rom_size, file);

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
        printf("    - [%s] SYSCON                        \n", sysconEnabled ? "X" : " ");
        printf("    - [%s] UART                          \n", uartEnabled ? "X" : " ");
        printf("                                         \n");
        printf("-----------------------------------------\n");
    }

    while (runEmu)
    {
        run_visc(cpu, clock_speed);
        bus_write(SYSCON_START, SYSCON_SHUTDOWN);
    }

    // TODO: Dont make it need to enable the buss after shutdown, this should'nt be possible if the CPU is powered of
    if (dump)
    {
        busEnable = true;
        cpu->low_plane[A1] = 2; // Dump all pages
        bus_write(SYSCON_START, SYSCON_DUMP);
        busEnable = false;
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
