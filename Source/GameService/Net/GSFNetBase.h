#pragma once

#include "GSFNet.h"
#include "../Class/GSTState.h"

#include "./Network/SocketConfig.h"
#include "./Network/Session.h"
#include "LeanPacket_generated.h"


//------------------------------------------------------------------------------
// 네트워킹을 하기위한 최소단위
//------------------------------------------------------------------------------
class GSFNetBase : public GSTState<GSFNet::Mode>, public SessionEventHandler
{
	GSFNet::NetConnectionData _connectionInfo;
	std::shared_ptr<Session> session_;  // 서버 구조에 맞춤

public:
	GSFNetBase() : GSTState<GSFNet::Mode>(GSFNet::Mode::MAX) {}
	GSFNetBase(GSFNet::Mode inMode) : GSTState<GSFNet::Mode>(inMode) {}
	virtual ~GSFNetBase() {}

	virtual void Enter();
	virtual void Exit();
	virtual void Update();

	void Init(GSFNet::NetConnectionData&& indata);
	void Send(LeanPacket::Protocol inProtocol, class FlatBufferBuilder& inbuilder);

protected:
	bool Connect(FString inAddr, uint16 inPort = 8888);
	void Disconnct();

	virtual void onConnected(bool result, std::shared_ptr<Session> session) override {};
	virtual void onDisconnected(std::shared_ptr<Session> session) override {};
	virtual void onError(std::shared_ptr<Session> session) override {};
};