
#pragma once

#include <cstdint>
#include <cassert>

#include "AllocateHelper.h"
#include "Buffer.h"
#include "Crc32.h"


// #pragma pack(push, 1)은 MSVC, gcc, clang 모두 지원하는 것 같다.
// 빌드 해보고 실패하면 그때 수정하자
#pragma pack(push, 1)
struct PacketHeader
{
	uint32_t size = 0;
	uint16_t packetId = 0;
	uint32_t crc32 = 0;
};
#pragma pack(pop)


class Packet
{
public:

	PacketHeader header_;
	uint8_t* data_ = nullptr;
	uint32_t len_ = 0;

	Packet(const PacketHeader* header, const void* data, uint32_t len)
		: header_(*header)
	{
		data_ = SimpleAllocator::Alloc<uint8_t>(len);
		len_ = len;

		if (header->crc32 != 0)
		{
			uint32_t crc32 = Crc32::Compute(data, len);
			(crc32);
			assert(crc32 == header->crc32);
		}

		std::memcpy(data_, data, len);
	}
	~Packet()
	{
		if (data_ != nullptr)
		{
			SimpleAllocator::Free(data_);
			data_ = nullptr;
		}
	}

	static void* operator new(size_t size)
	{
		return SimpleAllocator::Alloc<Buffer>(size);
	}
	static void operator delete(void* ptr)
	{
		return SimpleAllocator::Free(ptr);
	}

};



inline Buffer* MakePacket(uint16_t packetId, const void* data, int32_t size)
{
	int32_t totalSize = sizeof(PacketHeader) + size;
	Buffer* buffer = new Buffer(totalSize);

	PacketHeader header;
	header.packetId = packetId;
	header.size = totalSize;
	header.crc32 = Crc32::Compute(data, size);
	buffer->copy(&header, sizeof(header));

	buffer->copy(data, size);
	return buffer;
}

inline bool PopPacket(Buffer* buffer, Packet*& packet)
{
	assert(packet == nullptr);

	uint32_t remainBytes = buffer->getSize();
	uint8_t* nowBuf = buffer->getBuffer();

	if (remainBytes >= sizeof(PacketHeader))
	{
		const PacketHeader* header = reinterpret_cast<const PacketHeader*>(nowBuf);
		if (header->size < sizeof(PacketHeader))
		{
			return false;
		}

		if (header->size > remainBytes)
		{
			buffer->makeSpace(header->size - remainBytes);
			return true;
		}

		void* body = nowBuf + sizeof(PacketHeader);
		uint32_t bodySize = header->size - sizeof(PacketHeader);

		packet = new Packet(header, body, bodySize);

		buffer->moveStartPos(header->size);
		return true;
	}

	return true;
}

