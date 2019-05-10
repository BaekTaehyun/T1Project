#include "GSFNetGame.h"
#include "../Message/GSFMessageNet.h"
#include "../Message/GSFMessageManager.h"

// SessionEventHandler
void GSFNetGame::onConnected(bool result, std::shared_ptr<Session> session)
{
	GMessage()->GetNet().SendMessage(MessageNet::ID::GAME_SERVER_CONNECTION_SUCCESS, GSFMessageNone());
}

void GSFNetGame::onDisconnected(std::shared_ptr<Session> session)
{
	GMessage()->GetNet().SendMessage(MessageNet::ID::GAME_SERVER_DISCONNECTION, GSFMessageNone());
}

void GSFNetGame::onError(std::shared_ptr<Session> session)
{
	GMessage()->GetNet().SendMessage(MessageNet::ID::GAME_SERVER_CONNECTION_FAIL, GSFMessageNone());
}