#pragma once
#include <typeinfo>
#include "LeanPacket_generated.h"

class FGsNet
{
public:
	enum Mode
	{
		LOBBY,
		GAME,
		RECONNECT,
		MAX,
	};
	struct NetConnectionData
	{
		NetConnectionData() {};
		NetConnectionData(FString& addr, uint16& port) : _addr(addr), _port(port) {}
		NetConnectionData(FString&& addr, uint16&& port) : _addr(std::move(addr)), _port(std::move(port)) {}
		FString _addr;
		uint16  _port;
	};
};

template<typename T>
static const T* GetPacket(const void* buf, uint32_t len)
{
	const uint8_t* data = static_cast<const uint8_t*>(buf);
	flatbuffers::Verifier verifier(data, len);
	if (!verifier.VerifyBuffer<T>())
	{
#if WITH_EDITOR 
		GSLOG(Error, TEXT(u8" %s Packet verifier failed"), typeid(T).name());
#endif
		return nullptr;
	}
	return flatbuffers::GetRoot<T>(data);
}
