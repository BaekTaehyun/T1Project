#include "GsNetBase.h"
#include "./flatbuffers/flatbuffers.h"

#include "../UTIL/GsText.h"

// connection ó���� �Ѵ�.
void FGsNetBase::Enter()
{
	Connect(_connectionInfo._addr, _connectionInfo._port);
};

// disconnection ó���� �Ѵ�.
void FGsNetBase::Exit()
{
	Disconnct();
};

void FGsNetBase::Update()
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
void FGsNetBase::Init(FGsNet::NetConnectionData&& indata)
{
	_connectionInfo = std::move(indata);

	session_ = std::make_shared<Session>(this);
}

bool FGsNetBase::Connect(FString inAddr, uint16 inPort)
{
	if (nullptr == session_) return false;
	return session_->connect(TOstring(inAddr).c_str(), inPort);
}

void FGsNetBase::Disconnct()
{
	if (nullptr == session_) return;
	session_->disconnect();
}

void FGsNetBase::Send(LeanPacket::Protocol inProtocol, FlatBufferBuilder& inbuilder)
{

}
