
#include "SocketTask.h"
#include "TaskExecuter.h"
#include "Session.h"
#include "Crc32.h"

#include <vector>


ConnectSocketTask::ConnectSocketTask(std::shared_ptr<Session> session, const char* host, uint16_t port)
	: session_(session)
	, host_(host)
	, port_(port)
{
}

bool ConnectSocketTask::onExecute(TaskExecuter* executer)
{
	//(executer);
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
		session_->onConnected(isSucceed(), socket_, nullptr, nullptr);
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
	if (isSucceed() == false)
	{
		session_->onConnected(isSucceed(), socket_, nullptr, nullptr);
	}
	else
	{
		auto task = std::make_shared<KeyExchangeSocketTask>(session_, socket_);
		executer->push(task);
	}
}


const uint16_t KEY_EXCHANGE_PACKET_ID = 1;

constexpr uint32_t CHACHA_KEY_SIZE = sizeof(CHACAH20_KEY::key) + sizeof(CHACAH20_KEY::counter);
constexpr uint32_t KEY_EXCHANGE_COUNT = CHACHA_KEY_SIZE / sizeof(uint64_t);
constexpr uint32_t KEY_EXCHANGE_SIZE = KEY_EXCHANGE_COUNT * sizeof(uint64_t);


KeyExchangeSocketTask::KeyExchangeSocketTask(std::shared_ptr<Session> session, FSocket* socket)
	: session_(session)
	, socket_(socket)
{
	keyExchange_ = new KeyExchange();
	auto key = keyExchange_->getPublicKey(DiffieHellman::P64, DiffieHellman::G, KEY_EXCHANGE_COUNT);
	sender_.make(socket_, KEY_EXCHANGE_PACKET_ID, key.data(), KEY_EXCHANGE_SIZE);
}

bool KeyExchangeSocketTask::onExecute(TaskExecuter* executer)
{
	if (sender_.send() == false)
	{
		result_ = false;
		return true;
	}

	if (sender_.isDone() == false)
	{
		executer->push(shared_from_this());
		return false;
	}

	return true;
}

void KeyExchangeSocketTask::onEnd(TaskExecuter* executer)
{
	if (isSucceed() == false)
	{
		session_->onConnected(isSucceed(), socket_, nullptr, nullptr);
	}
	else
	{
		auto task = std::make_shared<CryptoMakerSocketTask>(session_, socket_, keyExchange_);
		executer->push(task);
	}
}

CryptoMakerSocketTask::CryptoMakerSocketTask(std::shared_ptr<Session> session, FSocket* socket, KeyExchange* keyExchange)
	: session_(session)
	, socket_(socket)
	, keyExchange_(keyExchange)
	, receiver_(socket, 1024)
{
}

bool CryptoMakerSocketTask::onExecute(TaskExecuter* executer)
{
	if (receiver_.receive() == false)
	{
		result_ = false;
		return true;
	}

	Packet* packet = receiver_.getPacket();
	if (packet == nullptr)
	{
		executer->push(shared_from_this());
		return false;
	}

	if (makeCrypto(packet) == false)
	{
		result_ = false;
		return true;
	}

	return true;
}

bool CryptoMakerSocketTask::makeCrypto(Packet* packet)
{
	if (packet->header_.packetId != KEY_EXCHANGE_PACKET_ID ||
		packet->len_ != KEY_EXCHANGE_SIZE)
	{
		return false;
	}

	if (packet->header_.crc32 != 0 &&
		packet->header_.crc32 != packet->calcCrc32())
	{
		return false;
	}

	const uint64_t* b = reinterpret_cast<const uint64_t*>(packet->data_);
	std::vector<uint64_t> Bs;
	for (int i = 0; i < 6; ++i)
	{
		Bs.push_back(*b);
		++b;
	}

	auto sk = keyExchange_->getSecretKey(Bs);
	CHACAH20_KEY key;
	::memcpy(key.key, sk.data(), sizeof(key.key));
	::memcpy(key.counter, sk.data() + sizeof(key.key), sizeof(key.counter));

	encrypter_ = new ChaCha20Encrypter();
	decrypter_ = new ChaCha20Decrypter();

	if (encrypter_->setKey(key) == false ||
		decrypter_->setKey(key) == false)
	{
		return false;
	}

	return true;
}

