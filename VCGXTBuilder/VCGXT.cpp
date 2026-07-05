#include "VCGXT.h"

#include <array>
#include <iostream>
#include <fstream>
#include <regex>
#include <algorithm>
#include <cstring>
#include <cstdio>
#include "../deps/utf8cpp/utf8.h"

struct CharPos
{
	unsigned __int8 rowIndex;
	unsigned __int8 columnIndex;
};

wchar_t MostUsedCharacters[] = L"";

static std::array<CharPos, 0x10000> sTable;

VCGXT::VCGXT()
{
    m_WideCharCollection.insert(std::begin(MostUsedCharacters), std::end(MostUsedCharacters) - 1);
}

bool VCGXT::LoadText(const char *path)
{
	std::ifstream inputFile;

	std::string lineBuffer;

	std::vector<uint16_t> wideLineBuffer;

	std::regex tableFormat(R"-(\[([0-9A-Z_]{1,7})\])-");
	std::regex entryFormat(R"-(([0-9A-Z_]{1,7})=(.+))-");
	std::smatch matchResult;

	std::map<std::string, std::map<std::string, std::vector<uint16_t> >, TableSortMethod>::iterator tableIt;

	m_GxtData.clear();

	inputFile.open(path);

	if (!inputFile)
	{
		std::cout << "Open input file failed.\n";
		return false;
	}

	skip_utf8_signature(inputFile);

	while (std::getline(inputFile, lineBuffer))
	{
		if (lineBuffer.empty() || lineBuffer.front() == ';')
		{
			continue;
		}
		else if (std::regex_match(lineBuffer, matchResult, tableFormat))
		{
			tableIt = m_GxtData.emplace(matchResult.str(1), std::map<std::string, std::vector<uint16_t> >()).first;
		}
		else if (std::regex_match(lineBuffer, matchResult, entryFormat))
		{
			if (m_GxtData.empty())
			{
				std::cout << "Key " << matchResult.str(1) << "belongs to no table.\n";
				return false;
			}

			UTF8ToUTF16(matchResult.str(2), wideLineBuffer);

			if ((std::count(wideLineBuffer.begin(), wideLineBuffer.end(), '~') % 2) != 0)
			{
				std::cout << "Invalid token format at key: " << matchResult.str(1) << ". Skipped." << '\n';
				continue;
			}

			if (tableIt->second.emplace(matchResult.str(1), wideLineBuffer).second)
			{
				for (uint16_t chr : wideLineBuffer)
				{
					if (chr >= 0x80)
					{
						m_WideCharCollection.insert(chr);
					}
				}
			}
			else
			{
				std::cout << "Repeated entry:\n" << matchResult.str(1) << "\nin table:\n" << tableIt->first << '\n' << '\n';
				return false;
			}
		}
		else
		{
			std::cout << "Invalid line:\n" << lineBuffer << '\n' << '\n';
			return false;
		}
	}

	return true;
}

void VCGXT::SaveAsGXT(const char *path)
{
	FILE *outputFile;
	long foTableBlock, foKeyBlock, foDataBlock;
	int32_t tableBlockSize, keyBlockSize, TDATOffset, firstTDATEntryOffset, dataBlockSize;
	char eightChars[8];

	tableBlockSize = this->m_GxtData.size() * SizeOfTABL;

	outputFile = std::fopen(path, "wb");

	if (outputFile == nullptr)
	{
		std::cout << "Failed to create output file.\n";
		return;
	}

	std::fwrite("TABL", 4, 1, outputFile);
	std::fwrite(&tableBlockSize, 4, 1, outputFile);
	foTableBlock = 8;
	std::fseek(outputFile, tableBlockSize, SEEK_CUR);

	for (auto &table : this->m_GxtData)
	{
		foKeyBlock = std::ftell(outputFile);

		keyBlockSize = table.second.size() * SizeOfTKEY;
		dataBlockSize = GetDataBlockSize(table.second);

		std::memset(eightChars, 0, 8);
		table.first.copy(eightChars, 7);
		std::fseek(outputFile, foTableBlock, SEEK_SET);
		std::fwrite(eightChars, 1, 8, outputFile);
		std::fwrite(&foKeyBlock, 4, 1, outputFile);
		foTableBlock += SizeOfTABL;

		std::fseek(outputFile, foKeyBlock, SEEK_SET);
		if (table.first != "MAIN")
		{
			std::fwrite(eightChars, 1, 8, outputFile);
		}
		std::fwrite("TKEY", 1, 4, outputFile);
		std::fwrite(&keyBlockSize, 4, 1, outputFile);
		foKeyBlock = std::ftell(outputFile);

		std::fseek(outputFile, keyBlockSize, SEEK_CUR);
		std::fwrite("TDAT", 1, 4, outputFile);
		std::fwrite(&dataBlockSize, 4, 1, outputFile);

		firstTDATEntryOffset = ftell(outputFile);
		
		for (auto &entry : table.second)
		{
			foDataBlock = ftell(outputFile);

			std::memset(eightChars, 0, 8);
			entry.first.copy(eightChars, 7);
			TDATOffset = foDataBlock - firstTDATEntryOffset;
			std::fseek(outputFile, foKeyBlock, SEEK_SET);
			std::fwrite(&TDATOffset, 4, 1, outputFile);
			std::fwrite(eightChars, 1, 8, outputFile);
			foKeyBlock += SizeOfTKEY;
			std::fseek(outputFile, foDataBlock, SEEK_SET);
			std::fwrite(entry.second.data(), 2, entry.second.size(), outputFile);
			
		}
	}

	std::fclose(outputFile);
}

void VCGXT::GenerateWMHHZStuff()
{
	FILE *characters;
	FILE *table;

	unsigned __int8 row = 0;
	unsigned __int8 column = 0;

	sTable.fill({ 63, 63 });

	characters = std::fopen("CHARACTERS.txt", "wb");
	table = std::fopen("wm_vcchs.dat", "wb");

	if (table == nullptr || characters == nullptr)
	{
		std::cout << "Failed to create output file.\n";
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

size_t VCGXT::GetDataBlockSize(const std::map<std::string,std::vector<uint16_t> > &table)
{
	size_t result = 0;

	for (auto &entry : table)
	{
		result += (entry.second.size() * 2);
	}

	return result;
}

void VCGXT::UTF8ToUTF16(const std::string &str, std::vector<uint16_t> &result)
{
    result.clear();
    utf8::utf8to16(str.begin(), str.end(), std::back_inserter(result));
    result.push_back(0);
}

void VCGXT::skip_utf8_signature(std::ifstream &stream)
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
