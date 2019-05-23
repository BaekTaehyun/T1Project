
#include "Session.h"
#include "SocketTask.h"
#include "TaskExecuter.h"
#include "Protocol.h"

Session::Session(SessionEventHandler* handler)
	: handler_(handler)
{
}

Session::~Session()
{
}

bool Session::connect(const char* host, uint16_t port)
{
	if (connected_ == true)
	{
		disconnect();
	}

	auto task = std::make_shared<ConnectSocketTask>(shared_from_this(), host, port);
	TaskExecuter::GetInstance()->push(task);
	return true;
}

void Session::disconnect()
{
	bool old = connected_.exchange(false);
	if (old == false)
	{
		return;
	}

	FSocket* socket = socket_;
	if (socket == nullptr)
	{
		return;
	}

	socket_ = nullptr;
	sendTask_ = nullptr;

	auto task = std::make_shared<DisconnectSocketTask>(shared_from_this(), socket);
	TaskExecuter::GetInstance()->push(task, 500);
}

bool Session::isConnected() const
{
	return connected_;
}

void Session::processIOEvent()
{
	auto session = shared_from_this();

	std::lock_guard<std::mutex> guard(eventLock_);
	if (handler_ != nullptr)
	{
		for (auto& e : ioEventArgs_)
		{
			if (e.type == IOEventType::Connected)
			{
				handler_->onConnected(e.result, session);
			}
			else if (e.type == IOEventType::Disconnected)
			{
				handler_->onDisconnected(session);
			}
			else if (e.type == IOEventType::Error)
			{
				handler_->onError(session);
			}
		}
	}

	ioEventArgs_.clear();
}

std::vector<Packet*> Session::popAll()
{
	std::vector<Packet*> packets;

	{
		std::lock_guard<std::mutex> guard(lock_);
		if (packets_.empty() == false)
		{
			packets_.swap(packets);
		}
	}

	return packets;
}

bool Session::send(uint16_t packetId, const void* data, int32_t size)
{
	if (isConnected() == false)
	{
		return false;
	}

	std::shared_ptr<SendSocketTask> sendTask = sendTask_;
	if (sendTask == nullptr)
	{
		return false;
	}

	if (sendTask->send(packetId, data, size) == false)
	{
		return false;
	}

	return true;
}


void Session::onConnected(bool result, FSocket* socket, std::shared_ptr<SendSocketTask> sender, std::shared_ptr<ReceiveSocketTask> receiver)
{
	if (result == false)
	{
		pushEvent(IOEventType::Connected, result);
		DestroyFSocket(socket);
		return;
	}

	socket_ = socket;
	start(socket, sender, receiver);
}

void Session::start(FSocket* socket, std::shared_ptr<SendSocketTask> sender, std::shared_ptr<ReceiveSocketTask> receiver)
{
	(socket);
	clear();

	connected_ = true;
	sendTask_ = sender;

	TaskExecuter::GetInstance()->push(receiver);

	pushEvent(IOEventType::Connected, true);
}

void Session::clear()
{
	{
		std::lock_guard<std::mutex> guard(lock_);
		packets_.clear();
	}
}

void Session::onPacket(FSocket* socket, std::vector<Packet*> packets)
{
	if (socket_ != socket)
	{
		for (auto packet : packets)
		{
			GetPacketPool()->Delete(packet);
		}

		return;
	}

	std::lock_guard<std::mutex> guard(lock_);
	for (auto packet : packets)
	{
		packets_.push_back(packet);
	}
}

void Session::onError(FSocket* socket)
{
	if (socket != socket_)
	{
		return;
	}

	pushEvent(IOEventType::Error, false);
	disconnect();
}

void Session::onDisconnected(FSocket* socket)
{
	pushEvent(IOEventType::Disconnected, true);
	DestroyFSocket(socket);
}

void Session::pushEvent(IOEventType type, bool result)
{
	std::lock_guard<std::mutex> guard(eventLock_);
	ioEventArgs_.emplace_back(type, result);
}