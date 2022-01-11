#include <windows.h>
#include "WMVC.h"
#include "CCharTable.h"

BOOL WINAPI DllMain(HMODULE hDllHandle, DWORD dwReason, LPVOID lpreserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        if (WMVC::WaitForDecrypt())
        {
            if (WMVC::MakeResourcePath(hDllHandle))
            {
                CCharTable::ReadTable();
                WMVC::PatchGame();
            }
        }
        else
        {
            MessageBoxW(nullptr, L"你正在使用的游戏版本不被支持！请确保你的游戏主程序版本为V1.0或V1.1", WMVERSIONWSTRING, MB_ICONWARNING);
        }
    }

    return TRUE;
}
