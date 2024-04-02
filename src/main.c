#include "bridge.h"
#include <string.h>

#define VERSION_STR "v0.1.4"
int clock_speed = 10;

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

void hexdump(uint32_t start, uint32_t size)
{
    for (uint32_t i = 0; i < size; i += 16)
    {
        printf("%08X: ", start + i);
        for (int j = 0; j < 4; j++)
        {
            if (i + j * 4 < size)
            {
                uint32_t addr = start + i + j * 4;
                busEnable = true;
                uint32_t v = bus_read(addr);
                busEnable = false;
                printf("%08X ", v);
            }
            else
            {
                printf("         ");
            }
        }
        printf(" ");
        for (int j = 0; j < 16; j++)
        {
            if (i + j < size)
            {
                uint32_t addr = start + i + j;
                busEnable = true;
                uint32_t v = bus_read(addr);
                busEnable = false;
                printf("%c", (v >= 32 && v <= 126) ? (char)v : '.');
            }
        }
        printf("\n");
    }
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
    printf("  - [%s] UART:\t0x80000002\n\n", uartEnabled ? "X" : " ");
    printf("  - [%s] Storage:\t0x80000003\n\n", storageEnabled ? "X" : " ");
}

void usage(char *s)
{
    printf("Usage: %s [OPTION]... [FILE]\n", s);
    printf("Emulate a VISC-I ISA CPU.\n\n");
    printf("Options:\n");
    printf("  -h,   --help                                          display this help and exit\n");
    printf("  -v,   --version                                       output version information and exit\n");
    printf("  -i,   --info                                          outputs the info box\n");
    printf("  -d,   --dump                                          dumps the register on shutdown\n");
    printf("  -l,   --debug                                         enable debug logging in the emulator\n");
    printf("  -s,   --step                                          enable step mode\n");
    printf("  -r,   --dump-rom                                      dumps the memory region with the ROM\n");
    printf("  -dd,  --dump-disk                                     dumps the memory region where the disk is located\n");
    printf("  -ld,  --devices                                       lists all available devices\n");
    printf("  -mm,  --memory-map                                    outputs the memory map in a easy to read format\n");
    printf("  -le,  --list-extensions                               outputs all available extensions \n");
    printf("  -e,   --extensions        [enable/disable/list]       manage extensions (enable, disable, list)\n");
    printf("  -ud,  --uart-disable                                  disable the builtin UART device\n");
    printf("  -ue,  --uart-enable                                   enable the builtin UART device (enabled by default)\n");
    printf("  -sd,  --syscon-disable                                disable the builtin SYSCON device\n");
    printf("  -se,  --syscon-enable                                 enable the builtin SYSCON device (enabled by default)\n");
    printf("  -rs,  --rom-size          [size]                      sets the ROM size (0xFF by default)\n");
    printf("  -c,   --clock-speed       [speed]                     sets the clock speed (10 by default. 0 for as fast as possible)\n");
    printf("  -di,  --disk-image        [path]                      the disk image that the storage drive will load\n");
    printf("  -ds,  --dump-stack                                    dumps the stack (from SP to BP)\n");
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
    bool dumpd = false;
    bool stack_dump = false;
    bool ld = false;
    bool le = false;
    bool info = false;
    char *disk_img = NULL;

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
        else if (strcmp(argv[i], "-dd") == 0 || strcmp(argv[i], "--dump-disk") == 0)
        {
            dumpd = true;
        }
        else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--info") == 0)
        {
            info = true;
        }
        else if (strcmp(argv[i], "-ld") == 0 || strcmp(argv[i], "--devices") == 0)
        {
            ld = true;
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
            le = true;
        }
        else if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--extensions") == 0)
        {
            if (i + 1 < argc)
            {
                if (strcmp(argv[i + 1], "list") == 0)
                {
                    le = true;
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
                        printf("[VISC] \x1B[31mERROR\x1B[0m Missing argument for 'enable'\n");
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
                        printf("[VISC] \x1B[31mERROR\x1B[0m Missing argument for 'disable'\n");
                        return EXIT_FAILURE;
                    }
                }
                else
                {
                    printf("[VISC] \x1B[31mERROR\x1B[0m Unknown subcommand for '%s' got '%s'\n", argv[i], argv[i + 1]);
                    return EXIT_FAILURE;
                }
            }
            else
            {
                printf("[VISC] \x1B[31mERROR\x1B[0m Missing subcommand for '%s'. Available subcommands:\n", argv[i]);
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

                printf("[VISC] \x1B[31mERROR\x1B[0m %s expected a argument. Usage %s %s [size]\n", argv[i], argv[0], argv[i]);
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

                printf("[VISC] \x1B[31mERROR\x1B[0m %s expected a argument. Usage %s %s [speed (Mhz)]\n", argv[i], argv[0], argv[i]);
            }
        }
        else if (strcmp(argv[i], "-di") == 0 || strcmp(argv[i], "--disk-image") == 0)
        {
            if (i + 1 < argc)
            {
                disk_img = argv[i + 1];
            }
            else
            {

                printf("[VISC] \x1B[31mERROR\x1B[0m %s expected a argument. Usage %s %s [disk image (path)]\n", argv[i], argv[0], argv[i]);
            }
        }
        else if (strcmp(argv[i], "-ds") == 0 || strcmp(argv[i], "--dump-stack") == 0)
        {
            stack_dump = true;
        }
        else
        {
            if (argv[i][0] == '-' || (argv[i][0] == '-' && argv[i][1] == '-'))
            {
                printf("[VISC] \x1B[31mERROR\x1B[0m Invalid option \"%s\"\n", argv[i]);
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
        printf("[VISC] \x1B[31mERROR\x1B[0m No input file specified\n");
        return EXIT_FAILURE;
    }

    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        perror("[VISC] \x1B[31mERROR\x1B[0m Error opening ROM file");
        return EXIT_FAILURE;
    }

    FILE *disk_raw = NULL;

    if (disk_img != NULL)
    {
        disk_raw = fopen(disk_img, "rb");
        if (disk_raw == NULL)
        {
            perror("[VISC] \x1B[31mERROR\x1B[0m Error opening disk image");
            return EXIT_FAILURE;
        }
    }

    // Setup the ROM
    rom_init(ROM_START, rom_size, file);

    // Setup the storage device
    if (disk_raw == NULL)
    {
        printf("[VISC] \x1B[0;33mWARNING!\x1B[0m No storage device enabled.\n");
    }
    else
    {
        storage_init(STORAGE_START, disk_raw);

        cpu->low_plane[A1] = 0;
        cpu->low_plane[A2] = MAX_SECTORS;
        cpu->low_plane[A3] = DEFAULT_STACK_END + 1;
        bus_write(STORAGE_START, STORAGE_READ);
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
        printf("  ROM:        %d\tB                      \n", rom_size - ROM_START);
        printf("                                         \n");
        printf("  EXTENSIONS:                            \n");
        printf("    - [%s] BASIC_SHIT                    \n", extension_enabled(cpu, BASIC_SHIT) ? "X" : " ");
        printf("    - [%s] MULTIPLY                      \n", extension_enabled(cpu, MULTIPLY) ? "X" : " ");
        printf("                                         \n");
        printf("  DEVICES:                               \n");
        printf("    - [%s] SYSCON                        \n", sysconEnabled ? "X" : " ");
        printf("    - [%s] UART                          \n", uartEnabled ? "X" : " ");
        printf("    - [%s] Storage                       \n", storageEnabled ? "X" : " ");
        printf("                                         \n");
        printf("-----------------------------------------\n");
    }

    if (ld)
        list_devices();

    if (le)
        list_extensions();

    // Hacky trick to clear stack
    for (int i = DEFAULT_STACK_START; i < DEFAULT_STACK_END; i++)
    {
        bus_write(i, 0);
    }

    while (runEmu)
    {
        run_visc(cpu, clock_speed);
        // Make sure to kill the CPU and emulator.
        bus_write(SYSCON_START, SYSCON_SHUTDOWN);
    }

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
    if (dumpd)
    {
        if (!storageEnabled)
            return 0;

        for (int i = 0; i < MAX_SECTORS; i++)
        {
            printf("Sector %d:\n", i + 1);
            hexdump((DEFAULT_STACK_END + 1) + (0x200 * i), 0x200);
        }
    }

    if (stack_dump)
    {
        hexdump(cpu->high_plane[SP], cpu->high_plane[BP] - cpu->high_plane[SP]);
    }

    fclose(file);

    return 0;
}
