#pragma once
#include "GSFNetBase.h"

class GSFNetLobby : public GSFNetBase, SessionEventHandler
{
public:
	GSFNetLobby() {};
	virtual ~GSFNetLobby() {}

	// SessionEventHandler
	virtual void onConnected(bool result, std::shared_ptr<Session> session) override;
	virtual void onDisconnected(std::shared_ptr<Session> session) override;
	virtual void onError(std::shared_ptr<Session> session) override;
};