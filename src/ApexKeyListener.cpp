
#include "ApexKeyListener.h"
#include "ApexMain.h"

ApexKeyListener::ApexKeyListener(int priority)
{
	APEX->AddKeyListener(this, priority);
}

ApexKeyListener::~ApexKeyListener()
{
	APEX->RemoveKeyListener(this);
}
