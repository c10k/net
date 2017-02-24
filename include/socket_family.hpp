#ifndef SOCKET_FAMILY_HPP
#define SOCKET_FAMILY_HPP

#include <mutex>
#include <cstring>
#include <utility>

extern "C" {
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
}

namespace net {

using addrIpv4  = sockaddr_in;
using addrIpv6  = sockaddr_in6;
using addrUnix  = sockaddr_un;
using addrStore = sockaddr_storage;

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
		TCP       = SOCK_STREAM,
		UDP       = SOCK_DGRAM,
		SEQPACKET = SOCK_SEQPACKET,
		RAW       = SOCK_RAW,
		RDM       = SOCK_RDM,
		NONBLOCK  = SOCK_NONBLOCK,
		CLOEXEC   = SOCK_CLOEXEC
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


	enum class opt {
		BROADCAST = SO_BROADCAST,
		DEBUG     = SO_DEBUG,
		DONTROUTE = SO_DONTROUTE,
		ERROR     = SO_ERROR,
		KEEPALIVE = SO_KEEPALIVE,
		LINGER    = SO_LINGER,
		OOBINLINE = SO_OOBINLINE,
		RCVBUF    = SO_RCVBUF,
		SNDBUF    = SO_SNDBUF,
		RCVLOWAT  = SO_RCVLOWAT,
		SNDLOWAT  = SO_SNDLOWAT,
		RCVTIMEO  = SO_RCVTIMEO,
		SNDTIMEO  = SO_SNDTIMEO,
		REUSEADDR = SO_REUSEADDR,
		REUSEPORT = SO_REUSEPORT,
		TYPE      = SO_TYPE,
#ifdef SO_USELOOPBACK
		USELOOPBACK = SO_USELOOPBACK
#endif
	};

	struct sockOpt {
		union {
			timeval t;
			linger l;
			int i;
		};
		enum { TIME, LINGER, INT } type;
		sockOpt()
		{
			i    = 0;
			type = INT;
		}
		auto getType() const noexcept { return type; }
		void setTime(const decltype(t.tv_sec) seconds,
		             const decltype(t.tv_usec) microseconds)
		{
			t.tv_sec  = seconds;
			t.tv_usec = microseconds;
			type      = TIME;
		}
		auto getTime() const
		{
			return (type == TIME)
			  ? std::make_pair(t.tv_sec, t.tv_usec)
			  : std::make_pair(static_cast<decltype(t.tv_sec)>(0),
			                   static_cast<decltype(t.tv_usec)>(0));
		}
		void setLinger(const bool _on, const int _linger)
		{
			l.l_onoff  = _on ? 1 : 0;
			l.l_linger = _linger;
			type       = LINGER;
		}
		auto getLinger() const
		{
			return (type == LINGER)
			  ? std::make_pair(static_cast<bool>(l.l_onoff), l.l_linger)
			  : std::make_pair(false, 0);
		}
		void setValue(const int _n)
		{
			i    = _n;
			type = INT;
		}
		auto getValue() const noexcept { return (type == INT) ? i : 0; }
	};
}


namespace methods {

	/** @function getErrorMsg
	* Returns the standard human readable error message corresponding to given
	* errorNumber.
	*
	* @param {int} errorNumber Error number whose string to return.
	* @Returns {string} Standard error string corresponding to given
	* errorNumber.
	*/
	inline std::string getErrorMsg(const int errorNumber)
	{
		static std::mutex m;
		std::lock_guard<std::mutex> lock(m);
		const char *errMsg = strerror(errorNumber);
		std::string returnString(errMsg);
		return returnString;
	}

	/** @function construct
	* Fills the given addrIpv4 structure object with given ip address and port.
	*
	* @param {addrIpv4} addrStruct Ipv4 structure object that needs to be filled
	* with given ip address and port.
	* @param {char []} _addr Ip address which needs to be filled in the addrIpv4
	* structure object.
	* @param {int} _port Port number which needs to be filled in the addrIpv4
	* structure object.
	* @Returns {int} 1 if sucessful, 0 if given ip address does not represent a
	* valid ip address, -1 if some error occurred.
	*/
	inline int construct(addrIpv4 &addrStruct, const char _addr[],
	                     const int _port) noexcept
	{
		std::memset(&addrStruct, 0, sizeof(addrStruct));
		addrStruct.sin_family = AF_INET;
		addrStruct.sin_port   = htons(_port);

		return inet_pton(AF_INET, _addr, &addrStruct.sin_addr);
	}


	/** @function construct
	* Fills the given addrIpv6 structure object with given ip address and port.
	*
	* @param {addrIpv6} addrStruct Ipv6 structure object that needs to be filled
	* with given ip address and port.
	* @param {char []} _addr Ip address which needs to be filled in the addrIpv6
	* structure object.
	* @param {int} _port Port number which needs to be filled in the addrIpv6
	* structure object.
	* @Returns {int} 1 if sucessful, 0 if given ip address does not represent a
	* valid ip address, -1 if some error occurred.
	*/
	inline int construct(addrIpv6 &addrStruct, const char _addr[],
	                     const int _port) noexcept
	{
		// TODO: replace code with call to getaddrinfo()
		std::memset(&addrStruct, 0, sizeof(addrStruct));
		addrStruct.sin6_family = AF_INET6;
		addrStruct.sin6_port   = htons(_port);

		return inet_pton(AF_INET6, _addr, &addrStruct.sin6_addr);
	}


	/** @function construct
	* Fills the given addrUnix structure object with given ip address and port.
	*
	* @param {addrUnix} addrStruct addrUnix structure object that needs to be
	* filled with given path.
	* @param {char []} _addr Path which needs to be filled in the addrUnix
	* structure object.
	* @Returns {int} Always returns 1.
	*/
	inline int construct(addrUnix &addrStruct, const char _addr[]) noexcept
	{
		std::memset(&addrStruct, 0, sizeof(addrStruct));
		addrStruct.sun_family = AF_UNIX;
		std::strncpy(addrStruct.sun_path, _addr, 108);
		return 1;
	}
}
}

#endif