void CryptoMakerSocketTask::onEnd(TaskExecuter* executer)
{
	//(executer);
	session_->onConnected(isSucceed(), socket_,
		std::make_shared<SendSocketTask>(session_, socket_, 4 * 1024, encrypter_),
		std::make_shared<ReceiveSocketTask>(session_, socket_, 64 * 1024, decrypter_));
}



ReceiveSocketTask::ReceiveSocketTask(std::shared_ptr<Session> session, FSocket* socket, int32_t bufferSize, ChaCha20Decrypter* decrypter)
	: session_(session)
	, socket_(socket)
	, buffer_(bufferSize)
	, decrypter_(decrypter)
{
}

ReceiveSocketTask::~ReceiveSocketTask()
{
	delete decrypter_;
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

	int32_t recvSize = 0;
	if (socket_->Recv(buffer_.getEndBuffer(), buffer_.space(), recvSize) == false)
	{
		return false;
	}

	buffer_.moveCurPos(recvSize);
	return true;
}

void ReceiveSocketTask::onEnd(TaskExecuter* executer)
{
	//(executer);
	if (isSucceed() == false)
	{
		session_->onError(socket_);
		return;
	}

	std::vector<Packet*> packets;
	while (true)
	{
		Packet* packet = nullptr;
		if (PopPacket(&buffer_, decrypter_, packet) == false)
		{
			session_->onError(socket_);
			break;
		}

		if (packet == nullptr)
		{
			break;
		}

		if (packet->header_.crc32 != 0 &&
			packet->header_.crc32 != packet->calcCrc32())
		{
			session_->onError(socket_);
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
		executer->push(shared_from_this());
	}
}



SendSocketTask::SendSocketTask(std::shared_ptr<Session> session, FSocket* socket, int32_t bufferSize, ChaCha20Encrypter* encrypter)
	: session_(session)
	, socket_(socket)
	, buffer_(bufferSize)
	, encrypter_(encrypter)
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
			GetBufferPool()->Delete(buffer);
		}
		request_.clear();
		isSending_ = true;
	}

	{
		for (auto buffer : pending_)
		{
			GetBufferPool()->Delete(buffer);
		}
		pending_.clear();
		buffer_.clear();
	}

	delete encrypter_;
	encrypter_ = nullptr;
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
			pending_.pop_front();

			bool result = onReadyToSend(buffer);
			GetBufferPool()->Delete(buffer);

			if (result == false)
			{
				result_ = false;
				return true;
			}

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

bool SendSocketTask::onReadyToSend(Buffer* buffer)
{
	uint32_t remainLen = buffer->getSize();
	uint8_t* streamBuf = buffer->getBuffer();

	while (remainLen != 0)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(streamBuf);
		if (header->size < sizeof(PacketHeader))
		{
			return false;
		}

		if (header->size > remainLen)
		{
			return false;
		}

		if (buffer_.makeSpace(header->size) < static_cast<int32_t>(header->size))
		{
			return false;
		}

		const void* data = streamBuf + sizeof(PacketHeader);
		uint32_t dataSize = header->size - sizeof(PacketHeader);
		header->crc32 = Crc32::Compute(data, dataSize);

		buffer_.copy(header, sizeof(*header));

		if (encrypter_->encrypt(data, dataSize, buffer_.getEndBuffer(), buffer_.space()) != dataSize)
		{
			return false;
		}

		buffer_.moveCurPos(dataSize);

		streamBuf += header->size;
		remainLen -= header->size;
	}

	return true;
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
	//(executer);
	socket_->Close();
	return true;
}

void DisconnectSocketTask::onEnd(TaskExecuter* executer)
{
	//(executer);
	session_->onDisconnected(socket_);
}

