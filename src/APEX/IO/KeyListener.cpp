
#include "Apex\IO\KeyListener.h"
#include "Apex\ApexMain.h"

namespace apex
{
	KeyListener::KeyListener(int priority)
	{
		APEX->AddKeyListener(this, priority);
	}

	KeyListener::~KeyListener()
	{
		APEX->RemoveKeyListener(this);
	}
} // namspace apex
