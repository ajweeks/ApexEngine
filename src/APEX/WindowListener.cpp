
#include <APEX\WindowListener.h>
#include <APEX\ApexMain.h>

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