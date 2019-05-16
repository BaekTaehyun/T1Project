#pragma once

#include "GsNet.h"
#include "../Class/GsState.h"

#include "./Network/SocketConfig.h"
#include "./Network/Session.h"
#include "LeanPacket_generated.h"


//------------------------------------------------------------------------------
// ��Ʈ��ŷ�� �ϱ����� �ּҴ���
//------------------------------------------------------------------------------
class FGsNetBase : public TGsState<FGsNet::Mode>, public SessionEventHandler
{
	FGsNet::NetConnectionData _connectionInfo;
	std::shared_ptr<Session> session_;  // ���� ������ ����

public:
	FGsNetBase() : TGsState<FGsNet::Mode>(FGsNet::Mode::MAX) {}
	FGsNetBase(FGsNet::Mode inMode) : TGsState<FGsNet::Mode>(inMode) {}
	virtual ~FGsNetBase() {}

	virtual void Enter();
	virtual void Exit();
	virtual void Update();

	void Init(FGsNet::NetConnectionData&& indata);
	void Send(LeanPacket::Protocol inProtocol, class FlatBufferBuilder& inbuilder);

protected:
	bool Connect(FString inAddr, uint16 inPort = 8888);
	void Disconnct();

	virtual void onConnected(bool result, std::shared_ptr<Session> session) override {};
	virtual void onDisconnected(std::shared_ptr<Session> session) override {};
	virtual void onError(std::shared_ptr<Session> session) override {};
};