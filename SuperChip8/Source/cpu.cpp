#include "../Headers/cpu.h"
#include "../Headers/ram.h"

CPU::CPU(RAM * memory, Keyboard * keyboard)
{
	DISPLAY_WIDTH = 64;
	DISPLAY_HEIGHT = 32;

	//Init own ram
	ram = memory;
	kb = keyboard;

    //Initialize program counter to 0x200 as per docs
    programCounter = 0x200; //512

    //Clear stack and registers
    for(int i = 0; i < 16; i++)
    {
        stack[i] = 0x00;
        registers[i] = 0x00;
    }

    //Clear display
	for (int i = 0; i < DISPLAY_WIDTH; i++)
	{
		for (int j = 0; j < DISPLAY_HEIGHT; j++)
		{
			display[i][j] = 0;
		}
	}

    //Initialize IRegister
    iRegister = 0;

	//Initialize the stack pointer
	stackPointer = 0;

	//Initialize timers
	delayTimer = 0;
	soundTimer = 0;

	//Some opcodes get a random number, so seed it:
	srand(time(NULL));

	//Set high res to false
	high = false;

	//Set RPL Flags
	for (int i = 0; i < 8; i++)
	{
		rplFlags[i] = 0;
	}
}

CPU::~CPU()
{
}

uint16_t CPU::Decode()
{
	uint16_t opcode = ram->GetMemoryAt(programCounter) << 8 | ram->GetMemoryAt(programCounter + 1);
	return opcode;
}

void CPU::ParseOpcode()
{
	uint16_t opcode = Decode();

    //Some opcodes start with the same first nibble. Therefore, we want to just look at the first nibble.
    switch(opcode & 0xF000)
    {
        case 0x0000:
			switch (opcode & 0x00F0)
			{
				case 0x00B0:
					ScrollUp(opcode & 0x000F);
				break;

				case 0x00C0:
					ScrollDown(opcode & 0x000F);
				break;
			}
            switch(opcode & 0x00FF) 
            {
                case 0x00E0:
                    ClearScreen_00E0();
                break;

                case 0x00EE:
                    Return_00EE();
                break;

				case 0x00FD:
					Unused_0NNN(); 
				break;

				case 0x00FE:
					SetLowResMode();
				break;

				case 0x00FF:
					SetHighResMode();
				break;

				case 0x00FB:
					ScrollRight();
				break;

				case 0x00FC:
					ScrollLeft();
				break;
            }
        break;
            
        case 0x1000:
            Jump_1NNN(opcode);
        break;
            
        case 0x2000:
            Call_2NNN(opcode);
        break;

        case 0x3000:
            SkipIfRegisterEqualsValue_3XNN(opcode);
        break;

        case 0x4000:
            SkipIfRegisterNotEqualsValue_4XNN(opcode);
        break;

        case 0x5000:
            SkipIfRegisterEqualsRegister_5XY0(opcode);
        break;

        case 0x6000:
            SetRegisterToValue_6XNN(opcode);
        break;

        case 0x7000:
            AddValueToRegister_7XNN(opcode);
        break;

        case 0x8000: //8XY0, 8XY1, 8XY2, 8XY3, 8XY4, 8XY5, 8XY6, 8XY7, 8XYE
            switch(opcode & 0x000F)
            {
                case 0x0000:
                    SetVXToVY_8XY0(opcode);
                break;

                case 0x0001:
                    SetVXtoVXorVY_8XY1(opcode);
                break;

                case 0x0002:
                    SetVXtoVXandVY_8XY2(opcode);
                break;

                case 0x0003:
                    SetVXtoVXxorVY_8XY3(opcode);
                break;

                case 0x0004:
                    AddVYtoVX_8XY4(opcode);
                break;

                case 0x0005:
                    SubtractVYfromVX_8XY5(opcode);
                break;
                case 0x0006:
                    ShiftVYRightByONE_8XY6(opcode);
                    break;

                case 0x0007:
                    SetVXtoVYminusVX_8XY7(opcode);
                    break;
                case 0x000E:
                    ShiftVYleftByOne_8XYE(opcode);
                    break;
            }
        break;

        case 0x9000:
            SkipInstructionifVXnotXY(opcode);
            break;

        case 0xA000:
            SetItoNNN(opcode);
            break;

        case 0xB000:
            JumptoNNNplusV0(opcode);
            break;

        case 0xC000:
            SetVXtoBitwiseandRandomNo(opcode);
            break;

        case 0xD000:
            DrawSpriteatVXandVYandN(opcode);
            break;

        case 0xE000:
            switch(opcode & 0x00FF) //EX9E, EXA1
            {
                case 0x009E:
                    SkipNextInstructionifKeyinVX(opcode);
                    break;

                case 0x00A1:
                    SkipNextInstructionifKeyNotinVX(opcode);
                    break;
            }
        break;

        case 0xF000:
            switch(opcode & 0x00FF) //FX07, FX0A, FX15, FX18, FX1E, FX29, FX33, FX55, FX65
            {
                case 0x0007:
                    SetVXtoValueofDelayTimer(opcode);
                    break;

                case 0x000A:
                    AwaitKeyPressthenStoreinVX(opcode);
                    break;

                case 0x0015:
                    SetDelayTimerToVX(opcode);
                    break;

                case 0x0018:
                    SetSoundTimerToVX(opcode);
                    break;

                case 0x001E:
                    AddVXToI(opcode);
                    break;

                case 0x0029:
                    SetIToLocationOfSpriteForCharacterInVX(opcode);
                    break;

				case 0x0030:
					SetIToHDFont(opcode);
					break;

                case 0x0033:
                    StoreBCD(opcode);
                    break;

                case 0x0055:
                    StoreV0ToVX(opcode);
                    break;

                case 0x0065:
                    FillV0ToVX(opcode);
                    break;

				case 0x0075:
					StoreRegisterToRPLFlags(opcode);
					break;

				case 0x0085:
					ReadRegisterFromRPLFlags(opcode);
					break;
            }
    }
}

