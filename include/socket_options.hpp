#ifndef SOCKET_OPTIONS_HPP
#define SOCKET_OPTIONS_HPP

#include <utility>
#include <typeinfo>

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


class SockOpt final {
	union {
		timeval t;
		linger l;
		int i;
	};
	enum { TIME = 0, LINGER = 1, INT = 2 } type;

	SockOpt(int, int) = delete;

public:
	SockOpt(const int _n) : i(_n), type(INT) {}
	explicit SockOpt(const bool _on, const int _linger) : type(LINGER)
	{
		l.l_onoff  = _on ? 1 : 0;
		l.l_linger = _linger;
	}
	explicit SockOpt(const long _seconds, const long _microseconds) : type(TIME)
	{
		t.tv_sec  = _seconds;
		t.tv_usec = _microseconds;
	}

	auto getType() const noexcept { return type; }

	auto getTime() const
	{
		return (type == TIME) ? std::make_pair(t.tv_sec, t.tv_usec)
		                      : throw std::bad_cast();
	}

	auto getLinger() const
	{
		return (type == LINGER)
		  ? std::make_pair(static_cast<bool>(l.l_onoff), l.l_linger)
		  : throw std::bad_cast();
	}

	auto getValue() const { return (type == INT) ? i : throw std::bad_cast(); }
};


inline bool operator==(const SockOpt &_lhs, const int _rhs)
{
	if (_lhs.getType() == 2) {
		return _lhs.getValue() == _rhs;
	} else {
		return false;
	}
}
inline bool operator==(const int _lhs, const SockOpt &_rhs)
{
	return _rhs == _lhs;
}


inline bool operator==(const SockOpt &_lhs, const linger _rhs)
{
	if (_lhs.getType() == 1) {
		const auto lin = _lhs.getLinger();
		return (lin.first == _rhs.l_onoff && lin.second == _rhs.l_linger);
	} else {
		return false;
	}
}
inline bool operator==(const linger _lhs, const SockOpt &_rhs)
{
	return _rhs == _lhs;
}


inline bool operator==(const SockOpt &_lhs, const timeval _rhs)
{
	if (_lhs.getType() == 0) {
		const auto time = _lhs.getTime();
		return (time.first == _rhs.tv_sec && time.second == _rhs.tv_usec);
	} else {
		return false;
	}
}
inline bool operator==(const timeval _lhs, const SockOpt &_rhs)
{
	return _rhs == _lhs;
}
}

#endif
