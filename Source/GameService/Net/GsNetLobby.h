#pragma once
#include "GsNetBase.h"

class FGsNetLobby : public FGsNetBase
{
public:
	FGsNetLobby() {};
	virtual ~FGsNetLobby() {}

	// SessionEventHandler
	virtual void onConnected(bool result, std::shared_ptr<Session> session) override;
	virtual void onDisconnected(std::shared_ptr<Session> session) override;
	virtual void onError(std::shared_ptr<Session> session) override;
};