void CPU::SetLowResMode()
{
	high = false;
	DISPLAY_WIDTH = 64;
	DISPLAY_HEIGHT = 32;
	IncrementProgramCounter();
}

void CPU::SetHighResMode()
{
	high = true;
	DISPLAY_WIDTH = 128;
	DISPLAY_HEIGHT = 64;
	IncrementProgramCounter();
}

void CPU::Unused_0NNN()
{
    IncrementProgramCounter();
}

void CPU::ClearScreen_00E0() //(WORKS - OKAY)
{
	//Clear display
	for (int i = 0; i < DISPLAY_WIDTH; i++)
	{
		for (int j = 0; j < DISPLAY_HEIGHT; j++)
		{
			display[i][j] = 0;
		}
	}
    IncrementProgramCounter();
}

void CPU::Return_00EE() //(WORKS - OKAY)
{
	stackPointer = stackPointer - 1;
	programCounter = stack[stackPointer];
	IncrementProgramCounter(); //THIS WAS MISSING!!! MAJOR BUG!!!
}


void CPU::Jump_1NNN(uint16_t opcode) //(WORKS - OKAY)
{
	uint16_t newAddr = opcode & 0x0FFF;
    programCounter = newAddr;
}

void CPU::Call_2NNN(uint16_t opcode) //(WORKS - OKAY)
{
	stack[stackPointer] = programCounter;
	stackPointer = stackPointer + 1;

	uint16_t newAddr = opcode & 0x0FFF;
	programCounter = newAddr;
}

