#pragma once

#include "../Class/GSTMessage.h"
#include "../Class/GSTMessageHandler.h"
#include "../Class/GSTSingleton.h"
#include "../Class/GSIManager.h"

#include "GSFMessageNet.h"
#include "GSFMessageSystem.h"
#include "GSFMessageLobby.h"
#include "GSFMessageGame.h"


class GSFMessageManager : GSTSingleton<GSFMessageManager>, public GSIManager
{
	typedef GSTMessageHandler<MessageSystem::ID, GSFMessageNone>	MSystem;
	typedef GSTMessageHandler<MessageNet::ID, GSFMessageNone>		MNet;
	typedef GSTMessageHandler<MessageLobby::Hive, GSFMessageNone>	MHive;
	typedef GSTMessageHandler<MessageLobby::Stage, GSFMessageNone>	MStage;
	
	MSystem	_system;
	MNet	_net;
	MHive	_hive;
	MStage  _stage;

public:
	GSFMessageManager() {};
	virtual ~GSFMessageManager();

	//GSIManager
	virtual void Initialize() override;
	virtual void Finalize() override;

	MSystem& GetSystem() { return _system; }
	MNet& GetNet() { return _net; }
	MHive& GetHive() { return _hive; }
	MStage& GetStage() { return _stage; }
};

typedef GSTSingleton<GSFMessageManager>	GSFMessageSingle;
