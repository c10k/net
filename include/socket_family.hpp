#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace net {
namespace SF {

	enum class domain {
		UNIX      = AF_UNIX,
		LOCAL     = AF_UNIX,
		IPv4      = AF_INET,
		IPv6      = AF_INET6,
		IPX       = AF_IPX,
		NETLINK   = AF_NETLINK,
		X25       = AF_X25,
		AX25      = AF_AX25,
		ATMPVC    = AF_ATMPVC,
		APPLETALK = AF_APPLETALK,
		PACKET    = AF_PACKET,
		ALG       = AF_ALG
	};

	enum class type {
		TCP      = SOCK_STREAM,
		UDP      = SOCK_DGRAM,
		SCTP     = SOCK_SEQPACKET,
		RAW      = SOCK_RAW,
		RDM      = SOCK_RDM,
		NONBLOCK = SOCK_NONBLOCK,
		CLOEXEC  = SOCK_CLOEXEC
	};

	enum class shut { READ = SHUT_RD, WRITE = SHUT_WR, READWRITE = SHUT_RDWR };
}

namespace method {

	inline int construct(sockaddr_in &addrStruct, SF::domain _d,
	  const char _addr[], const int _port)
	{
		std::memset(&addrStruct, 0, sizeof(addrStruct));
		addrStruct.sin_family = static_cast<int>(_d);
		addrStruct.sin_port   = htons(_port);

		return inet_pton(static_cast<int>(_d), _addr, &addrStruct.sin_addr);
	}

	inline int construct(sockaddr_in6 &addrStruct, SF::domain _d,
	  const char _addr[], const int _port)
	{
		std::memset(&addrStruct, 0, sizeof(addrStruct));
		addrStruct.sin6_family = static_cast<int>(_d);
		addrStruct.sin6_port   = htons(_port);

		return inet_pton(static_cast<int>(_d), _addr, &addrStruct.sin6_addr);
	}
}
}