//3XNN - Skip the next instruction if VX == NN (WORKS - OKAY)
void CPU::SkipIfRegisterEqualsValue_3XNN(uint16_t opcode)
{
	uint8_t& vx = registers[(opcode & 0x0F00) >> 8]; //Get value of X
	uint8_t value = opcode & 0x00FF; //Get value of NN
    
    //Check if the value in register IS EQUAL TO value
    if (vx == value)
    {
        IncrementProgramCounter(); //Then SKIP NEXT INSTRUCTION
    }

    IncrementProgramCounter(); //Increment program counter, as this instruction is done
}

//4XNN - Skip the next instruction if VX != NN (WORKS - OKAY)
void CPU::SkipIfRegisterNotEqualsValue_4XNN(uint16_t opcode)
{
	uint8_t vX = (opcode & 0x0F00) >> 8; //Get value of X
	uint8_t value = opcode & 0x00FF; //Get value of NN

	//Check if the value in register IS NOT EQUAL TO value
	if (registers[vX] != value)
	{
		IncrementProgramCounter(); //Then SKIP NEXT INSTRUCTION
	}

	IncrementProgramCounter(); //Increment program counter, as this instruction is done
}

//5XY0 - Skip the next instruction if VX == VY. (WORKS - OKAY)
void CPU::SkipIfRegisterEqualsRegister_5XY0(uint16_t opcode)
{
    uint8_t vX = registers[(opcode & 0x0F00) >> 8]; //Get value of X: Right shift to remove eight zeroes
	uint8_t vY = registers[(opcode & 0x00F0) >> 4]; //Get value of Y: Right shift to remove four zeroes

    //Check if the value in register IS EQUAL TO value from other register
    if (vX == vY)
    {
        IncrementProgramCounter(); //Then SKIP NEXT INSTRUCTION
    }

    IncrementProgramCounter(); //Increment program counter, as this instruction is done
}

//6XNN - Set VX to NN. (WORKS - OKAY)
void CPU::SetRegisterToValue_6XNN(uint16_t opcode)
{
    uint8_t index = (opcode & 0x0F00) >> 8; //Get value of X: Right shift to remove eight zeroes
	uint8_t value = opcode & 0x00FF; //Get value of NN
    registers[index] = value; //Set register to value
    IncrementProgramCounter();//Increment program counter, as this instruction is done
}

//7XNN - Add NN to VX. Don't change carry flag. (WORKS - OKAY)
void CPU::AddValueToRegister_7XNN(uint16_t opcode)
{
	uint8_t vX = (opcode & 0x0F00) >> 8; //Get value of X: Right shift to remove eight zeroes
	uint8_t value = opcode & 0x00FF; //Get value of NN
    registers[vX] += value; //Add value to register
    IncrementProgramCounter(); //Increment program counter, as this instruction is done
}

//8XY0 - Sets VX to the value of VY. (NOT TESTED)
void CPU::SetVXToVY_8XY0(uint16_t opcode)
{
	uint8_t & vx = registers[(0x0F00 & opcode) >> 8]; //reference for easier use
	uint8_t & vy = registers[(0x00F0 & opcode) >> 4]; //reference for easier use

	vx = vy;

    IncrementProgramCounter();
}

//8XY1 - Sets VX to VX or VY. (Bitwise OR operation) (NOT TESTED)
void CPU::SetVXtoVXorVY_8XY1(uint16_t opcode)
{
	uint8_t & vx = registers[(0x0F00 & opcode) >> 8]; //reference for easier use
	uint8_t & vy = registers[(0x00F0 & opcode) >> 4]; //reference for easier use

	vx = vx | vy;

    IncrementProgramCounter();
}

//8XY2 - Sets VX to VX and VY. (Bitwise AND operation) (NOT TESTED)
void CPU::SetVXtoVXandVY_8XY2(uint16_t opcode) 
{    
	uint8_t & vx = registers[(0x0F00 & opcode) >> 8]; //reference for easier use
	uint8_t & vy = registers[(0x00F0 & opcode) >> 4]; //reference for easier use
		
	vx = vx & vy;

    IncrementProgramCounter();
}

