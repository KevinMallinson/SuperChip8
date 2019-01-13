#pragma once

#include <stdint.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "cpu.h"
#include "keyboard.h"

class Computer
{
public:
	Computer();
	~Computer();
	bool ReadRom(char * romPath);
	void GameLoop();
	void ConstructWindow();

private:
	sf::RenderWindow * window;
	void Draw();

	//Computer has a CPU
	CPU * cpu;

	//Computer has RAM
	RAM * ram;

	//Computer has a keyboard
	Keyboard * kb;


};
