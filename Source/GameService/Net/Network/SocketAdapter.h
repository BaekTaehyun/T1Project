
#pragma once

#include "SocketConfig.h"

#ifndef __UNREAL__

#include <cstdint>
#include <string>
#include <vector>

#include <winsock2.h>


class IPAddress
{
public:

	static std::vector<IPAddress> GetHostByName(int family, int sockType, const char* host, uint16_t port);
	static const IPAddress& GetAnyAddress(int family);

	IPAddress();
	IPAddress(const sockaddr_storage& addr);
	IPAddress(const sockaddr* sa, size_t len);
	IPAddress(const char* host, uint16_t port);
	IPAddress(int family, const char* host, uint16_t port);

	int getFamily() const;

	const sockaddr* getSockAddr() const;
	int getSockAddrLen() const;

	std::string getIpString() const;
	uint16_t getPort() const;

	void setPort(uint16_t port);

	std::string ToString(bool appendPort) const;

private:

	void setAddress(int family, int sockType, const char* host, uint16_t port);

private:

	sockaddr_storage sockAddr_;
};


class FSocket
{
	SOCKET socket_ = INVALID_SOCKET;

public:

	FSocket();
	~FSocket();

	bool Create(int family);
	void SetNonBlocking(bool isNonBlocking);

	bool Connect(const IPAddress& addr);
	void Close();

	bool HasPendingConnection(bool& connected);
	bool HasPendingData(uint32_t& pendingSize);

	bool Send(const uint8_t* data, int32_t size, int32_t& sent);
	bool Recv(uint8_t* data, int32_t size, int32_t& read);

};

#endif

