#include "GSFNetBase.h"
#include "./flatbuffers/flatbuffers.h"
#include "../UTIL/Text.h"

// connection ó���� �Ѵ�.
void GSFNetModeBase::Enter()
{
	Connect(_connectionInfo._addr, _connectionInfo._port);
};

// disconnection ó���� �Ѵ�.
void GSFNetModeBase::Exit()
{
	Disconnct();
};

void GSFNetModeBase::Update()
{
	// ����! processIOEvent()���� session_ �����͸� ������ �� ������
		// ���⼭ session�� �����ؼ� �������.
		// Session::processIOEvent()���� disconnect�� ó���� ���
		// Client::onDisconnected()���� session_ = nullptr; �� ó���Ǿ� �߻��Ѵ�.
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

//���� ��ü ����ó��
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