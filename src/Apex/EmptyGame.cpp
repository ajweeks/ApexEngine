
#include "Apex\EmptyGame.h"

#include "Apex\ApexMain.h"

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
	static float x = 0;
	static float y = 0;
	const float width = 100;
	const float height = 35;
	const float xv = 3;
	const float yv = 5;

	sf::RectangleShape rect;
	rect.setSize(sf::Vector2f(width, height));
	rect.setPosition(x, y);
	rect.setFillColor(sf::Color::White);
	target.draw(rect);

	x += xv;
	y += yv;

	if (x > target.getSize().x - width)
	{
		x = 0;
	}
	if (y > target.getSize().y - height)
	{
		y = 0;
	}
}
