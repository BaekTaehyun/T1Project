#pragma once


#include "SocketConfig.h"

#ifndef __UNREAL__

#include <cstdlib>

class StdAllocator
{
public:

	template<typename T>
	static T* Alloc(size_t size)
	{
		if (size <= 0)
		{
			size = 1;
		}

		return static_cast<T*>(std::malloc(size));
	}

	static void Free(void* ptr)
	{
		if (ptr == nullptr)
		{
			return;
		}

		std::free(ptr);
	}
};

using SimpleAllocator = StdAllocator;

#else

#include "UnrealMemory.h"

class UE4Allocator
{
public:

	template<typename T>
	static T* Alloc(size_t size)
	{
		if (size <= 0)
		{
			size = 1;
		}

		return static_cast<T*>(FMemory::Malloc(size));
	}

	static void Free(void* ptr)
	{
		if (ptr == nullptr)
		{
			return;
		}

		FMemory::Free(ptr);
	}
};

using SimpleAllocator = UE4Allocator;

#endif

