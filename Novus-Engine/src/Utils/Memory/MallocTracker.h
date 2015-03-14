//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_MALLOC_TRACKER_H
#define NOVUS_MALLOC_TRACKER_H

#include "Application/Common.h"

namespace novus
{

class MallocTracker
{
	struct MemAllocation
	{
		size_t size;
		std::string fileName;
		std::string functionName;
		int lineNum;
	};

public:
	static MallocTracker* getInstance();

	void Alloc(void* p, size_t size, const char* fileName, const char* functionName, int lineNum);
	bool FreePtr(void* p, const char* fileName, const char* functionName, int lineNum);

	void DumpTrackedMemory();

	unsigned int getUsedMemory() const { return mTotalMemory; }

private:
	MallocTracker();
	~MallocTracker() {}

	MallocTracker(const MallocTracker&);
	MallocTracker& operator= (const MallocTracker&);

private:
	static MallocTracker* mpInstance;

	std::map<void*, MemAllocation> mAllocations;

	unsigned int mTotalMemory;
};

}

#endif