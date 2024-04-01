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
- DR: 4bit identifier of a destination register (output)
- SR2: 4bit identifier of a source register 
- SR1: 4bit identifier of a source register 
- OPCODE: 8bit opcode
- CLASS: 4bit class identifier 

So in `0xAAAAAAAABBCDEFFG`
- A = IMM
- B = DATA
- C = DR
- D = SR2
- E = SR1
- F = OPCODE
- G = CLASS

In memory it would be:
```
0x00000000: 0xAAAAAAAA
0x00000001: 0xBBCDEFFG
```

In the code `0x00000000` is represented as
 ```c
uint32_t val_low
````
and `0x00000001` is represented as
 ```c
uint32_t val_high
````

To decode these parts in the instruction based on these 2 32bit numbers we would do:
```c
void extract_instruction(uint32_t val_low, uint32_t val_high)
{
    int imm = val_low & 0xFFFFFFFF;
    int data = (val_high >> 24) & 0xFF;
    int dr = (val_high >> 20) & 0xF;
    int sr2 = (val_high >> 16) & 0xF;
    int sr1 = (val_high >> 12) & 0xF;
    int opcode = (val_high >> 4) & 0xF;
    int class = val_high & 0xF;
}
```

## Instruction classes
1. **CLASS_DATA**: CPU Management and data management
2. **CLASS_ARITH**: Arithmetic and logic
3. **CLASS_JUMP**: CPU Jump and branch logic
4. **CLASS_ALGO**: Common algorithms
5. **CLASS_FLOAT**: Float point extensions
5.	**CLASS_RESRV**: Reserved
6.	**CLASS_RESRV**: Reserved
7.	**CLASS_RESRV**: Reserved
8.	**CLASS_RESRV**: Reserved
9.	**CLASS_RESRV**: Reserved
10.	**CLASS_RESRV**: Reserved
11.	**CLASS_RESRV**: Reserved
12.	**CLASS_RESRV**: Reserved
13.	**CLASS_RESRV**: Reserved
14.	**CLASS_RESRV**: Reserved
15.	**CLASS_RESRV**: Reserved
16.	**CLASS_RESRV**: Reserved

## Extensions
| Letter | Name       | Function                                            |
|--------|------------|-----------------------------------------------------|
| A      | RESERVED   | RESERVED                                            |
| B      | BASIC_SHIT | Contains everything requires to be turning complete |
| C      | RESERVED   | RESERVED                                            |
| D      | RESERVED   | RESERVED                                            |
| E      | RESERVED   | RESERVED                                            |
| F      | RESERVED   | RESERVED                                            |
| G      | RESERVED   | RESERVED                                            |
| H      | RESERVED   | RESERVED                                            |
| I      | RESERVED   | RESERVED                                            |
| J      | RESERVED   | RESERVED                                            |
| K      | RESERVED   | RESERVED                                            |
| L      | RESERVED   | RESERVED                                            |
| M      | MULTIPLY   | Multiplication and division                         |
| N      | RESERVED   | RESERVED                                            |
| O      | RESERVED   | RESERVED                                            |
| P      | RESERVED   | RESERVED                                            |
| Q      | RESERVED   | RESERVED                                            |
| R      | RESERVED   | RESERVED                                            |
| S      | RESERVED   | RESERVED                                            |
| T      | RESERVED   | RESERVED                                            |
| U      | RESERVED   | RESERVED                                            |
| V      | RESERVED   | RESERVED                                            |
| W      | RESERVED   | RESERVED                                            |
| X      | RESERVED   | RESERVED                                            |
| Y      | RESERVED   | RESERVED                                            |
| Z      | RESERVED   | RESERVED                                            |

### Instruction Set
- **CLASS_DATA**
    1. **NOP** - Does nothing
    2. **LD**
        - **Description**: Loads a value into DR from address fetched from IMM
        - **Parameters**:
            - DR - Destination register
            - IMM - Address for value
    3. **ST**
        - **Description**: Stores a value from SR1 into address IMM
        - **Parameters**:
            - SR1 - Source register
            - IMM - Address for destination
    4. **LDI**
        - **Description**: Loads the immediate value IMM into register DR
        - **Parameters**:
            - DR - Destination register
            - IMM - Immediate value to load
    5. **LDP**
        - **Description**: Loads a value into DR from the address stored in the register SR1
        - **Parameters**:
            - DR - Destination register
            - SR1 - Source register storing address
    6. **STP**
        - **Description**: Stores a value from SR1 into the address stored in the register SR2
        - **Parameters**:
            - SR1 - Source register
            - SR2 - Destination register storing address
    7. **SRP**
        - **Description**: Switches to register plane IMM
        - **Parameters**:
            - IMM - Register plane to switch to
    8. **PUSH**
        - **Description**: Pushes the value in SR1 to the stack
        - **Parameters**:
            - SR1 - Source register
    9. **POP**
        - **Description**: Pops the value at the top of the stack into DR
        - **Parameters**:
            - DR - Destination register

- **CLASS_ALU**
    1. **ADD**
        - **Description**: Adds the values of SR1 and SR2 together and stores the result in DR
        - **Parameters**:
            - SR1 - Source register 1
            - SR2 - Source register 2
            - DR - Destination register
    2. **SUB**
        - **Description**: Subtracts the value of SR1 from the value of SR2 and stores the result in DR
        - **Parameters**:
            - SR1 - Source register 1
            - SR2 - Source register 2
            - DR - Destination register
    3. **RESERVED**
        - **Description**: Reserved, MUL (Needs the MULTIPLY extension)
    4. **RESERVED**
        - **Description**: Reserved, DIV (Needs the MULTIPLY extension)
    5. **SHL**
        - **Description**: Shifts the value of SR1 left by the value of SR2 bits and stores the result in DR
        - **Parameters**:
            - SR1 - Source register 1
            - SR2 - Source register 2 (number of bits to shift by)
            - DR - Destination register
    6. **SHR**
        - **Description**: Shifts the value of SR1 right by the value of SR2 bits and stores the result in DR
        - **Parameters**:
            - SR1 - Source register 1
            - SR2 - Source register 2 (number of bits to shift by)
            - DR - Destination register
    7. **AND**
        - **Description**: Performs a bitwise AND operation between the values of SR1 and SR2 and stores the result in DR
        - **Parameters**:
            - SR1 - Source register 1
            - SR2 - Source register 2
            - DR - Destination register
    8. **OR**
        - **Description**: Performs a bitwise OR operation between the values of SR1 and SR2 and stores the result in DR
        - **Parameters**:
            - SR1 - Source register 1
            - SR2 - Source register 2
            - DR - Destination register
    9. **XOR**
        - **Description**: Performs a bitwise XOR operation between the values of SR1 and SR2 and stores the result in DR
        - **Parameters**:
            - SR1 - Source register 1
            - SR2 - Source register 2
            - DR - Destination register

### Instruction Set (Multiply extension)
- **CLASS_ALU**
    1. **MUL** (3)
        - **Description**: Multiplies the value of SR1 by the value of SR2 and stores the result in DR
        - **Parameters**:
            - SR1 - Source register 1
            - SR2 - Source register 2
            - DR - Destination register
    2. **DIV** (4)
        - **Description**: Divides the value of SR1 by the value of SR2 and stores the result in DR
        - **Parameters**:
            - SR1 - Source register 1
            - SR2 - Source register 2
            - DR - Destination register