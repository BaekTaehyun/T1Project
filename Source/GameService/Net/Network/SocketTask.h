#pragma once


#include <memory>
#include <string>
#include <deque>
#include <mutex>

#include "SocketHelper.h"
#include "Task.h"
#include "Buffer.h"


class Session;


class ConnectSocketTask : public Task
{
	std::shared_ptr<Session> session_;
	FSocket* socket_ = nullptr;

	std::string host_;
	uint16_t port_;

public:

	ConnectSocketTask(std::shared_ptr<Session> session, const char* host, uint16_t port);

private:

	bool onExecute(TaskExecuter* executer) override;
	void onEnd(TaskExecuter* executer) override;

};

class CheckConnectSocketTask : public Task
{
	std::shared_ptr<Session> session_;
	FSocket* socket_ = nullptr;

public:

	CheckConnectSocketTask(std::shared_ptr<Session> session, FSocket* socket);

private:

	bool onExecute(TaskExecuter* executer) override;
	void onEnd(TaskExecuter* executer) override;

};

class ReceiveSocketTask : public Task
{
	std::shared_ptr<Session> session_;
	FSocket* socket_ = nullptr;

	Buffer buffer_;

public:

	ReceiveSocketTask(std::shared_ptr<Session> session, FSocket* socket, int32_t bufferSize);

private:

	bool onExecute(TaskExecuter* executer) override;
	void onEnd(TaskExecuter* executer) override;

	bool onReceive();

};

class SendSocketTask : public Task
{
	const int32_t MAX_SEND_BUFFER = 64 * 1024; // 패킷을 복사하다 보면 넘어 갈 수 있다.
	const size_t LIMIT_SEND_QUEUE_COUNT = 5 * 10; // sendQueue 최대 제한(1초에 5번 보내고 10초 대기 기준)

	std::shared_ptr<Session> session_;
	FSocket* socket_ = nullptr;

	std::mutex lock_;
	std::deque<Buffer*> request_;
	bool isSending_ = false;

	std::deque<Buffer*> pending_;
	Buffer buffer_;

public:

	SendSocketTask(std::shared_ptr<Session> session, FSocket* socket, int32_t bufferSize);
	~SendSocketTask();

	bool send(uint16_t packetId, const void* data, int32_t size);

	bool isComplete() const;

	void clear();

private:

	bool onExecute(TaskExecuter* executer) override;
	void onEnd(TaskExecuter* executer) override;

	void onReadyToSend(Buffer* buffer);
	bool onSend();

};


class DisconnectSocketTask : public Task
{
	std::shared_ptr<Session> session_;
	FSocket* socket_ = nullptr;

public:

	DisconnectSocketTask(std::shared_ptr<Session> session, FSocket* socket);

private:

	bool onExecute(TaskExecuter* executer) override;
	void onEnd(TaskExecuter* executer) override;

};
