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
	GSTStateMng<GSFGameMode::Mode, GSFGameModeBase, GSFGameModeAllocator>::RemoveAll();
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
	GSTStateMng<GSFGameMode::Mode, GSFGameModeBase, GSFGameModeAllocator>::InitState();


	// 메시지 등록
	GSTMessageHandler<MessageSystem::ID, GTSMessageNone>::MessageType delFunc;
	delFunc.AddRaw(this, &GSFGameModeManager::OnReconnectionStart);
	_messagehandler.GetDeliveryAddress().Add(MessageSystem::ID::RECONNECT_START, delFunc);

}

void GSFGameModeManager::OnReconnectionStart(const GTSMessageNone& message)
{

}

void GSFGameModeManager::OnReconnectionEnd(const GTSMessageNone& message)
{

}