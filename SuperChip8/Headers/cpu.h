#pragma once

#include <stdint.h>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "RAM.h"
#include "keyboard.h"

constexpr auto RANDOM_MAX = 255;
constexpr auto RANDOM_MIN = 0;

//The chip8 supports a 64x32 pixel display - and from what I've read a single byte is used to represent a sprite.
//(A single sprite is 8 pixels wide)

class CPU
{
public:
	CPU(RAM * memory, Keyboard * kb); //Pass ram and keyboard in to CPU as it needs to know it
    ~CPU();

	//Some public methods to start the cycle, and to parse a single opcode.
	inline void ParseSingle() { ParseOpcode(); }
	inline uint8_t GetDelayTimer() { return delayTimer; }
	inline uint8_t GetSoundTimer() { return soundTimer; }

	//Temporary
	inline void SetDelayTimer(uint8_t val) { delayTimer = val; }
	inline void SetSoundTimer(uint8_t val) { soundTimer = val; }

	//TEMP PUBLIC
	//The chip8 supports a 64x32 pixel display - and from what I've read a single byte is used to represent a sprite.
	//(A single sprite is 8 pixels wide)
	uint8_t display[128][64];

	//Flag for high res mode - TEMP PUBLIC
	bool high;

	int DISPLAY_WIDTH;
	int DISPLAY_HEIGHT;



private:

	//CPU must have access to ram and keyboard
	RAM * ram;
	Keyboard * kb;

    //The Chip8 has 16 8-bit registers - thus we can represent it as above, with len 16.
    //The Chip8 registers are V0 to VF - therefore, they can be referred to directly by their array location.
    uint8_t registers[16];

	uint8_t rplFlags[8];

    //The Chip8 has two timers, which count down at 60hz.
    //When non-zero, the soundTimer will make a beep.
    //Both timers can be set to the value of a register, therefore it's width is 8 bits.
    uint8_t delayTimer, soundTimer;

    //The stack stores 16 bit numbers, and has a maximum nesting of 16 - i.e. the stack will overflow past 16.
    uint16_t stack[16];

    //The program counter (PC) should be 16-bit, and is used to store the currently executing address.
    uint16_t programCounter;

    //The stack pointer (SP) can be 8-bit, it is used to point to the topmost level of the stack.
    uint8_t stackPointer;

    //16bit register (For memory address) (Similar to void pointer)
    uint16_t iRegister;


    //Since we only have 35 opcodes, we're going to 'fudge' it by wrapping the parsing in a switch statement.
    //If we were working on e.g. the Intel 8080, this would be pretty messy and we might want to consider something like function pointers instead.
    //We don't pass the opcode to the function, since the opcode will be determined in realtime by reading the memory & program counter.
    void ParseOpcode();
	inline void IncrementProgramCounter() { programCounter += 2; } //Instructions are 16 bits, so += 2. 
	inline void IncrementStackPointer() { stackPointer++; }
	inline void DecrementStackPointer() { stackPointer--; }
	uint16_t Decode();
	void ClearScreen_00E0();
	void Unused_0NNN(); //Only implementing in case we need it eventually
	void Return_00EE();
	void Jump_1NNN(uint16_t opcode);
	void Call_2NNN(uint16_t opcode);
	void SkipIfRegisterEqualsValue_3XNN(uint16_t opcode);
	void SkipIfRegisterNotEqualsValue_4XNN(uint16_t opcode);
	void SkipIfRegisterEqualsRegister_5XY0(uint16_t opcode);
	void SetRegisterToValue_6XNN(uint16_t opcode);
	void AddValueToRegister_7XNN(uint16_t opcode);
	void SetVXToVY_8XY0(uint16_t opcode);
	void SetVXtoVXorVY_8XY1(uint16_t opcode);
	void SetVXtoVXandVY_8XY2(uint16_t opcode);
	void SetVXtoVXxorVY_8XY3(uint16_t opcode);
	void AddVYtoVX_8XY4(uint16_t opcode);
	void SubtractVYfromVX_8XY5(uint16_t opcode);
	void ShiftVYRightByONE_8XY6(uint16_t opcode);
	void SetVXtoVYminusVX_8XY7(uint16_t opcode);
	void ShiftVYleftByOne_8XYE(uint16_t opcode);
	void SkipInstructionifVXnotXY(uint16_t opcode);
	void SetItoNNN(uint16_t opcode);
	void JumptoNNNplusV0(uint16_t opcode);
	void SetVXtoBitwiseandRandomNo(uint16_t opcode);
	void DrawSpriteatVXandVYandN(uint16_t opcode);
	void SkipNextInstructionifKeyinVX(uint16_t opcode);
	void SkipNextInstructionifKeyNotinVX(uint16_t opcode);
	void SetVXtoValueofDelayTimer(uint16_t opcode);
	void AwaitKeyPressthenStoreinVX(uint16_t opcode);
	void SetDelayTimerToVX(uint16_t opcode);
	void SetSoundTimerToVX(uint16_t opcode);
	void AddVXToI(uint16_t opcode);
	void SetIToLocationOfSpriteForCharacterInVX(uint16_t opcode);
	void StoreBCD(uint16_t opcode);
	void StoreV0ToVX(uint16_t opcode);
	void FillV0ToVX(uint16_t opcode);
	
	void ScrollDown(int n);
	void ScrollUp(int n);
	void ScrollLeft();
	void ScrollRight();
	void SetLowResMode();
	void SetHighResMode();
	void StoreRegisterToRPLFlags(uint16_t opcode);
	void ReadRegisterFromRPLFlags(uint16_t opcode);
	void SetIToHDFont(uint16_t opcode);
	void Debugger(uint16_t opcode);
};