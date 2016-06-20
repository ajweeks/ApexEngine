
#include "KeyListener.h"
#include "Game.h"

KeyListener::KeyListener(Game* game) :
	m_Game(game)
{
	m_Game->AddKeyListener(this);
}

KeyListener::~KeyListener()
{
	m_Game->RemoveKeyListener(this);
}
