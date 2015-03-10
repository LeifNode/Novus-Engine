//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_MEMORY_H
#define NOVUS_MEMORY_H

#define DEBUG_MEMORY_ALLOC

void * operator new(size_t size, const char* fileName, const char* functionName, int line);
void * operator new[](size_t size, const char* fileName, const char* functionName, int line);

void operator delete(void * p, const char* fileName, const char* functionName, int line);
void operator delete[](void * p, const char* fileName, const char* functionName, int line);

//void allocTracker_malloc(void * p, const char* fileName, const char* functionName, int line);
void allocTracker_free(void * p, const char* fileName, const char* functionName, int line);

#ifdef DEBUG_MEMORY_ALLOC
	#define NE_NEW new(__FILE__, __FUNCTION__, __LINE__)

	#define NE_DELETE(ptr) allocTracker_free(ptr, __FILE__, __FUNCTION__, __LINE__), delete ptr, ptr = 0
	#define NE_DELETEARR(ptr) allocTracker_free(ptr, __FILE__, __FUNCTION__, __LINE__), delete [] ptr, ptr = 0
#else
	#define NE_NEW new
	//#define NE_DELETE delete

	#define NE_DELETE(ptr) delete ptr, ptr = 0
	#define NE_DELETEARR(ptr) delete[] ptr, ptr = 0
#endif

#endif