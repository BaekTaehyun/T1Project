#pragma once

#include "../Class/GsMessage.h"
#include "../Class/GsMessageHandler.h"
#include "../Class/GsSingleton.h"
#include "../Class/GsManager.h"

#include "GsMessageNet.h"
#include "GsMessageSystem.h"
#include "GsMessageLobby.h"
#include "GsMessageGame.h"


class FGsMessageManager : 
	public TGsSingleton<FGsMessageManager>,
	public IGsManager
{
	typedef TGsMessageHandler<MessageSystem::ID>	MSystem;
	typedef TGsMessageHandler<MessageNet::ID>		MNet;
	typedef TGsMessageHandler<MessageLobby::Hive>	MHive;
	typedef TGsMessageHandler<MessageLobby::Stage>	MStage;
	
	MSystem	_system;
	MNet	_net;
	MHive	_hive;
	MStage  _stage;

public:
	FGsMessageManager() {};
	virtual ~FGsMessageManager();

	//IGsManager
	virtual void Initialize() override;
	virtual void Finalize() override;
	virtual void Update() override;

	MSystem& GetSystem() { return _system; }
	MNet& GetNet() { return _net; }
	MHive& GetHive() { return _hive; }
	MStage& GetStage() { return _stage; }
};

typedef TGsSingleton<FGsMessageManager>	FGsMessageSingle;
#define GMessage() FGsMessageSingle::Instance
