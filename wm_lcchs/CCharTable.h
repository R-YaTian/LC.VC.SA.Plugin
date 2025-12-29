#pragma once
#include "CFont.h"

#define OUT_DAT_FILE
#pragma pack(push, 1)
struct CharPos
{
	unsigned __int8 rowIndex;
	unsigned __int8 columnIndex;
};
#pragma pack(pop)
class CCharTable
{
public:
	static char datPath[];

	static CharPos GetCharPos(unsigned __int16 chr);

	static void InitTable();
	static void ReadTable();
	CCharTable();

private:
	//static CharPos m_Table[];
};
