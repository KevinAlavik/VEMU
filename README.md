# VEMU
A VISC-I ISA emulator built in C. 
<img width="1179" alt="image" src="https://github.com/KevinAlavik/VEMU/assets/95900603/ca3768db-27ef-434d-bfa4-bf69c5f6c4a7">
*Example running roms/test.bin*

## Building
To build VEMU run `make` but to build with debug logging do `make debug`. If you want no debug logging but still want to see the registers last values (dump). Then run `make dump`

**To dump the registers you dont need to build with dump, you can run using the -d (--dump) option in VEMU**

## Installing
To install just run `make install` then `vemu -v`

## Usage
`Usage: vemu [OPTION]... [FILE]`

## Arch
This follows the VISC arch see [SPECS.md](SPECS.md).

## License
This project is published under the GPL-v3 license see [LICENSE](LICENSE)
