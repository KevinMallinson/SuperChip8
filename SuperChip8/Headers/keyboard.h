#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>

class Keyboard
{
public:
	Keyboard();
	void GetInput();
	bool KeyAt(int index);

private:
	unsigned short keyboard;
};