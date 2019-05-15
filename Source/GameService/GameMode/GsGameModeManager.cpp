#include "GsGameModeManager.h"
#include "GsGameModeLobby.h"
#include "GsGameModeGame.h"
#include "../Message/GsMessageManager.h"

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
FGsGameModeBase* FGsGameModeAllocator::Alloc(FGsGameMode::Mode inMode)
{
	if (inMode == FGsGameMode::Mode::LOBBY)
	{
		return new FGsGameModeLobby();
	}
	else if (inMode == FGsGameMode::Mode::GAME)
	{
		return new FGsGameModeGame();
	}
	return NULL;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
FGsGameModeManager* FGsGameModeSingle::Instance = NULL;

FGsGameModeManager::~FGsGameModeManager()
{
	int a = 0;
}

void FGsGameModeManager::Initialize()
{
	TGsSingleton::InitInstance(this);

	constexpr std::initializer_list<FGsGameMode::Mode> allMode = { FGsGameMode::Mode::LOBBY, FGsGameMode::Mode::GAME };
	for (auto& e : allMode)
	{
		MakeInstance(e);
	}

	ChangeState(FGsGameMode::Mode::LOBBY);

	Super::InitState();


	//// 메시지 등록예 1)
	//GSTMessageHandler<MessageSystem::ID, GSFMessageNone>::MessageType delFunc;
	//auto handle = delFunc.AddRaw(this, &GSFGameModeManager::OnReconnectionStart);
	//_messagehandler.GetDeliveryAddress().Add(MessageSystem::ID::RECONNECT_START, delFunc);

	//// 메시지 등록 2)
	GSCHECK(GSFMessageSingle::Instance);
	GSFMessageSingle::Instance->GetSystem().AddRaw(MessageSystem::ID::RECONNECT_END, this, &FGsGameModeManager::OnReconnectionEnd);
	//_messagehandler.

	//// 메시지 호출 1)
	//GSTMessage<MessageSystem::ID, GSFMessageNone> Reconect_Start(MessageSystem::ID::RECONNECT_START, GTSMessageNone());
	//_messagehandler.SendMessage(Reconect_Start);
	//_messagehandler.SendMessageC(GSTMessage<MessageSystem::ID, GSFMessageNone>(MessageSystem::ID::RECONNECT_END, GTSMessageNone()));

	//// 메시지 호출 2)
	GSFMessageSingle::Instance->GetSystem().SendMessage(MessageSystem::ID::RECONNECT_END);
}

void FGsGameModeManager::Finalize()
{
	Super::RemoveAll();
	TGsSingleton::RemoveInstance();
}

void FGsGameModeManager::OnReconnectionStart()
{
	GSLOG(Warning, TEXT("FGsGameModeManager : OnReconnectionStart"));

	GetState()->OnReconnectionStart();
}

void FGsGameModeManager::OnReconnectionEnd()
{
	GSLOG(Warning, TEXT("FGsGameModeManager : OnReconnectionStart"));
	GetState()->OnReconnectionEnd();
}