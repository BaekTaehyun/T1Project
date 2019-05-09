#pragma once
#include "GSFNetBase.h"

class GSFNetGame : public GSFNetBase, public SessionEventHandler
{
public:
	GSFNetGame() {};
	virtual ~GSFNetGame() {}

	// SessionEventHandler
	virtual void onConnected(bool result, std::shared_ptr<Session> session) override;
	virtual void onDisconnected(std::shared_ptr<Session> session) override;
	virtual void onError(std::shared_ptr<Session> session) override;
};