
#pragma once


#include "SocketConfig.h"


FSocket* ConnectFSocket(const char* host, uint16_t port);
void DestroyFSocket(FSocket* socket);

