#include "GsGameFlowManager.h"
#include "GsGameFlowLobby.h"
#include "GsGameFlowGame.h"
#include "../Message/GsMessageManager.h"

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
FGsGameFlowBase* FGsGameFlowAllocator::Alloc(FGsGameFlow::Mode inMode)
{
	if (inMode == FGsGameFlow::Mode::LOBBY)
	{
		return new FGsGameFlowLobby();
	}
	else if (inMode == FGsGameFlow::Mode::GAME)
	{
		return new FGsGameFlowGame();
	}
	return NULL;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
FGsGameFlowManager* FGsGameFlowSingle::Instance = NULL;

FGsGameFlowManager::~FGsGameFlowManager()
{
	int a = 0;
}

void FGsGameFlowManager::Initialize()
{
	TGsSingleton::InitInstance(this);

	constexpr std::initializer_list<FGsGameFlow::Mode> allMode = { FGsGameFlow::Mode::LOBBY, FGsGameFlow::Mode::GAME };
	for (auto& e : allMode)
	{
		auto inst = MakeInstance(e);
		inst->Init();
	}

	ChangeState(FGsGameFlow::Mode::LOBBY);

	Super::InitState();


	//// 메시지 등록예 1)
	//GSTMessageHandler<MessageSystem::ID, GSFMessageNone>::MessageType delFunc;
	//auto handle = delFunc.AddRaw(this, &GSFGameModeManager::OnReconnectionStart);
	//_messagehandler.GetDeliveryAddress().Add(MessageSystem::ID::RECONNECT_START, delFunc);

	//// 메시지 등록 2)
	GSCHECK(GMessage());
	GMessage()->GetSystem().AddRaw(MessageSystem::ID::RECONNECT_END, this, &FGsGameFlowManager::OnReconnectionEnd);
	//_messagehandler.

	//// 메시지 호출 1)
	//GSTMessage<MessageSystem::ID, GSFMessageNone> Reconect_Start(MessageSystem::ID::RECONNECT_START, GTSMessageNone());
	//_messagehandler.SendMessage(Reconect_Start);
	//_messagehandler.SendMessageC(GSTMessage<MessageSystem::ID, GSFMessageNone>(MessageSystem::ID::RECONNECT_END, GTSMessageNone()));

	//// 메시지 호출 2)
	GMessage()->GetSystem().SendMessage(MessageSystem::ID::RECONNECT_END);
}

void FGsGameFlowManager::Finalize()
{
	Super::RemoveAll();
	TGsSingleton::RemoveInstance();
}

void FGsGameFlowManager::OnReconnectionStart()
{
	GSLOG(Warning, TEXT("FGsGameFlowManager : OnReconnectionStart"));

	GetState()->OnReconnectionStart();
}

void FGsGameFlowManager::OnReconnectionEnd()
{
	GSLOG(Warning, TEXT("FGsGameFlowManager : OnReconnectionStart"));
	GetState()->OnReconnectionEnd();
}