#include "syscon.h"
#include "uart.h"
#include "../visc.h"

uint32_t syscon_base;

uint32_t syscon_read(uint32_t addr)
{
    // no read fuck you
    return 0; 
}

void syscon_write(uint32_t addr, uint32_t data)
{
    switch(data) {
        case SYSCON_SHUTDOWN:
            #ifdef DEBUG_LOG
                printf("[VISC - Syscon] Triggered SHUTDOWN!\n");
            #endif
            shouldRun = false;
            runEmu = false; // Make sure to kill the emulator too
            break;
        case SYSCON_RESET:
            #ifdef DEBUG_LOG
                printf("[VISC - Syscon] Triggered RESET!\n");
            #endif
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
            break;
       case SYSCON_DUMP:
            #ifdef DEBUG_LOG
                printf("[VISC - Syscon] Triggered DUMP!\n");
            #endif
            if (cpu->low_plane[A1] == 0) {
                switch_plane(cpu, 0);
                uart_print("A: 0x%08X\n", cpu->curPlane[A]);
                uart_print("B: 0x%08X\n", cpu->curPlane[B]);
                uart_print("C: 0x%08X\n", cpu->curPlane[C]);
                uart_print("D: 0x%08X\n", cpu->curPlane[D]);
                uart_print("E: 0x%08X\n", cpu->curPlane[E]);
                uart_print("F: 0x%08X\n", cpu->curPlane[F]);
                uart_print("G: 0x%08X\n", cpu->curPlane[G]);
                uart_print("H: 0x%08X\n", cpu->curPlane[H]);
                uart_print("A1: 0x%08X\n", cpu->curPlane[A1]);
                uart_print("A2: 0x%08X\n", cpu->curPlane[A2]);
                uart_print("A3: 0x%08X\n", cpu->curPlane[A3]);
                uart_print("A4: 0x%08X\n", cpu->curPlane[A4]);
                uart_print("A5: 0x%08X\n", cpu->curPlane[A5]);
                uart_print("A6: 0x%08X\n", cpu->curPlane[A6]);
                uart_print("A7: 0x%08X\n", cpu->curPlane[A7]);
                uart_print("A8: 0x%08X\n", cpu->curPlane[A8]);
            } else if (cpu->low_plane[A1] == 1) {
                switch_plane(cpu, 1);
                uart_print("A_SHADOW: 0x%08X\n", cpu->curPlane[A_SHADOW]);
                uart_print("B_SHADOW: 0x%08X\n", cpu->curPlane[B_SHADOW]);
                uart_print("C_SHADOW: 0x%08X\n", cpu->curPlane[C_SHADOW]);
                uart_print("D_SHADOW: 0x%08X\n", cpu->curPlane[D_SHADOW]);
                uart_print("E_SHADOW: 0x%08X\n", cpu->curPlane[E_SHADOW]);
                uart_print("F_SHADOW: 0x%08X\n", cpu->curPlane[F_SHADOW]);
                uart_print("G_SHADOW: 0x%08X\n", cpu->curPlane[G_SHADOW]);
                uart_print("H_SHADOW: 0x%08X\n", cpu->curPlane[H_SHADOW]);
                uart_print("PC: 0x%08X\n", cpu->curPlane[PC]);
                uart_print("SP: 0x%08X\n", cpu->curPlane[SP]);
                uart_print("BP: 0x%08X\n", cpu->curPlane[BP]);
                uart_print("RESERVED: 0x%08X\n", cpu->curPlane[RESERVED_1]);
                uart_print("RESERVED: 0x%08X\n", cpu->curPlane[RESERVED_2]);
                uart_print("STATUS: 0x%08X\n", cpu->curPlane[STATUS]);
                uart_print("HARTID: 0x%08X\n", cpu->curPlane[HARTID]);
                uart_print("FLAGS: 0x%08X\n", cpu->curPlane[FLAGS]);
            } else {
                printf("[VISC - Syscon] Invalid command \"%d\"", cpu->low_plane[A1]);
                exit(EXIT_FAILURE);
            }
            break;
        default:
            printf("[VISC - Syscon] Unknown command \"%d\"", data);
            break;
    }
}

void syscon_init(uint32_t base, uint32_t size)
{
    syscon_base = base;
    add_device(base, size, syscon_read, syscon_write);
}