//8XY3 - Sets VX to VX xor VY. (NOT TESTED)
void CPU::SetVXtoVXxorVY_8XY3(uint16_t opcode)
{
	uint8_t & vx = registers[(0x0F00 & opcode) >> 8]; //reference for easier use
	uint8_t & vy = registers[(0x00F0 & opcode) >> 4]; //reference for easier use

	vx = vx ^ vy;

    IncrementProgramCounter();
}

//8XY4 - Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't. (SHOULD BE WORKING)
void CPU::AddVYtoVX_8XY4(uint16_t opcode)
{
	uint8_t & vx = registers[(0x0F00 & opcode) >> 8]; //reference for easier use
	uint8_t & vy = registers[(0x00F0 & opcode) >> 4]; //reference for easier use

	//Check if vX + vY > 255
	if (vx + vy > 255)
	{
		//It's greater than, so carry
		registers[0xF] = 1;
	}
	else
	{
		//It's less than or equal, so no carry
		registers[0xF] = 0;
	}

	//Do the actual operation
	vx = vx + vy;

	IncrementProgramCounter();
}

//8XY5 - VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't. (WORKING - OKAY)
void CPU::SubtractVYfromVX_8XY5(uint16_t opcode)
{
	uint8_t & vx = registers[(0x0F00 & opcode) >> 8]; //reference for easier use
	uint8_t & vy = registers[(0x00F0 & opcode) >> 4]; //reference for easier use

	//Check if vX - vY < 0
	if (vx - vy < 0)
	{
		//It's less than, so there's a borrow
		registers[0xF] = 0;
	}
	else
	{
		//It's not less than, so no borrow
		registers[0xF] = 1;
	}

	//Do the actual operation
	vx = vx - vy;

    IncrementProgramCounter();
}

//opcode 8XY7 - Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't. (WORKING - OKAY)
void CPU::SetVXtoVYminusVX_8XY7(uint16_t opcode )
{
	uint8_t & vx = registers[(0x0F00 & opcode) >> 8]; //reference for easier use
	uint8_t & vy = registers[(0x00F0 & opcode) >> 4]; //reference for easier use

	//Test the subtraction
	if (vy - vx < 0)
	{
		registers[0xF] = 0;
	}
	else
	{
		registers[0xF] = 1;
	}

	//Perform the actual operation
	vx = vy - vx;

	//Advance the program counter
    IncrementProgramCounter();
}

//8XYE - If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2 (Left Shift by 1)
void CPU::ShiftVYleftByOne_8XYE(uint16_t opcode)
{
	uint8_t & vx = registers[(0x0F00 & opcode) >> 8]; //reference for easier use
	registers[0xF] = vx >> 7; //Get MSB
	vx = vx << 1;
    IncrementProgramCounter();
}

//8XY6 - If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2. (Shift Right by 1)
void CPU::ShiftVYRightByONE_8XY6(uint16_t opcode)
{
	uint8_t & vx = registers[(0x0F00 & opcode) >> 8]; //reference for easier use
	registers[0xF] = vx & 1; //Get LSB
	vx = vx >> 1;
	IncrementProgramCounter();
}

//9XY0 - Skips the next instruction if VX doesn't equal VY. (Usually the next instruction is a jump to skip a code block)                
void CPU::SkipInstructionifVXnotXY(uint16_t opcode)
{
	uint8_t & vx = registers[(opcode & 0x0F00) >> 8];
	uint8_t & vy = registers[(opcode & 0x00F0) >> 4];

	if (vx != vy)
	{
		IncrementProgramCounter();
	}

    IncrementProgramCounter();  
}
//ANNN - Sets I to the address NNN (WORKING - OKAY)
void CPU::SetItoNNN(uint16_t opcode)
{
	uint16_t address = opcode & 0x0FFF;
	iRegister = address;
    IncrementProgramCounter();
}

//BNNN - Jumps to the address NNN plus V0
void CPU::JumptoNNNplusV0(uint16_t opcode){
    programCounter = registers[(opcode & 0x0FFF)] + registers[(0x0000)];
    }

