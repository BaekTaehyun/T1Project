#include "GsNetLobby.h"
#include "../Message/GsMessageNet.h"
#include "../Message/GsMessageManager.h"

// SessionEventHandler
void FGsNetLobby::onConnected(bool result, std::shared_ptr<Session> session)
{
	GMessage()->GetNet().SendMessageAsync(MessageNet::ID::LOGIN_SERVER_CONNECTION_SUCCESS, FGsMessageNone());
}

void FGsNetLobby::onDisconnected(std::shared_ptr<Session> session)
{
	GMessage()->GetNet().SendMessageAsync(MessageNet::ID::LOGIN_SERVER_DISCONNECTION, FGsMessageNone());
}

void FGsNetLobby::onError(std::shared_ptr<Session> session)
{
	GMessage()->GetNet().SendMessageAsync(MessageNet::ID::LOGIN_SERVER_CONNECTION_FAIL, FGsMessageNone());
}