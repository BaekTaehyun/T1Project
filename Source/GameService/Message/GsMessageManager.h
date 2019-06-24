#pragma once

#include "../Class/GsMessage.h"
#include "../Class/GsMessageHandler.h"
#include "../Class/GsSingleton.h"
#include "../Class/GsManager.h"

#include "GsMessageNet.h"
#include "GsMessageSystem.h"
#include "GsMessageLobby.h"
#include "GsMessageGame.h"
#include "GsMessageItem.h"
#include "GsMessageGameObject.h"
#include "../GameObject/Item/CItem.h"


class FGsMessageManager : 
	public TGsSingleton<FGsMessageManager>,
	public IGsManager
{
	typedef TGsMessageHandler<MessageSystem::ID>	MSystem;
	typedef TGsMessageHandler<MessageNet::ID>		MNet;
	typedef TGsMessageHandler<MessageLobby::Hive>	MHive;
	typedef TGsMessageHandler<MessageLobby::Stage>	MStage;
	typedef TGsMessageHandlerOneParam<MessageItem::ItemAction , UCItem&> 	MItem;
	//테스트 Gameobject 이벤트 처리용 메세지
	typedef TGsMessageHandlerOneParam<MessageGameObject::Action, class GsGameObjectEventParamBase&> MGameObject;
	
	MSystem	_system;
	MNet	_net;
	MHive	_hive;
	MStage  _stage;
	MGameObject _go;
	MItem _Item;

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
	MGameObject& GetGo() { return _go; }
	MItem& GetItem() { return _Item; }
};

typedef TGsSingleton<FGsMessageManager>	FGsMessageSingle;
#define GMessage() FGsMessageSingle::Instance()
