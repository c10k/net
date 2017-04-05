#ifndef SERVER_HPP
#define SERVER_HPP

#include "socket.hpp"
#include <functional>
#include <poll.h>
#include <sys/resource.h>
#include <thread>
#include <vector>
#include <algorithm>

namespace tcp {

using namespace net;

enum class events {
	IN     = POLLIN,
	OUT    = POLLOUT,
	PRI    = POLLPRI,
	RDHUP  = POLLRDHUP,
	ERR    = POLLERR,
	HANGUP = POLLHUP,
	INVAL  = POLLNVAL
};

enum class expo { EP, MT };

inline constexpr events operator|(events a, events b) noexcept
{
	return static_cast<events>(static_cast<int>(a) | static_cast<int>(b));
}

inline constexpr short int operator&(short int a, events b) noexcept
{
	return static_cast<short int>(a & static_cast<int>(b));
}

class server : private Socket {
private:
	// Socket listenSocket;

	// std::vector<net::Socket> connectedSockets;
	std::string listenIp;
	int listenPort;

	void processUsingPoll();

public:
	std::function<void(net::Socket &)> OnAccept;
	std::function<void(net::Socket &)> OnRequest;
	std::function<void(net::Socket &)> OnError;
	std::function<void()> OnClose;

	server(const std::string &_addr, const int _q = SOMAXCONN)
	    : Socket(std::get<0>(methods::IpAndPortParser(_addr)), Type::TCP)
	{
		Domain d;
		std::tie(d, listenIp, listenPort) = methods::IpAndPortParser(_addr);
	}

	void run(const expo policy);
};
}
#endif