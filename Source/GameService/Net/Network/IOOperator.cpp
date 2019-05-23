
#include "IOOperator.h"


OnePacketReceiver::OnePacketReceiver(FSocket* socket, int32_t bufferSize)
	: socket_(socket)
	, buffer_(bufferSize)
	, buffSize_(sizeof(PacketHeader))
{
}

bool OnePacketReceiver::receive()
{
	if (packet_ != nullptr)
	{
		return true;
	}

	int32_t recvSize = 0;
	if (socket_->Recv(buffer_.getEndBuffer(), buffSize_ - buffer_.getSize(), recvSize) == false)
	{
		return false;
	}

	buffer_.moveCurPos(recvSize);

	if (PopPacket(&buffer_, nullptr, packet_) == false)
	{
		return false;
	}

	if (packet_ == nullptr)
	{
		const PacketHeader* header = SeekPacketHeader(&buffer_);
		if (header != nullptr)
		{
			if (header->size < sizeof(PacketHeader))
			{
				return false;
			}

			buffSize_ = header->size;
		}

		return true;
	}

	return true;
}


OnePacketSender::~OnePacketSender()
{
	delete buffer_;
}

void OnePacketSender::make(FSocket* socket, uint8_t packetId, const void* data, int32_t size)
{
	socket_ = socket;
	buffer_ = MakePacket(packetId, data, size);
}

bool OnePacketSender::send()
{
	if (isDone() == true)
	{
		return true;
	}

	int32_t sent = 0;
	if (socket_->Send(buffer_->getBuffer(), buffer_->getSize(), sent) == false)
	{
		return false;
	}

	buffer_->moveStartPos(sent);
	return true;
}

bool OnePacketSender::isDone() const
{
	if (buffer_ == nullptr)
	{
		return false;
	}

	return buffer_->getSize() == 0;
}

