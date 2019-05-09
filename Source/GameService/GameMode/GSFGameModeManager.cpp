#include "GSFGameModeManager.h"
#include "GSFGameModeLobby.h"
#include "GSFGameModeGame.h"
#include "../Message/GSFMessageManager.h"

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
GSFGameModeBase* GSFGameModeAllocator::Alloc(GSFGameMode::Mode inMode)
{
	if (inMode == GSFGameMode::Mode::LOBBY)
	{
		return new GSFGameModeLobby();
	}
	else if (inMode == GSFGameMode::Mode::GAME)
	{
		return new GSFGameModeGame();
	}
	return NULL;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
GSFGameModeManager* GSFGameModeSingle::Instance = NULL;

GSFGameModeManager::~GSFGameModeManager()
{
	int a = 0;
}

void GSFGameModeManager::Initialize()
{
	GSTSingleton::InitInstance(this);

	constexpr std::initializer_list<GSFGameMode::Mode> allMode = { GSFGameMode::Mode::LOBBY, GSFGameMode::Mode::GAME };
	for (auto& e : allMode)
	{
		MakeInstance(e);
	}

	ChangeState(GSFGameMode::Mode::LOBBY);

	Super::InitState();


	//// 메시지 등록예 1)
	//GSTMessageHandler<MessageSystem::ID, GSFMessageNone>::MessageType delFunc;
	//auto handle = delFunc.AddRaw(this, &GSFGameModeManager::OnReconnectionStart);
	//_messagehandler.GetDeliveryAddress().Add(MessageSystem::ID::RECONNECT_START, delFunc);

	//// 메시지 등록 2)
	GSCHECK(GSFMessageSingle::Instance);
	GSFMessageSingle::Instance->GetSystem().AddRaw(MessageSystem::ID::RECONNECT_END, this, &GSFGameModeManager::OnReconnectionEnd);
	//_messagehandler.

	//// 메시지 호출 1)
	//GSTMessage<MessageSystem::ID, GSFMessageNone> Reconect_Start(MessageSystem::ID::RECONNECT_START, GTSMessageNone());
	//_messagehandler.SendMessage(Reconect_Start);
	//_messagehandler.SendMessageC(GSTMessage<MessageSystem::ID, GSFMessageNone>(MessageSystem::ID::RECONNECT_END, GTSMessageNone()));

	//// 메시지 호출 2)
	GSFMessageSingle::Instance->GetSystem().SendMessage(MessageSystem::ID::RECONNECT_END, GSFMessageNone());
}

void GSFGameModeManager::Finalize()
{
	Super::RemoveAll();
	GSTSingleton::RemoveInstance();
}

void GSFGameModeManager::OnReconnectionStart(const GSFMessageNone& message)
{
	GSLOG(Warning, TEXT("GSFGameModeManager : OnReconnectionStart"));

	GetState()->OnReconnectionStart();
}

void GSFGameModeManager::OnReconnectionEnd(const GSFMessageNone& message)
{
	GSLOG(Warning, TEXT("GSFGameModeManager : OnReconnectionStart"));
	GetState()->OnReconnectionEnd();
}