#pragma once
#include "stdinc.h"

#define WMVERSIONWSTRING L"《侠盗猎车手:罪恶都市》汉化补丁 Koishi(1.0) Build220111 by 无名汉化组"

class WMVC
{
public:
    static bool WaitForDecrypt();
    static injector::auto_pointer SelectAddress(unsigned int addr10, unsigned int addr11, unsigned int addrsteam);
    static bool MakeResourcePath(HMODULE module);
    static void PatchGame();
};
