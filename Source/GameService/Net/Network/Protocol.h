
#pragma once

#include <cstdint>
#include <cassert>
#include <cstring>

#include "AllocateHelper.h"
#include "Buffer.h"
#include "Crc32.h"
#include "./Crypto/ChaCha20.h"
#include "CoreMinimal.h"
#include "../../Class/GsSingleton.h"

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
		//(data);
		data_ = SimpleAllocator::Alloc<uint8_t>(len);
		len_ = len;
	}
	~Packet()
	{
		if (data_ != nullptr)
		{
			SimpleAllocator::Free(data_);
			data_ = nullptr;
		}
	}

	uint32_t calcCrc32() const
	{
		return Crc32::Compute(data_, len_);
	}

	/*
	static void* operator new(size_t size)
	{
		return SimpleAllocator::Alloc<Buffer>(size);
	}
	static void operator delete(void* ptr)
	{
		return SimpleAllocator::Free(ptr);
	}*/
};


// 최초 할당수 100
class FPacketPool 
#ifdef __UNREAL__
	: public TLockFreeClassAllocator<Packet, 100>, TGsPoolSingle<FPacketPool>
#endif
{
public:
	Packet* New(const PacketHeader* header, const void* data, uint32_t len)
	{
#ifdef __UNREAL__
		return new (Allocate()) Packet(header, data, len);
#else
		return new Packet(header, data, len);
#endif
	}
	void Delete(Packet* in)
	{
#ifdef __UNREAL__
		Free(in);
#else
		delete in;
#endif
	}
};
template<>
FPacketPool* TGsPoolSingle<FPacketPool>::_instance = nullptr;
#define GetPacketPool() TGsPoolSingle<FPacketPool>::GetInstance()

inline Buffer* MakePacket(uint16_t packetId, const void* data, int32_t size)
{
	int32_t totalSize = sizeof(PacketHeader) + size;
	Buffer* buffer = GetBufferPool()->New(totalSize);//new Buffer(totalSize); 

	PacketHeader header;
	header.packetId = packetId;
	header.size = totalSize;
	header.crc32 = 0;
	buffer->copy(&header, sizeof(header));

	buffer->copy(data, size);
	return buffer;
}

inline const PacketHeader*	SeekPacketHeader(const Buffer* buffer)
{
	uint32_t remainLen = buffer->getSize();
	const uint8_t* streamBuf = buffer->getBuffer();

	if (remainLen >= sizeof(PacketHeader))
	{
		return reinterpret_cast<const PacketHeader*>(streamBuf);
	}

	return nullptr;
}

inline bool PopPacket(Buffer* buffer, ChaCha20Decrypter* decrypter, Packet*& packet)
{
	assert(packet == nullptr);

	uint32_t remainLen = buffer->getSize();
	uint8_t* streamBuf = buffer->getBuffer();

	if (remainLen >= sizeof(PacketHeader))
	{
		const PacketHeader* header = reinterpret_cast<const PacketHeader*>(streamBuf);
		if (header->size < sizeof(PacketHeader))
		{
			return false;
		}

		if (header->size > remainLen)
		{
			int32_t needLen = header->size - remainLen;
			if (buffer->makeSpace(needLen) < needLen)
			{
				return false;
			}

			return true;
		}

		void* data = streamBuf + sizeof(PacketHeader);
		uint32_t dataSize = header->size - sizeof(PacketHeader);

		//packet = new Packet(header, data, dataSize);
		packet = GetPacketPool()->New(header, data, dataSize);

		if (decrypter != nullptr)
		{
			decrypter->decrypt(data, dataSize, packet->data_, packet->len_);
		}
		else
		{
			std::memcpy(packet->data_, data, dataSize);
		}

		buffer->moveStartPos(header->size);
		return true;
	}

	return true;
}

