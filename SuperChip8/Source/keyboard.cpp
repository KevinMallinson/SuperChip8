#include "../Headers/keyboard.h"

Keyboard::Keyboard()
{
	//Instantiate keyboard to 0, i.e. all bits set to 0
	keyboard = 0;
}

void Keyboard::GetInput()
{
	//Start by setting each key to 0
	keyboard = 0;

	//Originally was else if, but changed to if to allow for multiple key presses
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
	{
		//1st bit set
		keyboard = keyboard | 0b0000000000000001;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
	{
		//2nd bit set
		keyboard = keyboard | 0b0000000000000010;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
	{
		//3rd bit set
		keyboard = keyboard | 0b0000000000000100;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
	{
		//4th bit set
		keyboard = keyboard | 0b0000000000001000;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
	{
		//5th bit set
		keyboard = keyboard | 0b0000000000010000;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		//6th bit set
		keyboard = keyboard | 0b0000000000100000;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
	{
		//7th bit set
		keyboard = keyboard | 0b0000000001000000;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
	{
		//8th bit set
		keyboard = keyboard | 0b0000000010000000;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		//9th bit set
		keyboard = keyboard | 0b0000000100000000;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		//10th bit set
		keyboard = keyboard | 0b0000001000000000;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		//11th bit set
		keyboard = keyboard | 0b0000010000000000;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
	{
		//12th bit set
		keyboard = keyboard | 0b0000100000000000;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
	{
		//13th bit set
		keyboard = keyboard | 0b0001000000000000;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
	{
		//14th bit set
		keyboard = keyboard | 0b0010000000000000;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
	{
		//15th bit set
		keyboard = keyboard | 0b0100000000000000;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::V))
	{
		//16th bit set
		keyboard = keyboard | 0b1000000000000000;
	}
}

bool Keyboard::KeyAt(int index)
{
	//Create a mask
	unsigned short mask = 1;
	mask = mask << index;

	//Now the 1 is in the correct spot, so AND with the keyboard
	unsigned short key = keyboard & mask;

	//Right shift it back again index times
	key = key >> index;

	//If it's 1, it's pressed, else it's not pressed
	if (key == 1)
	{
		return true;
	}

	return false;
}
