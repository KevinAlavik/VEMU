#include <stdio.h>
#include <stdint.h>

#define ROM_SIZE 1

int main(int argc, char *argv[])
{
    uint32_t data[ROM_SIZE] = {0x00000008};

    for (int i = 0; i < ROM_SIZE; i++)
    {
        uint8_t *bytePointer = (uint8_t *)&data[i];
        for (int j = 0; j < sizeof(uint32_t); j++)
        {
            putchar(bytePointer[j]);
        }
    }

    return 0;
}
