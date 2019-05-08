#include "GSFGameModeManager.h"

#include "GSFGameModeLobby.h"
#include "GSFGameModeGame.h"
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
void GSFGameModeManager::RemoveAll()
{
	Super::RemoveAll();
}
void GSFGameModeManager::InitState()
{
	constexpr std::initializer_list<GSFGameMode::Mode> allMode = { GSFGameMode::Mode::LOBBY, GSFGameMode::Mode::GAME };
	for (auto& e : allMode)
	{
		MakeInstance(e);
	}

	ChangeState(GSFGameMode::Mode::LOBBY);

	// super::InitState() 동일코드
	Super::InitState();


	// 메시지 등록
	GSTMessageHandler<MessageSystem::ID, GTSMessageNone>::MessageType delFunc;
	auto handle = delFunc.AddRaw(this, &GSFGameModeManager::OnReconnectionStart);
	_messagehandler.GetDeliveryAddress().Add(MessageSystem::ID::RECONNECT_START, delFunc);


	_messagehandler.AddRaw(MessageSystem::ID::RECONNECT_END, this, &GSFGameModeManager::OnReconnectionEnd);
	//essagehandler.GetDeliveryAddress().Add(, delFunc);

	GSTMessage<MessageSystem::ID, GTSMessageNone> Reconect_Start(MessageSystem::ID::RECONNECT_START, GTSMessageNone());
	_messagehandler.SendMessage(Reconect_Start);
	_messagehandler.SendMessageC(GSTMessage<MessageSystem::ID, GTSMessageNone>(MessageSystem::ID::RECONNECT_END, GTSMessageNone()));

	_messagehandler.SendMessage(MessageSystem::ID::RECONNECT_END, GTSMessageNone());
}

void GSFGameModeManager::OnReconnectionStart(const GTSMessageNone& message)
{
	GSLOG(Warning, TEXT("GSFGameModeManager : OnReconnectionStart"));

	GetState().Get().OnReconnectionStart();
}

void GSFGameModeManager::OnReconnectionEnd(const GTSMessageNone& message)
{
	GSLOG(Warning, TEXT("GSFGameModeManager : OnReconnectionStart"));
	GetState().Get().OnReconnectionEnd();
}