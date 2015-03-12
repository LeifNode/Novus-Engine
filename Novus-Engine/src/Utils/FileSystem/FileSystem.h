//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once


#ifndef NOVUS_FILE_SYSTEM_H
#define NOVUS_FILE_SYSTEM_H

#include <string>

namespace novus
{

class File;

class FileSystem
{
	friend class NovusApplication;

public:
	File* OpenFile(const wchar_t* path);

private:
	FileSystem(const wchar_t* relativePath);
	~FileSystem();

private:
	std::wstring mRelativePath;
};

}

#endif