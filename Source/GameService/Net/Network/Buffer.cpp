
#include "Buffer.h"

#include <cstring>
#include <cassert>


namespace
{
	inline int32_t	GetKBSize(int32_t size)
	{
		constexpr int32_t ONE_KB = 1024;
		if ((size % ONE_KB) == 0)
		{
			return size;
		}

		return ((size / ONE_KB) + 1) * ONE_KB;
	}
}

template<>
FBufferPool* TGsPoolSingle<FBufferPool>::_instance = nullptr;

Buffer::Buffer(int32_t size)
{
	size_ = GetKBSize(size);
	buffer_ = SimpleAllocator::Alloc<uint8_t>(size_);
}

Buffer::~Buffer()
{
	if (buffer_ != nullptr)
	{
		SimpleAllocator::Free(buffer_);
		buffer_ = nullptr;
	}
}

int32_t Buffer::moveStartPos(int32_t diff)
{
	assert(diff <= curPos_);
	if (getSize() < diff)
	{
		return 0;
	}

	startPos_ += diff;
	return space();
}

int32_t Buffer::moveCurPos(int32_t diff)
{
	assert(diff <= space());
	if (space() < diff)
	{
		return 0;
	}

	curPos_ += diff;
	return space();
}

int32_t Buffer::space() const
{
	assert(curPos_ <= size_);
	return size_ - curPos_;
}

int32_t Buffer::copy(const void* data, int32_t size)
{
	if (space() < size)
	{
		return 0;
	}

	std::memcpy(getEndBuffer(), data, size);
	moveCurPos(size);

	return space();
}

int32_t Buffer::compact()
{
	int32_t dataSize = getSize();
	if (dataSize == 0)
	{
		clear();
	}
	else
	{
		if (startPos_ > 0)
		{
			std::memmove(buffer_, getBuffer(), dataSize);

			startPos_ = 0;
			curPos_ = dataSize;
		}
	}

	return space();
}

int32_t Buffer::makeSpace(int32_t size)
{
	int32_t spaceSize = compact();
	if (spaceSize >= size)
	{
		return spaceSize;
	}

	// 버퍼가 부족하다면 할당한다.
	int32_t newSize = GetKBSize(size_ + size);
	uint8_t* temp = SimpleAllocator::Alloc<uint8_t>(newSize);
	std::memcpy(temp, getBuffer(), getSize());

	SimpleAllocator::Free(buffer_);
	buffer_ = temp;
	size_ = newSize;
	return space();
}

void Buffer::clear()
{
	startPos_ = 0;
	curPos_ = 0;
}

