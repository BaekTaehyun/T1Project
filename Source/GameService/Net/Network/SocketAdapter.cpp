
#include "SocketAdapter.h"


#ifndef USE_UNREAL4

#include <ws2tcpip.h>

namespace
{

	class CheckSocket
	{
	public:

		enum class State
		{
			None,
			OK,
			Failed,
		};

		static State CheckConnect(SOCKET socket)
		{
			// connect 실패는 excpetfds로 알 수 있다.
			// 하지만 접속 성공후 서버에서 아무것도 보내지 않으면
			// readfds가 반응하지 않기 때문에 readfds만으로 connect 성공 여부를 알 수 없다.
			// 따라서 writefds를 동시에 체크한다.
			fd_set read;
			fd_set write;
			fd_set except;
			FD_ZERO(&read);
			FD_ZERO(&write);
			FD_ZERO(&except);

			FD_SET(socket, &read);
			FD_SET(socket, &write);
			FD_SET(socket, &except);

			timeval timeout;
			timeout.tv_sec = 0;
			timeout.tv_usec = 0;
			int ret = ::select(static_cast<int>(socket + 1), &read, &write, &except, &timeout);
			if (ret == SOCKET_ERROR)
			{
				return State::Failed;
			}
			else if (ret == 0)
			{
				return State::None;
			}

			if (FD_ISSET(socket, &except))
			{
				return State::Failed;
			}

			if (FD_ISSET(socket, &write) || FD_ISSET(socket, &read))
			{
				return State::OK;
			}

			return State::None;
		}

		static State CheckReceive(SOCKET socket)
		{
			//return State::OK;

			//*
			fd_set read;
			fd_set except;
			FD_ZERO(&read);
			FD_ZERO(&except);

			FD_SET(socket, &read);
			FD_SET(socket, &except);

			timeval timeout;
			timeout.tv_sec = 0;
			timeout.tv_usec = 0;
			int ret = ::select(static_cast<int>(socket + 1), &read, nullptr, &except, &timeout);
			if (ret == SOCKET_ERROR)
			{
				return State::Failed;
			}
			else if (ret == 0)
			{
				return State::None;
			}

			if (FD_ISSET(socket, &except))
			{
				return State::Failed;
			}

			if (FD_ISSET(socket, &read))
			{
				return State::OK;
			}

			return State::None;
			//*/
		}

		static State CheckSend(SOCKET socket)
		{
			fd_set write;
			fd_set except;
			FD_ZERO(&write);
			FD_ZERO(&except);

			FD_SET(socket, &write);
			FD_SET(socket, &except);

			timeval timeout;
			timeout.tv_sec = 0;
			timeout.tv_usec = 0;
			int ret = ::select(static_cast<int>(socket + 1), nullptr, &write, &except, &timeout);
			if (ret == SOCKET_ERROR)
			{
				return State::Failed;
			}
			else if (ret == 0)
			{
				return State::None;
			}

			if (FD_ISSET(socket, &except))
			{
				return State::Failed;
			}

			if (FD_ISSET(socket, &write))
			{
				return State::OK;
			}

			return State::None;
		}

	};

}

std::vector<IPAddress> IPAddress::GetHostByName(int family, int sockType, const char* host, uint16_t port)
{
	std::vector<IPAddress> addrs;

	addrinfo hints;
	::memset(&hints, 0, sizeof(hints));
	hints.ai_family = family;
	hints.ai_socktype = sockType;
	hints.ai_flags = AI_ADDRCONFIG | AI_PASSIVE;

	addrinfo* result = nullptr;
	int ret = ::getaddrinfo(host, std::to_string(port).c_str(), &hints, &result);
	if (ret == 0)
	{
		for (addrinfo* ptr = result; ptr != nullptr; ptr = ptr->ai_next)
		{
			if (ptr->ai_family == AF_INET)
			{
				addrs.emplace_back(ptr->ai_addr, ptr->ai_addrlen);
			}
			else if (ptr->ai_family == AF_INET6)
			{
				addrs.emplace_back(ptr->ai_addr, ptr->ai_addrlen);
			}
		}

		::freeaddrinfo(result);
		result = nullptr;
	}

	return addrs;
}

const IPAddress& IPAddress::GetAnyAddress(int family)
{
	if (family == AF_INET6)
	{
		static IPAddress ipv6Any("::", 0);
		return ipv6Any;
	}

	static IPAddress ipv4Any("0.0.0.0", 0);
	return ipv4Any;
}



IPAddress::IPAddress()
{
	::memset(&sockAddr_, 0, sizeof(sockAddr_));
}

IPAddress::IPAddress(const sockaddr_storage& addr)
	: sockAddr_(addr)
{
}

IPAddress::IPAddress(const sockaddr* sa, std::size_t len)
{
	::memset(&sockAddr_, 0, sizeof(sockAddr_));
	if (sa->sa_family == AF_INET && len == sizeof(sockaddr_in))
	{
		*reinterpret_cast<sockaddr_in*>(&sockAddr_) = *(reinterpret_cast<const sockaddr_in*>(sa));
	}
	else if (sa->sa_family == AF_INET6 && len == sizeof(sockaddr_in6))
	{
		*reinterpret_cast<sockaddr_in6*>(&sockAddr_) = *(reinterpret_cast<const sockaddr_in6*>(sa));
	}
}

IPAddress::IPAddress(const char* host, uint16_t port)
{
	setAddress(AF_UNSPEC, SOCK_STREAM, host, port);
}

