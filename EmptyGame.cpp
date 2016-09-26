
#include "EmptyGame.h"

#include <APEX\ApexMain.h>

EmptyGame::EmptyGame()
{
	APEX->SetWindowTitle("Empty Game");
	
}

EmptyGame::~EmptyGame()
{
}

void EmptyGame::Tick(sf::Time elapsed)
{
}

void EmptyGame::Draw(sf::RenderTarget& target)
{
}
