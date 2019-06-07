#pragma once

#include "GsNet.h"
#include "GsFBAllocator.h"
#include "../Class/GsState.h"
#include "./Network/SocketConfig.h"
#include "./Network/Session.h"
#include "../class/GsMessageHandler.h"


//------------------------------------------------------------------------------
// 네트워킹을 하기위한 최소단위
//------------------------------------------------------------------------------
class FGsNetBase : public TGsState<FGsNet::Mode>, public SessionEventHandler
{
	FGsNet::NetConnectionData _connectionInfo;
	std::shared_ptr<Session> session_;  // 서버 구조에 맞춤
	TGsMessageHandlerOneParam <int, Packet*> _handler;
	int _protocol;
public:
	FGsNetBase() : TGsState<FGsNet::Mode>(FGsNet::Mode::MAX) {}
	FGsNetBase(FGsNet::Mode inMode) : TGsState<FGsNet::Mode>(inMode) {}
	virtual ~FGsNetBase() {}

	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;

	void InitConnection(FGsNet::NetConnectionData&& indata);
	void Send(LeanPacket::Protocol inProtocol, class FBBuilder& inbuilder);

protected:
	bool Connect(FString inAddr, uint16 inPort = 8888);
	void Disconnct();

	virtual void onConnected(bool result, std::shared_ptr<Session> session) override {};
	virtual void onDisconnected(std::shared_ptr<Session> session) override {};
	virtual void onError(std::shared_ptr<Session> session) override {};
};