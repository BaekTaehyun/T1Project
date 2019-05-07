#pragma once
#include "../Class/GSTMessageHandler.h"
#include "MessageSystem.h"

class GSFMessageHandlerSystem : public GSTMessageHandler<MessageSystem::ID, GTSMessageNone>
{
public:
	GSFMessageHandlerSystem() {};
	virtual ~GSFMessageHandlerSystem();

	virtual void InsertMessage() override;
};
