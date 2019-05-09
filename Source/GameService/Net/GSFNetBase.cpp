#include "GSFNetBase.h"
#include "./flatbuffers/flatbuffers.h"
#include "../UTIL/Text.h"

// connection 처리를 한다.
void GSFNetModeBase::Enter()
{
	Connect(_connectionInfo._addr, _connectionInfo._port);
};

// disconnection 처리를 한다.
void GSFNetModeBase::Exit()
{
	Disconnct();
};

void GSFNetModeBase::Update()
{
	// 주의! processIOEvent()에서 session_ 포인터를 삭제할 수 있으니
		// 여기서 session을 복사해서 사용하자.
		// Session::processIOEvent()에서 disconnect를 처리할 경우
		// Client::onDisconnected()에서 session_ = nullptr; 로 처리되어 발생한다.
	std::shared_ptr<Session> session = session_;
	if (session == nullptr)
	{
		return;
	}

	session->processIOEvent();

	auto packets = session->popAll();
	for (auto packet : packets)
	{
		assert(packet->header_.packetId == 1);

		delete packet;
	}

	packets.clear();
};

//세션 객체 생성처리
void GSFNetModeBase::Init(GSFNet::NetConnectionData&& indata)
{
	_connectionInfo = std::move(indata);
	session_ = std::make_shared<Session>(this);
}

bool GSFNetModeBase::Connect(FString inAddr, uint16 inPort)
{
	if (nullptr == session_) return false;
	return session_->connect(TOstring(inAddr).c_str(), inPort);
}

void GSFNetModeBase::Disconnct()
{
	if (nullptr == session_) return;
	session_->disconnect();
}

void GSFNetModeBase::Send(LeanPacket::Protocol inProtocol, FlatBufferBuilder& inbuilder)
{

}

// SessionEventHandler
void GSFNetModeBase::onConnected(bool result, std::shared_ptr<Session> session)
{
	
}

void GSFNetModeBase::onDisconnected(std::shared_ptr<Session> session)
{

}

void GSFNetModeBase::onError(std::shared_ptr<Session> session)
{

}