
#include <APEX\IO\MouseListener.h>
#include <APEX\ApexMain.h>

namespace apex
{
	MouseListener::MouseListener()
	{
		APEX->AddMouseListener(this);
	}

	MouseListener::~MouseListener()
	{
		APEX->RemoveMouseListener(this);
	}
} // namespace apex
