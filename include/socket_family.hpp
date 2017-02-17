#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <mutex>
#include <cstring>
#include <errno.h>

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


	enum class recv {
		NONE    = 0,
		PEEK    = MSG_PEEK,
		OOB     = MSG_OOB,
		WAITALL = MSG_WAITALL
	};
	inline constexpr recv operator|(recv a, recv b) noexcept
	{
		return static_cast<recv>(static_cast<int>(a) | static_cast<int>(b));
	}

	enum class send {
		NONE     = 0,
		EOR      = MSG_EOR,
		OOB      = MSG_OOB,
		NOSIGNAL = MSG_NOSIGNAL
	};
	inline constexpr send operator|(send a, send b) noexcept
	{
		return static_cast<send>(static_cast<int>(a) | static_cast<int>(b));
	}
}


namespace method {

	inline std::string getErrorMsg(int errorNumber)
	{
		static std::mutex lockToErrorString;
		std::lock_guard<std::mutex> lockGuard(lockToErrorString);
		char *errMsg = strerror(errorNumber);
		std::string returnString(errMsg);
		return returnString;
	}

	inline int construct(
	  sockaddr_in &addrStruct, const char _addr[], const int _port) noexcept
	{
		std::memset(&addrStruct, 0, sizeof(addrStruct));
		addrStruct.sin_family = AF_INET;
		addrStruct.sin_port   = htons(_port);

		return inet_pton(AF_INET, _addr, &addrStruct.sin_addr);
	}


	inline int construct(
	  sockaddr_in6 &addrStruct, const char _addr[], const int _port) noexcept
	{
		std::memset(&addrStruct, 0, sizeof(addrStruct));
		addrStruct.sin6_family = AF_INET6;
		addrStruct.sin6_port   = htons(_port);

		return inet_pton(AF_INET6, _addr, &addrStruct.sin6_addr);
	}


	inline int construct(sockaddr_storage &addrStruct, SF::domain _d,
	  const char _addr[], const int _port)
	{
		std::memset(&addrStruct, 0, sizeof(addrStruct));

		switch (_d) {
			case SF::domain::IPv4: {
				sockaddr_in *p = reinterpret_cast<sockaddr_in *>(&addrStruct);
				p->sin_family  = AF_INET;
				p->sin_port    = htons(_port);
				return inet_pton(AF_INET, _addr, &p->sin_addr);
			}

			case SF::domain::IPv6: {
				sockaddr_in6 *p = reinterpret_cast<sockaddr_in6 *>(&addrStruct);
				p->sin6_family  = AF_INET6;
				p->sin6_port    = htons(_port);
				return inet_pton(AF_INET6, _addr, &p->sin6_addr);
			}

			default: return -1;
		}
	}
}
}
