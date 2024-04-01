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
## Emulator Specific
