
#include "SocketTask.h"
#include "TaskExecuter.h"
#include "Session.h"

#include <vector>


ConnectSocketTask::ConnectSocketTask(std::shared_ptr<Session> session, const char* host, uint16_t port)
	: session_(session)
	, host_(host)
	, port_(port)
{
}

bool ConnectSocketTask::onExecute(TaskExecuter* executer)
{
	(executer);
	socket_ = ConnectFSocket(host_.c_str(), port_);
	if (socket_ == nullptr)
	{
		result_ = false;
		return true;
	}

	return true;
}

void ConnectSocketTask::onEnd(TaskExecuter* executer)
{
	if (isSucceed() == false)
	{
		session_->onConnected(isSucceed(), socket_);
	}
	else
	{
		auto task = std::make_shared<CheckConnectSocketTask>(session_, socket_);
		executer->push(task);
	}
}

CheckConnectSocketTask::CheckConnectSocketTask(std::shared_ptr<Session> session, FSocket* socket)
	: session_(session)
	, socket_(socket)
{
}

bool CheckConnectSocketTask::onExecute(TaskExecuter* executer)
{
	bool done = false;
	if (socket_->HasPendingConnection(done) == false)
	{
		result_ = false;
		return true;
	}

	if (done == true)
	{
		return true;
	}

	executer->push(shared_from_this());
	return false;
}

void CheckConnectSocketTask::onEnd(TaskExecuter* executer)
{
	(executer);
	session_->onConnected(isSucceed(), socket_);
}


ReceiveSocketTask::ReceiveSocketTask(std::shared_ptr<Session> session, FSocket* socket, int32_t bufferSize)
	: session_(session)
	, socket_(socket)
	, buffer_(bufferSize)
{
}

bool ReceiveSocketTask::onExecute(TaskExecuter* executer)
{
	if (session_->isConnected() == false)
	{
		result_ = false;
		return true;
	}

	uint32_t pendingSize = 0;
	if (socket_->HasPendingData(pendingSize) == true)
	{
		if (onReceive() == false)
		{
			result_ = false;
			return true;
		}

		return true;
	}

	executer->push(shared_from_this());
	return false;
}

bool ReceiveSocketTask::onReceive()
{
	buffer_.compact();

	int recvSize = 0;
	if (socket_->Recv(buffer_.getEndBuffer(), buffer_.space(), recvSize) == false)
	{
		return false;
	}

	buffer_.moveCurPos(recvSize);
	return true;
}

void ReceiveSocketTask::onEnd(TaskExecuter* executer)
{
	(executer);
	if (isSucceed() == false)
	{
		session_->onError(socket_);
		return;
	}

	std::vector<Packet*> packets;
	while (true)
	{
		Packet* packet = nullptr;
		if (PopPacket(&buffer_, packet) == false)
		{
			session_->onError(socket_);
			break;
		}

		if (packet == nullptr)
		{
			break;
		}

		packets.push_back(packet);
	}

	if (packets.empty() == false)
	{
		session_->onPacket(socket_, std::move(packets));
	}

	if (session_->isConnected() == true)
	{
		TaskExecuter::GetInstance()->push(shared_from_this());
	}
}



SendSocketTask::SendSocketTask(std::shared_ptr<Session> session, FSocket* socket, int32_t bufferSize)
	: session_(session)
	, socket_(socket)
	, buffer_(bufferSize)
{
}

SendSocketTask::~SendSocketTask()
{
	clear();
}

bool SendSocketTask::send(uint16_t packetId, const void* data, int32_t size)
{
	Buffer* buffer = MakePacket(packetId, data, size);

	{
		std::lock_guard<std::mutex> guard(lock_);
		request_.push_back(buffer);
		if (isSending_ == true)
		{
			return true;
		}

		isSending_ = true;
	}

	TaskExecuter::GetInstance()->push(shared_from_this());
	return true;
}

bool SendSocketTask::isComplete() const
{
	if (request_.empty() == false)
	{
		return false;
	}

	if (pending_.empty() == false)
	{
		return false;
	}

	if (buffer_.getSize() > 0)
	{
		return false;
	}

	return true;
}

void SendSocketTask::clear()
{
	{
		std::lock_guard<std::mutex> guard(lock_);
		for (auto buffer : request_)
		{
			delete buffer;
		}
		request_.clear();
		isSending_ = true;
	}

	{
		for (auto buffer : pending_)
		{
			delete buffer;
		}
		pending_.clear();
		buffer_.clear();
	}
}

bool SendSocketTask::onExecute(TaskExecuter* executer)
{
	if (session_->isConnected() == false)
	{
		result_ = false;
		return true;
	}

	{
		std::lock_guard<std::mutex> guard(lock_);
		pending_.assign(request_.begin(), request_.end());
		request_.clear();
	}

	if (pending_.size() > LIMIT_SEND_QUEUE_COUNT)
	{
		result_ = false;
		return true;
	}

	// 모은 데이터 사이즈가 MAX_SEND_BUFFER 보다 작다면 좀더 모은다.
	// (하나의 패킷 사이즈가 MAX_SEND_BUFFER 보다 크다면 다 보내고 모은다.
	if (buffer_.getSize() < MAX_SEND_BUFFER)
	{
		while (pending_.empty() == false)
		{
			Buffer* buffer = pending_.front();
			onReadyToSend(buffer);
			delete buffer;

			pending_.pop_front();

			if (buffer_.getSize() > MAX_SEND_BUFFER)
			{
				break;
			}
		}
	}

	if (onSend() == false)
	{
		result_ = false;
		return true;
	}

	executer->push(shared_from_this());
	return false;
}

void SendSocketTask::onReadyToSend(Buffer* buffer)
{
	buffer_.makeSpace(buffer->getSize());
	buffer_.copy(buffer->getBuffer(), buffer->getSize());
}

bool SendSocketTask::onSend()
{
	if (buffer_.getSize() > 0)
	{
		int32_t sent = 0;
		if (socket_->Send(buffer_.getBuffer(), buffer_.getSize(), sent) == false)
		{
			return false;
		}

		buffer_.moveStartPos(sent);
		buffer_.compact();
	}

	return true;
}

void SendSocketTask::onEnd(TaskExecuter* executer)
{
	if (isSucceed() == true)
	{
		{
			std::lock_guard<std::mutex> guard(lock_);
			isSending_ = !isComplete();
			if (isSending_ == false)
			{
				return;
			}
		}

		executer->push(shared_from_this());
	}
	else
	{
		session_->onError(socket_);
	}

	clear();
}


DisconnectSocketTask::DisconnectSocketTask(std::shared_ptr<Session> session, FSocket* socket)
	: session_(session)
	, socket_(socket)
{
}

bool DisconnectSocketTask::onExecute(TaskExecuter* executer)
{
	(executer);
	socket_->Close();
	return true;
}

void DisconnectSocketTask::onEnd(TaskExecuter* executer)
{
	(executer);
	session_->onDisconnected(socket_);
}

