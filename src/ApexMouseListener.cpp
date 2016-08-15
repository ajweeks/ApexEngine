
#include "ApexMouseListener.h"
#include "ApexMain.h"

ApexMouseListener::ApexMouseListener()
{
	APEX->AddMouseListener(this);
}

ApexMouseListener::~ApexMouseListener()
{
	APEX->RemoveMouseListener(this);
}
