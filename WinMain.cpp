#include "StdAfx.h"
#include "MainWindow.h"

CAppModule _Module;

int WINAPI _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nShowCmd)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nShowCmd);

	int result;


	CoInitialize(NULL);
	{
		CMessageLoop message_loop;
		CMainWindow main;


		_Module.Init(NULL, hInstance);
		_Module.AddMessageLoop(&message_loop);
		main.Create(NULL, CWindow::rcDefault, TEXT(""), WS_POPUP);

		result = message_loop.Run();

		_Module.RemoveMessageLoop();
		_Module.Term();
	}

	CoUninitialize();

	return result;
}