//CXNN - Sets VX to the result of a bitwise AND operation on a random number (Typically: 0 to 255) and NN (WORKING - OKAY)
void CPU::SetVXtoBitwiseandRandomNo(uint16_t opcode)
{
	uint8_t random = rand() % RANDOM_MAX;
	uint8_t val = opcode & 0x00FF;
	uint8_t index = (opcode & 0x0F00) >> 8;
	
	registers[index] = random & val;

    IncrementProgramCounter();
}

//DXYN - Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. 
//Each row of 8 pixels is read as bit-coded starting from memory location I; I value doesn’t change after the execution of this instruction.
// As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that doesn’t happen
void CPU::DrawSpriteatVXandVYandN(uint16_t opcode)
{
    //Get the x, y coordinates
    uint8_t x = registers[(opcode & 0x0F00) >> 8];
	uint8_t y = registers[(opcode & 0x00F0) >> 4];

	//Without this the UFO doesn't wrap around ONLY when I use a 2D array
	if (x >= DISPLAY_WIDTH) x %= DISPLAY_WIDTH;
	if (y >= DISPLAY_HEIGHT) y %= DISPLAY_HEIGHT;

	uint8_t height = opcode & 0x000F;

	if (height != 0) //Height is 1 to 15
	{
		//We don't know if we've flipped any bits yet, so set to 0 for now.
		registers[0xF] = 0;

		//Loop for each row that encompasses the sprite
		for (int sprite_rows = 0; sprite_rows < height; sprite_rows++)
		{
			//The pixel is just one ROW of the SPRITE which is N rows
			int pixel = ram->GetMemoryAt(iRegister + sprite_rows); //Get each row of the sprite by incrementing I

			for (int sprite_bits = 0; sprite_bits < 8; sprite_bits++) //Each sprite is 8 units wide because of 8 bits
			{
				//Get the bit we want to check if it's set:
				int bit = pixel & (0x80 >> sprite_bits);
				int col = x + sprite_bits;
				int row = y + sprite_rows;

				//Check if the bit is set
				if (bit != 0)
				{
					//We want to draw, but we must check if the bit is already set here:
					if (display[col][row] == 1)
					{
						//It is set, so we need to set the 0xF register because we will erase.
						registers[0xF] = 1;
					}

					//XOR the position with 1, which will flip the bit.
					display[col][row] ^= 1;
				}
			}
		}
	}
	else //Height is 0 so draw 16x16
	{
		//Set height to 16
		height = 16;

		registers[0xF] = 0;
		for (int sprite_rows = 0; sprite_rows < height; sprite_rows++)
		{
			int pixel = (ram->GetMemoryAt(iRegister + sprite_rows * 2) << 8) | (ram->GetMemoryAt(iRegister + 1 + sprite_rows * 2));
			for (int sprite_bits = 0; sprite_bits < 16; sprite_bits++) 
			{
				int bit = pixel & (0x8000 >> sprite_bits);
				int col = x + sprite_bits;
				int row = y + sprite_rows;
				if (bit != 0)
				{
					if (display[col][row] == 1)
					{
						registers[0xF] = 1;
					}
					display[col][row] ^= 1;
				}
			}
		}
	}
    IncrementProgramCounter();
}

 //EX9E - Skips the next instruction if the key stored in VX is pressed. 
 // (Usually the next instruction is a jump to skip a code block)
void CPU::SkipNextInstructionifKeyinVX(uint16_t opcode)
{
	//A key is stored in vX.
	uint8_t vX = registers[(opcode & 0x0F00) >> 8];

    if (kb->KeyAt(vX) == true)
	{
		IncrementProgramCounter();
    }

	IncrementProgramCounter();
}
//EXA1 - Skips the next instruction if the key stored in VX isn't pressed. 
//(Usually the next instruction is a jump to skip a code block)
void CPU::SkipNextInstructionifKeyNotinVX(uint16_t opcode){
	//A key is stored in vX.
	uint8_t vX = registers[(opcode & 0x0F00) >> 8];

	if (kb->KeyAt(vX) == false)
	{
		IncrementProgramCounter();
	}

	IncrementProgramCounter();
}


