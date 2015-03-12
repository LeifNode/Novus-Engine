#include "File.h"
#include "Application/Common.h"

namespace novus
{

File::File()
	: mMemoryBlock(NULL),
	mSize(0),
	mCursorPos(0),
	mGood(false)
{
}

File::~File()
{
	NE_DELETE(mMemoryBlock);
}

bool File::Load(const std::wstring& path)
{
	NE_DELETE(mMemoryBlock);
	mSize = 0;
	mCursorPos = 0;
	mGood = false;

	std::ifstream file = std::ifstream(path, std::ios::ate | std::ios::binary);
	if (!file.good())
		return false;

	mGood = true;

	mSize = file.tellg();

	mMemoryBlock = NE_NEW char[mSize];

	file.seekg(std::ios::beg);

	file.read(mMemoryBlock, mSize);

	file.close();

	return true;
}

void File::ReadLine(char* charArr, const size_t& maxNum)
{
	bool newLine = false;
	for (int c = 0; c < maxNum; c++)
	{
		if (mCursorPos >= mSize)
		{
			mGood = false;
			charArr[c] = '\0';
		}
		else if (newLine || !mGood)
		{
			charArr[c] = '\0';
		}
		else
		{
			charArr[c] = reinterpret_cast<char*>(mMemoryBlock)[mCursorPos];
			mCursorPos++;

			if (charArr[c] == '\n' || charArr[c] == '\r')
			{
				charArr[c] = '\0';
				newLine = true;
			}
		}
	}
}

void File::setReadPos(const size_t& readPos)
{
	mCursorPos = readPos; 

	if (mCursorPos < mSize)
		mGood = true;
}

}