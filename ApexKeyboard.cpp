#include "ApexKeyboard.h"

bool ApexKeyboard::keysdown[sf::Keyboard::KeyCount];
bool ApexKeyboard::keysdownLastFrame[sf::Keyboard::KeyCount];

ApexKeyboard::ApexKeyboard()
{
}

ApexKeyboard::~ApexKeyboard()
{
}

void ApexKeyboard::Tick()
{
	for (size_t i = 0; i < sf::Keyboard::KeyCount; ++i)
	{
		keysdownLastFrame[i] = keysdown[i];
		keysdown[i] = sf::Keyboard::isKeyPressed((sf::Keyboard::Key)i);
	}
}

bool ApexKeyboard::IsKeyDown(const sf::Keyboard::Key& key)
{
	return keysdown[key];
}

bool ApexKeyboard::IsKeyPressed(const sf::Keyboard::Key& key)
{
	return keysdown[key] && keysdownLastFrame[key] == false;
}

bool ApexKeyboard::IsKeyReleased(const sf::Keyboard::Key& key)
{
	return keysdown[key] == false && keysdownLastFrame[key];
}
