#ifndef SOCKET_OPTIONS_HPP
#define SOCKET_OPTIONS_HPP

#include <utility>

extern "C" {
#include <sys/socket.h>
}

namespace net {

enum class Opt {
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

struct SockOpt {
	union {
		timeval t;
		linger l;
		int i;
	};
	enum { TIME, LINGER, INT } type;

	SockOpt(const int _n) { setValue(_n); }
	SockOpt(const bool _on, const int _linger) { setLinger(_on, _linger); }
	SockOpt(const decltype(t.tv_sec) _seconds,
	        const decltype(t.tv_usec) _microseconds)
	{
		setTime(_seconds, _microseconds);
	}

	auto getType() const noexcept { return type; }

	void setTime(const decltype(t.tv_sec) _seconds,
	             const decltype(t.tv_usec) _microseconds)
	{
		t.tv_sec  = _seconds;
		t.tv_usec = _microseconds;
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

#endif
