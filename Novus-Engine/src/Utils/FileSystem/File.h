//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once


#ifndef NOVUS_FILE_H
#define NOVUS_FILE_H

#include <string>

namespace novus
{

class File
{
public:
	File();
	~File();

	bool Load(const std::wstring& path);

	void* getMemory() const { return mMemoryBlock; }
	size_t getSize() const { return mSize; }

	void ReadLine(char* charArr, const size_t& maxNum);

	void setReadPos(const size_t& readPos);
	size_t getReadPos() const { return mCursorPos; }

	bool good() const { return mGood; }

	const std::wstring& getPath() const { return mFilePath; }

private:
	std::wstring mFilePath;

	size_t mCursorPos;

	char* mMemoryBlock;
	size_t mSize;

	bool mGood;
};

}

#endif