//FX07 - Sets VX to the value of the delay timer
void CPU::SetVXtoValueofDelayTimer(uint16_t opcode){
    registers[(opcode & 0x0F00) >> 8] = delayTimer;
    IncrementProgramCounter();
}

//FX0A - A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)
void CPU::AwaitKeyPressthenStoreinVX(uint16_t opcode){
    bool key_pressed = false;

    for (int i = 0; i < 16; i++){
        if (kb->KeyAt(i) == true) {
            registers[(opcode & 0x0F00) >> 8] = i;
            key_pressed = true;
			break;
        }
    }

    if(!key_pressed){
        return;
    }
    IncrementProgramCounter();
}

//FX15 - Sets the delay timer to VX
void CPU::SetDelayTimerToVX(uint16_t opcode)
{
    //Get the value of VX
    uint8_t vX = registers[(opcode & 0x0F00) >> 8];

    //Set the delay timer to the value
    delayTimer = vX;
    IncrementProgramCounter();
}

//FX18 - Sets the sound timer to VX
void CPU::SetSoundTimerToVX(uint16_t opcode)
{
    //Get the value of VX
    uint8_t vX = registers[(opcode & 0x0F00) >> 8];

    //Set the delay timer to the value
    soundTimer = vX;
    IncrementProgramCounter();
}

//FX1E - Adds VX to I
void CPU::AddVXToI(uint16_t opcode)
{
	//Get the value of VX
	uint8_t vX = registers[(opcode & 0x0F00) >> 8];

	//Check if we will get a buffer overflow (> 255)
	if (vX + iRegister > 255)
	{
		//It is greater, so set VF
		registers[0xF] = 1;
	}
	else
	{
		//It is less or equal, so unset VF
		registers[0xF] = 0;
	}

    //Add it to I
    iRegister += vX;
    IncrementProgramCounter();
}

//FX29 - Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font (WORKING - MAYBE)
void CPU::SetIToLocationOfSpriteForCharacterInVX(uint16_t opcode)
{
	//Get the value of VX
	uint8_t value = registers[(opcode & 0x0F00) >> 8];

    //Set I to location of sprite
	iRegister = value * 5;

    IncrementProgramCounter();
}

//FX33 - Stores the binary-coded decimal representation of VX at I, I+1, I+2. (WORKING - OKAY)
void CPU::StoreBCD(uint16_t opcode)
{
	//Get the value of VX
	uint8_t vX = registers[(opcode & 0x0F00) >> 8];
    ram->SetMemoryAt(iRegister, vX / 100);			//E.g. 256 / 100 = 2.56 = 2.
    ram->SetMemoryAt(iRegister+1, (vX / 10) % 10);	//E.g. 256 / 10 = 25.6, so 25. then 25 / 10 = 2 r(5).
    ram->SetMemoryAt(iRegister+2, vX % 10);			//E.g. 256 / 10 = 25 r(6).

    IncrementProgramCounter();
}

//FX55 - Stores V0 to VX (including VX) in memory starting at address I. I is increased by 1 for each value written (WORKING - OKAY)
void CPU::StoreV0ToVX(uint16_t opcode)
{ 
	//Get the value of x (not vx, a variable x!)
	uint8_t x = (opcode & 0x0F00) >> 8;
    
    uint16_t IValue = iRegister;

    //Loop through all registers up to VX
    for (uint8_t i = 0; i <= x; i++)
    {
        //Store its value in memory at I
        ram->SetMemoryAt(IValue, registers[i]);

        //Increment I's value
        IValue += 1;
    }

	//Set I to I + x + 1
	//iRegister = iRegister + x + 1;

    IncrementProgramCounter();
}

