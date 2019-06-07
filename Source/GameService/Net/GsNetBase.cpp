#include "GsNetBase.h"
#include "./flatbuffers/flatbuffers.h"
#include "../UTIL/GsText.h"

// connection 처리를 한다.
void FGsNetBase::Enter()
{
	Connect(_connectionInfo._addr, _connectionInfo._port);
};

// disconnection 처리를 한다.
void FGsNetBase::Exit()
{
	Disconnct();
};

void FGsNetBase::Update()
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
		_protocol = (int)packet->header_.packetId;
		_handler.SendMessage(_protocol, packet);
		GetPacketPool()->Delete(packet);
	}

	packets.clear();

};

//세션 객체 생성처리
void FGsNetBase::InitConnection(FGsNet::NetConnectionData&& indata)
{
	_connectionInfo = std::move(indata);

	session_ = std::make_shared<Session>(this);
}

bool FGsNetBase::Connect(FString inAddr, uint16 inPort)
{
	if (nullptr == session_)
		return false;

	return session_->connect(TOstring(inAddr).c_str(), inPort);
}

void FGsNetBase::Disconnct()
{
	if (nullptr == session_)
		return;

	session_->disconnect();
}

//FBBuilder fbBuilder;
//auto offset = StoS::CreateAccountValidationRequest(fbBuilder.get(), accountDbId);
//fbBuilder.get().Finish(offset);
void FGsNetBase::Send(LeanPacket::Protocol inProtocol, FBBuilder& inbuilder)
{
	if (nullptr == session_)
		return;

	session_->send((uint16_t)inProtocol, inbuilder.getBuffer(), inbuilder.size());
}

