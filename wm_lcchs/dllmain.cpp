#include <windows.h>
#include "WMLC.h"
#include <cstdio>

BOOL WINAPI DllMain(HMODULE hDllHandle, DWORD dwReason, LPVOID lpreserved)
{

	if (dwReason == DLL_PROCESS_ATTACH)
	{
#ifdef _DEBUG
        MessageBoxA(NULL, "准备调试", "提示", MB_OK);

        //创建调试窗口
        AllocConsole();
        //freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w+t", stdout);
        freopen_s(reinterpret_cast<FILE**>(stdout), "debug.txt", "w+t", stdout);
        //freopen_s(reinterpret_cast<FILE**>(stdout), "out.txt", "w+t", stdout);
        //FILE* f2=freopen("CONIN$", "r+t", stdin);
        SetConsoleTitleA("调试窗口");
#endif // DEBUG

		return (WMLC::CheckResourceFile(hDllHandle) && WMLC::CheckGameVersion());
	}

	return TRUE;
}
