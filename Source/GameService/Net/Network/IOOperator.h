
#pragma once

#include <cstdint>

#include "SocketConfig.h"
#include "Buffer.h"
#include "Protocol.h"


class OnePacketReceiver
{
	FSocket* socket_ = nullptr;
	Buffer buffer_;
	int32_t buffSize_ = 0;

	Packet* packet_ = nullptr;

public:

	OnePacketReceiver(FSocket* socket, int32_t bufferSize);

	bool receive();
	Packet* getPacket() { return packet_; }

};

class OnePacketSender
{
	FSocket* socket_ = nullptr;
	Buffer* buffer_ = nullptr;

public:

	~OnePacketSender();

	void make(FSocket* socket, uint8_t packetId, const void* data, int32_t size);

	bool send();
	bool isDone() const;

};

