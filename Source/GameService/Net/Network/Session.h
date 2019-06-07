#pragma once


#include <cstdint>
#include <vector>
#include <mutex>
#include <atomic>

#include "SocketHelper.h"
#include "SocketTask.h"
#include "Protocol.h"



class Session;
class SessionEventHandler
{
public:

	virtual ~SessionEventHandler() {}

	virtual void onConnected(bool result, std::shared_ptr<Session> session) = 0;
	virtual void onDisconnected(std::shared_ptr<Session> session) = 0;
	virtual void onError(std::shared_ptr<Session> session) = 0;

};



class Session : public std::enable_shared_from_this<Session>
{
	enum class IOEventType
	{
		Connected,
		Disconnected,
		Error,
	};

	struct IOEventArg
	{
		IOEventType type;
		bool result = false;

		IOEventArg(IOEventType t, bool r)
			: type(t)
			, result(r)
		{
		}
	};


	FSocket* socket_ = nullptr;
	std::atomic_bool connected_{ false };

	SessionEventHandler* handler_ = nullptr;
	std::mutex eventLock_;
	std::vector<IOEventArg> ioEventArgs_;

	std::shared_ptr<SendSocketTask> sendTask_;

	std::mutex lock_;
	std::vector<Packet*> packets_;
public:

	explicit Session(SessionEventHandler* handler);
	~Session();

	FSocket* getSocket() const { return socket_; }
	void setHandler(SessionEventHandler* handler) { handler_ = handler; }

	bool connect(const char* host, uint16_t port);
	void disconnect();

	bool isConnected() const;

	void processIOEvent();

	std::vector<Packet*> popAll();
	bool send(uint16_t packetId, const void* data, int32_t size);

public:

	void onConnected(bool result, FSocket* socket, std::shared_ptr<SendSocketTask> sender, std::shared_ptr<ReceiveSocketTask> receiver);
	void onPacket(FSocket* socket, std::vector<Packet*> packets);
	void onError(FSocket* socket);
	void onDisconnected(FSocket* socket);

private:

	void start(FSocket* socket, std::shared_ptr<SendSocketTask> sender, std::shared_ptr<ReceiveSocketTask> receiver);
	void clear();

	void pushEvent(IOEventType type, bool result);

};
