#include "LCGXT.h"

#include <array>
#include <iostream>
#include <fstream>
#include <regex>
#include <locale>
#include <codecvt>
#include <algorithm>

#include <cstdio>
#include <cstring>
#include "../deps/utf8cpp/utf8.h"

struct CharPos
{
	uint8_t rowIndex;
	uint8_t columnIndex;
};

wchar_t MostUsedCharacters[] = L"";

static std::array<CharPos, 0x10000> sTable;

LCGXT::LCGXT()
{
    m_WideCharCollection.insert(std::begin(MostUsedCharacters), std::end(MostUsedCharacters) - 1);
}

bool LCGXT::LoadText(const char *path)
{
	std::ifstream inputFile;

	std::string lineBuffer;

	std::vector<uint16_t> wideLineBuffer;

	std::regex entryFormat(R"-(([0-9a-zA-Z_]{1,7})=(.+))-");
	std::smatch matchResult;

	m_GxtData.clear();

	inputFile.open(path);

	if (!inputFile.is_open())
	{
		std::cout << "Open input file failed." << std::endl;
		return false;
	}

	while (std::getline(inputFile, lineBuffer))
	{
		if (lineBuffer.empty() || lineBuffer.front() == ';') continue;

		if (std::regex_match(lineBuffer, matchResult, entryFormat))
		{
			UTF8ToUTF16(matchResult.str(2), wideLineBuffer);

			if (matchResult.str(1) == "CHS2500" || matchResult.str(1) == "CHS3000" || m_GxtData.emplace(matchResult.str(1), wideLineBuffer).second)
			{
				for (uint16_t chr : wideLineBuffer)
				{
					if (chr >= 0x80)
					{
						m_WideCharCollection.insert(chr);
					}
				}
			}
		}
		else
		{
			std::cout << "Invalid line:" << std::endl << lineBuffer << std::endl << std::endl;
			return false;
		}
	}

	return true;
}

void LCGXT::SaveAsGXT(const char *path)
{
	FILE *outputFile;

	char keyName[8];

	long fpKeyBlock, fpDataBlock;
	uint32_t keyBlockSize, dataBlockSize, offset;

	if (this->m_GxtData.empty())
		return;

	keyBlockSize = m_GxtData.size() * SizeOfTKEY;
	dataBlockSize = GetDataBlockSize();

	fpKeyBlock = 8;
	fpDataBlock = keyBlockSize + 8;

	outputFile = fopen(path, "wb");

	if (outputFile == nullptr)
	{
		std::cout << "Create output file failed." << std::endl;
		return;
	}

	fwrite("TKEY", 4, 1, outputFile);
	fwrite(&keyBlockSize, 4, 1, outputFile);
	fseek(outputFile, fpDataBlock, SEEK_SET);
	fwrite("TDAT", 4, 1, outputFile);
	fwrite(&dataBlockSize, 4, 1, outputFile);
	fpDataBlock += 8;

	for (auto &entry : m_GxtData)
	{
		fseek(outputFile, fpKeyBlock, SEEK_SET);
		offset = fpDataBlock - keyBlockSize - 16;
		fwrite(&offset, 4, 1, outputFile);
		memset(keyName, 0, 8);
		strncpy(keyName, entry.first.c_str(), 7);
		fwrite(keyName, 8, 1, outputFile);
		fpKeyBlock += SizeOfTKEY;
		fseek(outputFile, fpDataBlock, SEEK_SET);
		fwrite(entry.second.data(), 2, entry.second.size(), outputFile);
		fpDataBlock += entry.second.size() * 2;
	}

	fclose(outputFile);
}

size_t LCGXT::GetDataBlockSize()
{
	size_t result = 0;

	for (auto &entry : m_GxtData)
		result += entry.second.size() * 2;

	return result;
}

void LCGXT::GenerateWMHHZStuff()
{
	FILE *characters;
	FILE *table;

	uint8_t row = 0;
	uint8_t column = 0;

	sTable.fill({ 63, 63 });

	characters = std::fopen("CHARACTERS.txt", "wb");
	table = std::fopen("wm_lcchs.dat", "wb");

	if (table == nullptr || characters == nullptr)
	{
		std::cout << "Failed to create output file." << std::endl;
		return;
	}

	std::fwrite("\xFF\xFE", 2, 1, characters);

	for (uint16_t chr : this->m_WideCharCollection)
	{
		std::fwrite(&chr, 2, 1, characters);

		sTable[chr].rowIndex = row;
		sTable[chr].columnIndex = column;

		if (column < 63)
		{
			column += 1;
		}
		else
		{
			row += 1;

			std::fwrite(L"\n", 2, 1, characters);

			column = 0;
		}
	}

	std::fwrite(&sTable.front(), 2, 0x10000, table);

	std::fclose(characters);
	std::fclose(table);
}

void LCGXT::UTF8ToUTF16(const std::string &str, std::vector<uint16_t> &result)
{
    result.clear();
    utf8::utf8to16(str.begin(), str.end(), std::back_inserter(result));
    result.push_back(0);
}

void LCGXT::skip_utf8_signature(std::ifstream &stream)
{
	char header[3];

	stream.seekg(0);

	if (stream.get(header[0]) && stream.get(header[1]) && stream.get(header[2]))
	{
		if (header[0] == '\xEF' && header[1] == '\xBB' && header[2] == '\xBF')
		{
			return;
		}
	}

	stream.seekg(0);
}