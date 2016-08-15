
#include "ApexWindowListener.h"
#include "ApexMain.h"

ApexWindowListener::ApexWindowListener()
{
	APEX->AddWindowListener(this);
}

ApexWindowListener::~ApexWindowListener()
{
	APEX->RemoveWindowListener(this);
}
