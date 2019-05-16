#pragma once

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

