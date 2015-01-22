#include "MallocTracker.h"
//#include "Logger.h"
#include <sstream>

using novus::MallocTracker;

MallocTracker* MallocTracker::mpInstance = NULL;

MallocTracker* MallocTracker::getInstance()
{
	if (mpInstance == NULL)
		mpInstance = new MallocTracker();

	return mpInstance;
}

MallocTracker::MallocTracker()
	:
	mTotalMemory(0)
{

}

void MallocTracker::Alloc(void* p, size_t size, const char* fileName, const char* functionName, int lineNum)
{
	//if (free(p))

	MemAllocation memAlloc;
	memAlloc.Size = size;
	memAlloc.FileName = fileName;
	memAlloc.FunctionName = functionName;
	memAlloc.LineNum = lineNum;

	mAllocations[p] = memAlloc;

	mTotalMemory += static_cast<unsigned int>(size);
}

bool MallocTracker::FreePtr(void* p)
{
	auto it = mAllocations.find(p);

	if (it != mAllocations.end())
	{
		mTotalMemory -= static_cast<unsigned int>(it->second.Size);

		mAllocations.erase(it);

		return true;
	}

	return false;
}

void MallocTracker::DumpTrackedMemory()
{
	char sizeStr[25] = { 0 };

	sprintf(sizeStr, "%fKB\n", mTotalMemory / 1024.0f);

	OutputDebugStringA("Total memory allocated: ");
	OutputDebugStringA(sizeStr);
	OutputDebugStringA("Remaining memory allocations:\n");

	for (auto it = mAllocations.begin(); it != mAllocations.end(); ++it)
	{
		std::stringstream debugString;

		debugString << "Size: " << it->second.Size << "B, File: " << it->second.FileName << ", Function: " << it->second.FunctionName << ", Line: " << it->second.LineNum << std::endl;

		OutputDebugStringA(debugString.str().c_str());
	}
}