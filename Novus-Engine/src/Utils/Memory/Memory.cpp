#include "Memory.h"
#include "MallocTracker.h"

using novus::MallocTracker;

void* operator new(size_t size, const char* fileName, const char* functionName, int line)
{
	void* pMemory = ::operator new(size);

	MallocTracker::getInstance()->Alloc(pMemory, size, fileName, functionName, line);

	return pMemory;
}

void* operator new[](size_t size, const char* fileName, const char* functionName, int line)
{
	void* pMemory = ::operator new[](size);

	MallocTracker::getInstance()->Alloc(pMemory, size, fileName, functionName, line);

	return pMemory;
}

void operator delete(void * p, const char* fileName, const char* functionName, int line)
{
	MallocTracker::getInstance()->FreePtr(p);

	::operator delete(p);
}

void operator delete[](void * p, const char* fileName, const char* functionName, int line)
{
	MallocTracker::getInstance()->FreePtr(p);

	::operator delete[](p);
}

void allocTracker_free(void * p, const char* fileName, const char* functionName, int line)
{
	MallocTracker::getInstance()->FreePtr(p);
}