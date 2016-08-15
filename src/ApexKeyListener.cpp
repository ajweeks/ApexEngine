
#include "ApexKeyListener.h"
#include "ApexMain.h"

ApexKeyListener::ApexKeyListener()
{
	APEX->AddKeyListener(this);
}

ApexKeyListener::~ApexKeyListener()
{
	APEX->RemoveKeyListener(this);
}
