# VISC-I Devices
**Todo**
## Default Devices
- Timer
- Syscon
    - `0x80000000`
    - Parameter Registers:
        - A
        - B
    - Commands:
        | Name               | ID   | Parameters           | Note                                                      |
        |--------------------|------|----------------------|-----------------------------------------------------------|
        | Shutdown           | 0x01 |                      |                                                           |
        | Reset              | 0x02 |                      | Reboots the CPU, resets all registers (HPLANE and LPLANE) |
        | Dump (UART Needed) | 0x03 | A: Page (2 for both) | Physical access to register block is needed               |

## Optional Devices
- UART
    - `0x80000001`
    - Parameter Registers:
        - A
    - Commands:
        | Name  | ID   | Parameters            | Note |
        |-------|------|-----------------------|------|
        | Write | 0x01 | A: Character to write |      |
        | Clear | 0x02 |                       |      |