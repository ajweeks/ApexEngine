
#include "Apex\WindowListener.h"
#include "Apex\ApexMain.h"

namespace apex
{
	WindowListener::WindowListener()
	{
		APEX->AddWindowListener(this);
	}

	WindowListener::~WindowListener()
	{
		APEX->RemoveWindowListener(this);
	}
} // namespace apex