IPAddress::IPAddress(int family, const char* host, uint16_t port)
{
	setAddress(family, SOCK_STREAM, host, port);
}

int IPAddress::getFamily() const
{
	return sockAddr_.ss_family;
}

const sockaddr* IPAddress::getSockAddr() const
{
	return reinterpret_cast<const sockaddr*>(&sockAddr_);
}

int IPAddress::getSockAddrLen() const
{
	int family = getFamily();
	if (family == AF_INET)
	{
		return sizeof(sockaddr_in);
	}
	else if (family == AF_INET6)
	{
		return sizeof(sockaddr_in6);
	}

	return 0;
}

std::string IPAddress::getIpString() const
{
	char ip[INET6_ADDRSTRLEN] = { 0, };

	int family = getFamily();
	if (family == AF_INET)
	{
		const sockaddr_in* addr = reinterpret_cast<const sockaddr_in*>(&sockAddr_);
		::inet_ntop(family, &addr->sin_addr, ip, sizeof(ip));
	}
	else if (family == AF_INET6)
	{
		const sockaddr_in6* addr = reinterpret_cast<const sockaddr_in6*>(&sockAddr_);
		::inet_ntop(family, &addr->sin6_addr, ip, sizeof(ip));
	}

	return ip;
}

uint16_t IPAddress::getPort() const
{
	int family = getFamily();
	if (family == AF_INET)
	{
		const sockaddr_in* addr = reinterpret_cast<const sockaddr_in*>(&sockAddr_);
		return ::ntohs(addr->sin_port);
	}
	else if (family == AF_INET6)
	{
		const sockaddr_in6* addr = reinterpret_cast<const sockaddr_in6*>(&sockAddr_);
		return ::ntohs(addr->sin6_port);
	}

	return 0;
}

void IPAddress::setPort(uint16_t port)
{
	int family = getFamily();
	if (family == AF_INET)
	{
		sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(&sockAddr_);
		addr->sin_port = ::htons(port);
	}
	else if (family == AF_INET6)
	{
		sockaddr_in6* addr = reinterpret_cast<sockaddr_in6*>(&sockAddr_);
		addr->sin6_port = ::htons(port);
	}
}

std::string IPAddress::ToString(bool appendPort) const
{
	std::string addr = getIpString();
	if (appendPort == true)
	{
		addr += ":";
		addr += std::to_string(getPort());
	}

	return addr;
}

void IPAddress::setAddress(int family, int sockType, const char* host, uint16_t port)
{
	::memset(&sockAddr_, 0, sizeof(sockAddr_));

	auto addresses = GetHostByName(family, sockType, host, port);
	if (addresses.empty() == true)
	{
		return;
	}

	*this = addresses.front();
}



FSocket::FSocket()
{
}

FSocket::~FSocket()
{
	Close();
}

bool FSocket::Create(int family)
{
	socket_ = ::socket(family, SOCK_STREAM, IPPROTO_TCP);
	if (socket_ == INVALID_SOCKET)
	{
		return false;
	}

	int reuseAddr = 1;
	::setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&reuseAddr), sizeof(reuseAddr));
	return true;
}

void FSocket::SetNonBlocking(bool isNonBlocking)
{
	// non-blocking 설정
	unsigned long ul = isNonBlocking ? 1 : 0;
	::ioctlsocket(socket_, FIONBIO, &ul);
}

bool FSocket::Connect(const IPAddress& addr)
{
	if (::connect(socket_, addr.getSockAddr(), addr.getSockAddrLen()) == SOCKET_ERROR)
	{
		int errorCode = ::WSAGetLastError();
		if (errorCode != WSAEWOULDBLOCK)
		{
			Close();
			return false;
		}
	}

	return true;
}

void FSocket::Close()
{
	if (socket_ != INVALID_SOCKET)
	{
		::closesocket(socket_);
		socket_ = INVALID_SOCKET;
	}
}

bool FSocket::HasPendingConnection(bool& connected)
{
	connected = false;

	auto state = CheckSocket::CheckConnect(socket_);
	if (state == CheckSocket::State::Failed)
	{
		Close();
		return false;
	}
	else if (state == CheckSocket::State::OK)
	{
		connected = true;
	}

	return true;
}

bool FSocket::HasPendingData(uint32_t& pendingSize)
{
	pendingSize = 0;

	auto state = CheckSocket::CheckReceive(socket_);
	if (state == CheckSocket::State::Failed)
	{
		Close();
		return false;
	}
	else if (state == CheckSocket::State::OK)
	{
		u_long dataSize = 0;
		::ioctlsocket(socket_, FIONREAD, &dataSize);
		pendingSize = dataSize;
		return true;
	}

	return false;
}

bool FSocket::Send(const uint8_t* data, int32_t size, int32_t& sent)
{
	sent = 0;
	int result = ::send(socket_, reinterpret_cast<const char*>(data), size, 0);
	if (result <= SOCKET_ERROR)
	{
		int errorCode = ::WSAGetLastError();
		if (errorCode != WSAEWOULDBLOCK)
		{
			Close();
			return false;
		}
	}
	else
	{
		sent = result;
	}

	return true;
}

bool FSocket::Recv(uint8_t* data, int32_t size, int32_t& read)
{
	read = 0;
	int result = ::recv(socket_, reinterpret_cast<char*>(data), size, 0);
	if (result <= SOCKET_ERROR)
	{
		int errorCode = ::WSAGetLastError();
		if (errorCode != WSAEWOULDBLOCK)
		{
			Close();
			return false;
		}
	}
	else
	{
		read = result;
	}

	return true;
}

#endif