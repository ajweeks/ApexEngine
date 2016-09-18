
#include <APEX\IO\KeyListener.h>
#include <APEX\ApexMain.h>

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
