
#pragma once


#define USE_UNREAL4

#ifdef USE_UNREAL4

#include "CoreMinimal.h"
#include "Sockets.h"
#include "IPAddress.h"
#include "SocketSubsystem.h"

#else

#include "SocketAdapter.h"

#endif
