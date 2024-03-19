#ifndef __VISC_H__
#define __VISC_H__

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "bus.h"
#include "rom.h"

#define PLANE_SIZE 16

// First Register Plane
#define A 0
#define B 1
#define C 2
#define D 3
#define E 4
#define F 5
#define G 6
#define H 7
#define A1 8
#define A2 9
#define A3 10
#define A4 11
#define A5 12
#define A6 13
#define A7 14
#define A8 15

// Second Register Plane
#define A_SHADOW 0
#define B_SHADOW 1
#define C_SHADOW 2
#define D_SHADOW 3
#define E_SHADOW 4
#define F_SHADOW 5
#define G_SHADOW 6
#define H_SHADOW 7
#define PC 8
#define SP 9
#define BP 10
#define RESERVED_1 11
#define RESERVER_2 12
#define STATUS 13
#define HARTID 14
#define FLAGS 15

// Classes
//      2 -> 4 are un-impelemented
#define DATA_CLASS 0
#define ALU_CLASS 1
#define JUMP_CLASS 2
#define ALGORITHM_CLASS 3
#define FLOAT_CLASS 4

// DATA Instructions
#define LD 0
#define ST 1

typedef struct
{
    uint8_t class : 4;
    uint8_t opcode;
    uint8_t sr1 : 4;
    uint8_t sr2 : 4;
    uint8_t dr : 4;
    uint64_t data : 44;
} Instruction;

typedef struct
{
    uint32_t low_plane[PLANE_SIZE];
    uint32_t high_plane[PLANE_SIZE];
    uint8_t planeNum : 1;
    uint32_t *curPlane;
} VISC_I;

VISC_I *init_visc();
void run_visc(VISC_I *visc);
void switch_plane(VISC_I *cpu, int num);

#endif // __VISC_H__
