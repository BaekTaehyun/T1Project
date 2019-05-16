
#include "SocketHelper.h"


#ifndef USE_UNREAL4
#include <random>
#include <vector>
#else
#include <chrono>
#include <thread>
#endif


FSocket* ConnectFSocket(const char* host, uint16_t port)
{
#ifndef USE_UNREAL4
	auto addresses = IPAddress::GetHostByName(AF_UNSPEC, SOCK_STREAM, host, port);
	if (addresses.empty() == true)
	{
		return nullptr;
	}

	size_t idx = 0;
	if (addresses.size() > 1)
	{
		std::default_random_engine engine(std::random_device{}());
		std::uniform_int_distribution<size_t> dist(0, addresses.size() - 1);
		idx = dist(engine);
	}

	FSocket* socket = new FSocket();
	if (socket == nullptr)
	{
		return nullptr;
	}

	auto& addr = addresses[idx];
	if (socket->Create(addr.getFamily()) == false)
	{
		DestroyFSocket(socket);
		return nullptr;
	}

	socket->SetNonBlocking(true);
	if (socket->Connect(addr) == false)
	{
		DestroyFSocket(socket);
		return nullptr;
	}

	return socket;
#else
	ISocketSubsystem* socketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	if (socketSubsystem == nullptr)
	{
		return nullptr;
	}

	// 주의! 현재 언리얼4의 GetHostByName 구현은 적합한 첫번째 IP주소를 리턴한다.
	FResolveInfo* resolveInfo = socketSubsystem->GetHostByName(host);
	if (resolveInfo == nullptr)
	{
		return nullptr;
	}

	while (!resolveInfo->IsComplete())
	{
		std::chrono::milliseconds timeout(1);
		std::this_thread::sleep_for(timeout);
	}

	if (resolveInfo->GetErrorCode() != SE_NO_ERROR)
	{
		return nullptr;
	}

	const FInternetAddr& address = resolveInfo->GetResolvedAddress();
	if (address.IsValid() == false)
	{
		return nullptr;
	}

	TSharedRef<FInternetAddr> addr = address.Clone();
	addr->SetPort(port);

	FSocket* socket = socketSubsystem->CreateSocket(NAME_Stream, FString("Client"), false);
	if (socket == nullptr)
	{
		return nullptr;
	}

	socket->SetNonBlocking(true);
	if (socket->Connect(*addr) == false)
	{
		DestroyFSocket(socket);
		return nullptr;
	}

	return socket;
#endif
}

void DestroyFSocket(FSocket* socket)
{
#ifndef USE_UNREAL4
	delete socket;
#else
	if (socket == nullptr)
	{
		return;
	}

	ESocketConnectionState state = socket->GetConnectionState();
	if (state != ESocketConnectionState::SCS_NotConnected)
	{
		socket->Close();
	}

	ISocketSubsystem* socketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	if (socketSubsystem != nullptr)
	{
		socketSubsystem->DestroySocket(socket);
	}
#endif
}
