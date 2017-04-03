
#include "Game\Game.h"

#include "Apex\ApexMain.h"

#include "Windows.h"
#include <memory>

// Gotta catch those leaks!
#include <dxgidebug.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// TODO: Fix fucking bs memory leaks
	//_CrtSetBreakAlloc(79762);
#endif

	Game* game = new Game();
	
	APEX->Run(game);
	
	delete game;
	apex::ApexMain::DestroySingleton();

	return 0;
}

int main() // If the linker's subsystem is set to Console, this will be the application's entry point
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	return WinMain(hInstance, 0, nullptr, SW_SHOW);
}
