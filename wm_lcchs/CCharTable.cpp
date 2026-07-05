#include "CCharTable.h"
#include <array>
#include <limits>

const int CHARS_SIZE = 0x10000; //std::numeric_limits<unsigned __int16>::max(); //0x10000

//CharPos CCharTable::mTable[0x10000];
static std::array<CharPos, CHARS_SIZE> mTable;
char CCharTable::datPath[260];
CharPos CCharTable::GetCharPos(unsigned __int16 chr)
{
	CharPos result;
	
	if (chr < 0x60)
	{

		result.rowIndex = (chr >> 4);
		result.columnIndex = (chr & 0xF);
	}
	else
	{
		result = mTable[chr + 0x20];
	}

	return result;
}
void CCharTable::ReadTable()
{
	

	FILE* hfile = std::fopen(datPath, "rb");

	if (hfile != nullptr)
	{
		mTable.fill({ 63, 63 });

		std::fseek(hfile, 0, SEEK_END);
		int ppos = std::ftell(hfile);
		int datasize = sizeof(CharPos) * mTable.size();
		if (ppos == datasize)//131072 = 65536 * 2
		{
			std::fseek(hfile, 0, SEEK_SET);
			std::fread(mTable.data(), sizeof(CharPos), mTable.size(), hfile);
		}

		std::fclose(hfile);
	}
}

CCharTable::CCharTable()
{
	
}

static CCharTable instance;