//FX65 - Fills V0 to VX (including VX) with values from memory starting at address I. I is increased by 1 for each value written (WORKING - OKAY)
void CPU::FillV0ToVX(uint16_t opcode)
{
	//Get the value of x (not vx, a variable x!)
	uint8_t x = (opcode & 0x0F00) >> 8;
    
    uint16_t IValue = iRegister;

    //Loop through all registers up to VX
    for (uint8_t i = 0; i <= x; i++)
    {
		registers[i] = ram->GetMemoryAt(IValue);
		IValue++;
    }

	//Set I to I + x + 1
	//iRegister = iRegister + x + 1;

    IncrementProgramCounter();

}


void CPU::ScrollLeft()
{
	//Loop for each X and Y
	for (int x = 0; x < DISPLAY_WIDTH; x++)
	{
		for (int y = 0; y < DISPLAY_HEIGHT; y++)
		{
			//Check for out of bounds (over)
			if (x + 4 > DISPLAY_HEIGHT)
			{
				display[x][y] = 0;
			}
			else
			{
				display[x][y] = display[x + 4][y];

			}
		}
	}

	IncrementProgramCounter();
}

void CPU::ScrollRight()
{
	//Loop for each X and Y
	for (int x = DISPLAY_WIDTH - 1; x >= 0; x--)
	{
		for (int y = DISPLAY_HEIGHT - 1; y >= 0; y--)
		{
			//Check for out of bounds (under)
			if (x - 4 < 0) //because we are scrolling by 4 pixels
			{
				display[x][y] = 0;
			}
			else
			{
				display[x][y] = display[x - 4][y];

			}
		}
	}

	IncrementProgramCounter();
}

void CPU::ScrollDown(int n)
{
	//Loop for each X and Y
	for (int x = DISPLAY_WIDTH - 1; x >= 0; x--)
	{
		for (int y = DISPLAY_HEIGHT - 1; y >= 0; y--)
		{
			//Check for out of bounds (under)
			if (y < n)
			{
				display[x][y] = 0;
			}
			else
			{
				display[x][y] = display[x][y - n];

			}
		}
	}

	IncrementProgramCounter();
}

void CPU::ScrollUp(int n)
{
	//Loop for each X and Y
	for (int x = 0; x < DISPLAY_WIDTH; x++)
	{
		for (int y = 0; y < DISPLAY_HEIGHT; y++)
		{
			//Check for out of bounds (over)
			if (y + n > DISPLAY_HEIGHT)
			{
				display[x][y] = 0;
			}
			else
			{
				display[x][y] = display[x][y + n];

			}
		}
	}

	IncrementProgramCounter();
}

//FX75
void CPU::StoreRegisterToRPLFlags(uint16_t opcode)
{
	//Get X
	uint8_t x = (opcode & 0x0F00) >> 8;

	if (x <= 7)
	{
		for (int i = 0; i <= x; i++)
		{
			rplFlags[i] = registers[i];
		}
	}

	IncrementProgramCounter();
}

//FX85
void CPU::ReadRegisterFromRPLFlags(uint16_t opcode)
{
	//Get X
	uint8_t x = (opcode & 0x0F00) >> 8;

	if (x <= 7)
	{
		for (int i = 0; i <= x; i++)
		{
			registers[i] = rplFlags[i];
		}
	}

	IncrementProgramCounter();
}

//FX30
void CPU::SetIToHDFont(uint16_t opcode)
{
	//Get the value of VX
	uint8_t value = registers[(opcode & 0x0F00) >> 8];

	//Set I to location of sprite
	iRegister = 0x100 + value * 10;

	IncrementProgramCounter();
}

void CPU::Debugger(uint16_t opcode)
{
	std::cout << "PC: " << programCounter;
	std::cout << "	";
	std::cout << std::setfill('0') << std::setw(4) << std::hex << opcode << std::endl;
}