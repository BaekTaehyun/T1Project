#include "GSFNetLobby.h"
#include "../Message/GSFMessageNet.h"
#include "../Message/GSFMessageManager.h"

// SessionEventHandler
void GSFNetLobby::onConnected(bool result, std::shared_ptr<Session> session)
{
	GMessage()->GetNet().SendMessage(MessageNet::ID::LOGIN_SERVER_CONNECTION_SUCCESS, GSFMessageNone());
}

void GSFNetLobby::onDisconnected(std::shared_ptr<Session> session)
{
	GMessage()->GetNet().SendMessage(MessageNet::ID::LOGIN_SERVER_DISCONNECTION, GSFMessageNone());
}

void GSFNetLobby::onError(std::shared_ptr<Session> session)
{
	GMessage()->GetNet().SendMessage(MessageNet::ID::LOGIN_SERVER_CONNECTION_FAIL, GSFMessageNone());
}