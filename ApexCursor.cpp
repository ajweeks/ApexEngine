#include "ApexCursor.h"

#ifdef SFML_SYSTEM_WINDOWS

sf::ApexCursor::ApexCursor(const sf::ApexCursor::TYPE t)
{
	switch (t)
	{
	case sf::ApexCursor::WAIT:
		Cursor = LoadCursor(NULL, IDC_WAIT);
		break;
	case sf::ApexCursor::HAND:
		Cursor = LoadCursor(NULL, IDC_HAND);
		break;
	case sf::ApexCursor::NORMAL:
		Cursor = LoadCursor(NULL, IDC_ARROW);
		break;
	case sf::ApexCursor::TEXT:
		Cursor = LoadCursor(NULL, IDC_IBEAM);
		break;
	case sf::ApexCursor::MOVE:
		Cursor = LoadCursor(NULL, IDC_SIZEALL);
		//For more cursor options on Windows go here:
		// http://msdn.microsoft.com/en-us/library/ms648391%28v=vs.85%29.aspx
	}
}

void sf::ApexCursor::set(const sf::WindowHandle& aWindowHandle) const
{
	SetClassLongPtr(aWindowHandle, GCLP_HCURSOR, reinterpret_cast<LONG_PTR>(Cursor));
}

sf::ApexCursor::~ApexCursor()
{
	// Nothing to do for destructor :
	// no memory has been allocated (shared ressource principle)
}

#elif defined(SFML_SYSTEM_LINUX)

sf::StandardCursor::StandardCursor(const sf::StandardCursor::TYPE t)
{
	display = XOpenDisplay(NULL);

	switch (t)
	{
	case sf::StandardCursor::WAIT:
		Cursor = XCreateFontCursor(display, XC_watch);
		break;
	case sf::StandardCursor::HAND:
		Cursor = XCreateFontCursor(display, XC_hand1);
		break;
	case sf::StandardCursor::NORMAL:
		Cursor = XCreateFontCursor(display, XC_left_ptr);
		break;
	case sf::StandardCursor::TEXT:
		Cursor = XCreateFontCursor(display, XC_xterm);
		break;
	case sf::StandardCursor::MOVE:
		Cursor = XCreateFontCursor(display, XC_tcross);
		break;
		// For more cursor options on Linux go here:
		// http://www.tronche.com/gui/x/xlib/appendix/b/
	}
}

void sf::StandardCursor::set(const sf::WindowHandle& aWindowHandle) const
{
	XDefineCursor(display, aWindowHandle, Cursor);
	XFlush(display);
}

sf::StandardCursor::~StandardCursor()
{
	XFreeCursor(display, Cursor);
	delete display;
	display = NULL;
}

#else
#error This OS is not yet supported by the cursor library.
#endif