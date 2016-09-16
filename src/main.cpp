
#include "ApexMain.h"
#include "Windows.h"

// Gotta catch those leaks!
#include <Initguid.h>
#include <dxgidebug.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	typedef HRESULT(__stdcall *fPtr)(const IID&, void**);
	HMODULE hDll = LoadLibraryA("dxgidebug.dll");
	fPtr DXGIGetDebugInterface = (fPtr)GetProcAddress(hDll, "DXGIGetDebugInterface");

	IDXGIDebug* pDXGIDebug;
	DXGIGetDebugInterface(__uuidof(IDXGIDebug), (void**)&pDXGIDebug);
	_CrtSetBreakAlloc(242);
#endif

	APEX->Run();
	delete APEX;

	return 0;
}
