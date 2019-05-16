#include "GsNetGame.h"
#include "../Message/GsMessageNet.h"
#include "../Message/GsMessageManager.h"

// SessionEventHandler
void FGsNetGame::onConnected(bool result, std::shared_ptr<Session> session)
{
	GMessage()->GetNet().SendMessageAsync(MessageNet::ID::GAME_SERVER_CONNECTION_SUCCESS);
}

void FGsNetGame::onDisconnected(std::shared_ptr<Session> session)
{
	GMessage()->GetNet().SendMessageAsync(MessageNet::ID::GAME_SERVER_DISCONNECTION);
}

void FGsNetGame::onError(std::shared_ptr<Session> session)
{
	GMessage()->GetNet().SendMessageAsync(MessageNet::ID::GAME_SERVER_CONNECTION_FAIL);
}