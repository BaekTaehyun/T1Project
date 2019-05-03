#pragma once

#include "CoreMinimal.h"

template<typename OnMessage, typename Argu>
class GSTMessageDispatcher
{
private:
	OnMessage	_event;
public:
	DECLARE_EVENT_OneParam(GSTMessageDispatcher, OnMessage, Argu*);
	OnMessage& OnMessaged() { return _event; }
};