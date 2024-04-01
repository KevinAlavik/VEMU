# VISC-I Devices
**Todo**
- Device Parameter Registers:
    - A1
    - A2
## Default Devices
- Timer
    - Unimpelemented, waiting for interupts.
- Syscon
    - Location: `0x80000001`
    - Commands:
        | Name               | ID   | Parameters            | Note                                                      |
        |--------------------|------|-----------------------|-----------------------------------------------------------|
        | Shutdown           | 0x01 |                       |                                                           |
        | Reset              | 0x02 |                       | Reboots the CPU, resets all registers (HPLANE and LPLANE) |
        | Dump (UART Needed) | 0x03 | A1: Plane (2 for both) | Physical access to register block is needed               |

## Optional Devices
- UART
    - Location: `0x80000003`
    - Commands:
        | Name  | ID   | Parameters            | Note |
        |-------|------|-----------------------|------|
        | Write | 0x01 | A1: Character to write |      |
        | Clear | 0x02 |                       |      |