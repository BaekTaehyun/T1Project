#pragma once

#include "../Class/GSTMessage.h"
#include "../Class/GSTMessageHandler.h"
#include "../Class/GSTSingleton.h"

#include "GSFMessageNet.h"
#include "GSFMessageSystem.h"
#include "GSFMessageLobby.h"
#include "GSFMessageGame.h"


class GSFMessageManager : GSTSingleton<GSFMessageManager>
{
	typedef GSTMessageHandler<MessageSystem::ID, GTSMessageNone>	MSystem;
	typedef GSTMessageHandler<MessageNet::ID, GTSMessageNone>		MNet;
	typedef GSTMessageHandler<MessageLobby::Hive, GTSMessageNone>	MHive;
	typedef GSTMessageHandler<MessageLobby::Stage, GTSMessageNone>	MStage;
	
	MSystem	_system;
	MNet	_net;
	MHive	_hive;
	MStage  _stage;

public:
	GSFMessageManager() {};
	virtual ~GSFMessageManager() {};

	void Init();
	void RemoveAll();

	MSystem& GetSystem() { return _system; }
	MNet& GetNet() { return _net; }
	MHive& GetHive() { return _hive; }
	MStage& GetStage() { return _stage; }
};

typedef GSTSingleton<GSFMessageManager>	GSFMessageSingle;
