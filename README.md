# VEMU
A VISC-I ISA emulator built in C. 
<img width="1795" alt="image" src="https://github.com/KevinAlavik/VEMU/assets/95900603/87a80e7f-0aff-4094-b85b-03b9b358e9ea">
*Example running roms/test.bin*

## Building
To build VEMU run `make`.

## Installing
To install just run `make install` then `vemu -v`

## Usage
`Usage: vemu [OPTION]... [FILE]`

## Debuting
To enable debug logging run the emulator with the `-l` flag. To enable register dumping run with the `-d` flag. To dump the entire ROM region in memory (after running) add the `-r`flag.

## Arch
This follows the VISC arch see [SPECS.md](SPECS.md).

## License
This project is published under the GPL-v3 license see [LICENSE](LICENSE)
