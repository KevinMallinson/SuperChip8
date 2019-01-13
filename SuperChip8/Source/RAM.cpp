#include "../Headers/RAM.h"

RAM::RAM()
{
	//initialize the memory with all zeroes
	for (int i = 0; i < 4096; i++)
	{
		memory[i] = 0;
	}

	//Load fontset - there are 80 items in the array, stored in memory address 0x00 to 0x50 (80)
	for (int i = 0; i < 80; i++)
	{
		memory[i] = chip8_fontset[i];
	}

	//Load the HD fontset from 0x100
	for (int i = 0; i < 160; i++)
	{
		memory[0x100 + i] = chip8_hd_fontset[i];
	}
}

uint8_t RAM::GetMemoryAt(int location)
{
	return memory[location];
}

void RAM::SetMemoryAt(int location, uint8_t value)
{
	memory[location] = value;
}