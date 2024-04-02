# VISC-I Devices

**Todo**

- Device Parameter Registers:
  - A1
  - A2
  - A3

## Default Devices

- Timer
  - Unimpelemented, waiting for interupts.
- Syscon
  - Location: `0x80000001`
  - Commands:
    | Name | ID | Parameters | Note |
    |--------------------|------|-----------------------|-----------------------------------------------------------|
    | Shutdown | 0x01 | | |
    | Reset | 0x02 | | Reboots the CPU, resets all registers (HPLANE and LPLANE) |
    | Dump (UART Needed) | 0x03 | A1: Plane (2 for both) | Physical access to register block is needed |

## Optional Devices

- UART

  - Location: `0x80000002`
  - Commands:
    | Name | ID | Parameters | Note |
    |-------|------|------------------------|------|
    | Write | 0x01 | A1: Character to write | |
    | Clear | 0x02 | | |

- Storage Controller
  - Location: `0x80000003`
    - Note: Takes up 512 bytes. (Internal storage RAM, which is at `0x8000004` -`0x8000204`. Non accessible). So the device would end at `0x8000204`.
  - Commands:
    | Name | ID | Parameters | Note |
    |---------------|------|------------------------------------|------------------------------------|
    | Get Sector(s) | 0x01 | A1: Sector Number A2: Sector Count | Reads $ of sectors starting from # |
    | Write Sector(s) | 0x02 | A1: Sector Number A2: Sector Count A3: Data pointer | Writes data from \* to sector starting from # until $|
