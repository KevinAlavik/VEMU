# The VISC-I ISA
## The goal
The goal of VISC is to be a middle ground between CISC and RISC
## Privilege levels
1. Kernel mode (**Ring 0**) can access everything and is <ins>**not**</ins> configurable
2. Driver mode 1 (**Ring 1**) cannot do anything by default, but is configurable
3. Driver mode 2 (**Ring 2**) cannot do anything by default, but it is configurable
4. User mode (**Ring 3**) can do the bare minimum by default, but it is configurable
## Registers and register planes
A "Register plane" is a set of 16 registers that can be used at one time. A Restricted register is in **bold** and  is not accessible in user mode by default.
| Register Plane 0 | Register Plane 1 |
|------------------|------------------|
| A                | A_SHADOW         |
| B                | B_SHADOW         |
| C                | C_SHADOW         |
| D                | D_SHADOW         |
| E                | E_SHADOW         |
| F                | F_SHADOW         |
| G                | G_SHADOW         |
| H                | H_SHADOW         |
| A1               | **PC**           |
| A2               | **SP**           |
| A3               | **BP**           |
| A4               | RESERVED         |
| A5               | RESERVED         |
| A6               | **STATUS**       |
| A7               | **HARTID**       |
| A8               | **FLAGS**        |

The "shadow" group of registers is an extended group of general purpose registers.

## Instruction Format
Each instruction spans across 2 addresses in the buss. For a maximum size of 64 bit (32 for each address). So something like:

`0x00000000`: `0xFFFFFFFF`

`0x00000001`: `0xFFFFFFFF`

In this case the `0x00000000` has the first part of the instruction and `0x00000001` has the second part of it. These are the parts in the instruction:
- IMM: 32bit immediate value.(In this example it would be whats at `0x00000000`).
- DATA: 8bits of miscellaneous data.
- SR2: 4bit identifier of a source register 
- SR1: 4bit identifier of a source register 
- OPCODE: 8bit opcode
- CLASS: 4bit class identifier 

```mermaid
graph TD;
    A-->B;
    A-->C;
    B-->D;
    C-->D;
```