#include "../Headers/computer.h"

int main(int argc, char * argv[])
{
	if (argc == 1)
	{
		//The program is first argument
		std::cout << "CHIP8 must be run with a game as a command line argument. Exiting.";
	}
	//If numArgs is 2, then we only specify the ROM location.
	else if (argc == 2)
	{
		//Instantiate our ChipEight object on the stack
		Computer interpreter = Computer();
		interpreter.ConstructWindow();
		interpreter.ReadRom(argv[1]);
		interpreter.GameLoop();
	}

    return 0;
}