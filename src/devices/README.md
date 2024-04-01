# VISC-I Devices
**Todo**
- Device Parameter Registers:
    - G
    - H
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
        | Dump (UART Needed) | 0x03 | G: Plane (2 for both) | Physical access to register block is needed               |

## Optional Devices
- UART
    - Location: `0x80000003`
    - Commands:
        | Name  | ID   | Parameters            | Note |
        |-------|------|-----------------------|------|
        | Write | 0x01 | G: Character to write |      |
        | Clear | 0x02 |                       |      |