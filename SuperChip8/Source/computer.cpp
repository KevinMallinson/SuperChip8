#include "../Headers/computer.h"

Computer::Computer()
{
	kb = new Keyboard();
	ram = new RAM();
	cpu = new CPU(ram, kb);
}

Computer::~Computer()
{
	delete cpu;
	delete kb;
	delete ram;
}

// Copied from https://github.com/aaruel/CHIP8-SFML/blob/master/chip8.cpp#L49
bool Computer::ReadRom(char * romPath)
{
	FILE * file;
	errno_t err = fopen_s(&file, romPath, "rb");
	
	if (file == NULL)
	{
		return false;
	}
	else
	{
		//std::cout << "Game found." << std::endl;
	}

	fseek(file, 0L, SEEK_END);
	int filesize = ftell(file);
	fseek(file, 0L, SEEK_SET);

	unsigned char * buffer = (unsigned char*)malloc(filesize);
	fread(buffer, 1, filesize, file);

	for (int i = 0; i < filesize; ++i)
	{
		ram->SetMemoryAt(i + 0x200, buffer[i]);
	}

	fclose(file);

	return true;
}

void Computer::GameLoop()
{
	static float emulationSpeed = 1/1500.f;
	sf::Clock emulationClock;

	//The timers count down at 60Hz
	static float timerSpeed = 1.f / 60.f;
	sf::Clock timerClock;

	sf::SoundBuffer buffer;
	buffer.loadFromFile("beep.wav");

	sf::Sound sound;
	sound.setBuffer(buffer);

	//SFML stuff --drawing and getting input--
	while (window->isOpen())
	{
		//Count down the timers:
		if (timerClock.getElapsedTime().asSeconds() >= timerSpeed)
		{
			//The delaytimer counts down to zero at 60hz
			if (cpu->GetDelayTimer() > 0)
			{
				cpu->SetDelayTimer(cpu->GetDelayTimer() - 1);
			}

			//The soundtimer counts down to zero at 60hz
			if (cpu->GetSoundTimer() > 0)
			{
				cpu->SetSoundTimer(cpu->GetSoundTimer() - 1);

				//Beep!
				sound.play();
			}

			timerClock.restart();
		}

		//Compare how long has elapsed, with how long needs to elapse before we can enter
		if (emulationClock.getElapsedTime().asSeconds() >= emulationSpeed)
		{
			sf::Event event;
			while (window->pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window->close();
			}

			window->clear();

			//Get input
			kb->GetInput();

			//Fetch-Decode-Execute
			cpu->ParseSingle();

			//Draw something
			Draw();


			window->display();

			//Restart the clock so we can maintain 500Hz
			emulationClock.restart();
		}
	}
}

void Computer::ConstructWindow()
{
	window = new sf::RenderWindow(sf::VideoMode(1280, 640), "CHIP8");
}

void Computer::Draw()
{
	int sWidth = 1280 / cpu->DISPLAY_WIDTH;
	int sHeight = 640 / cpu->DISPLAY_HEIGHT;

	//Loop through the screen:
	for (int i = 0; i < cpu->DISPLAY_WIDTH; i++)
	{
		for (int j = 0; j < cpu->DISPLAY_HEIGHT; j++)
		{
			if (cpu->display[i][j] == 1)
			{
				//Draw the sprite
				sf::RectangleShape rectangle;
				rectangle.setSize(sf::Vector2f(sWidth, sHeight));
				rectangle.setOutlineColor(sf::Color::White);
				rectangle.setOutlineThickness(1);
				rectangle.setPosition(i * sWidth, j * sHeight);

				window->draw(rectangle);
			}
		}
	}

}
