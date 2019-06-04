
#pragma once

#include <cstdint>

#include "AllocateHelper.h"
#include "CoreMinimal.h"
#include "../../Class/GsSingleton.h"


class Buffer
{
	uint8_t* buffer_ = nullptr;
	int32_t size_ = 0;
	int32_t startPos_ = 0;
	int32_t curPos_ = 0;

public:

	explicit Buffer(int32_t size);
	~Buffer();

	/*static void* operator new(size_t size)
	{
		return SimpleAllocator::Alloc<Buffer>(size);
	}
	static void operator delete(void* ptr)
	{
		return SimpleAllocator::Free(ptr);
	}*/

	uint8_t* getBuffer() { return buffer_ + startPos_; }
	const uint8_t* getBuffer() const { return buffer_ + startPos_; }

	uint8_t* getEndBuffer() { return buffer_ + curPos_; }

	int32_t getSize() const { return curPos_ - startPos_; }

	int32_t moveStartPos(int32_t diff);
	int32_t moveCurPos(int32_t diff);

	int32_t space() const;
	int32_t copy(const void* data, int32_t size);

	int32_t compact();
	int32_t makeSpace(int32_t size);

	void clear();

};


class FBufferPool
#ifdef __UNREAL__
	: public TLockFreeClassAllocator<Buffer, 100>, TGsPoolSingle<FBufferPool>
#endif
{
public:
	Buffer* New(int32_t size)
	{
#ifdef __UNREAL__
		return new (Allocate()) Buffer(size);
#else
		return new Buffer(size);
#endif
	}
	void Delete(Buffer* in)
	{
#ifdef __UNREAL__
		Free(in);
#else
		delete in;
#endif
	}
};

#define GetBufferPool() TGsPoolSingle<FBufferPool>::GetInstance()