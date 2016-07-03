
#include "KeyListener.h"
#include "ApexMain.h"

KeyListener::KeyListener()
{
	APEX->AddKeyListener(this);
}

KeyListener::~KeyListener()
{
	APEX->RemoveKeyListener(this);
}
