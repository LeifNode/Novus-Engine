#include "FileSystem.h"
#include "File.h"
#include <fstream>

namespace novus
{

FileSystem::FileSystem(const wchar_t* relativePath)
	: mRelativePath(relativePath)
{
}

FileSystem::~FileSystem()
{
}

File* FileSystem::OpenFile(const wchar_t* path)
{
	std::wstring fullPath;

	fullPath.append(mRelativePath);
	fullPath.append(path);

	std::ifstream file;

	return NULL;
}

}