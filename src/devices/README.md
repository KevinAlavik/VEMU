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
    - Note: One sector takes up 512 bytes. And the storage controller has 512 \* 16 bytes of RAM (`0x2000`). Note that you cant just read at `0x80000203` for the first sector, the device only has a size of 1. So it ends at its location. Meaning to read you would do something like this (example in C):
      ```c
      cpu->low_plane[A1] = starting_sector;
      cpu->low_plane[A2] = ammount_of_sectors;
      cpu->low_plane[A3] = address;
      bus_write(0x80000003, 0x01);
      ```
      This really just gets the raw disk data. See "Get Sector(s)"
  - Commands:
    | Name | ID | Parameters | Note |
    |---------------|------|------------------------------------|------------------------------------|
    | Get Sector(s) | 0x01 | A1: Sector Number A2: Sector Count A3: Data out | Reads $ of sectors starting from #, puts the data into address % |
    | Write Sector(s) | 0x02 | A1: Sector Number A2: Sector Count A3: Data pointer | Writes data from \* to sector starting from # until $|
