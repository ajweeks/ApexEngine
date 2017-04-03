
#include "Apex\IO\MouseListener.h"
#include "Apex\ApexMain.h"

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
