#ifndef SOCKET_OPTIONS_HPP
#define SOCKET_OPTIONS_HPP

#include <utility>
#include <typeinfo>
#include <netinet/tcp.h>

extern "C" {
#include <sys/socket.h>
}

namespace net {

enum class Opt {
#ifdef SO_BROADCAST
    BROADCAST = SO_BROADCAST,
#endif
#ifdef SO_DEBUG
    DEBUG = SO_DEBUG,
#endif
#ifdef SO_DONTROUTE
    DONTROUTE = SO_DONTROUTE,
#endif
#ifdef SO_ERROR
    ERROR = SO_ERROR,
#endif
#ifdef SO_KEEPALIVE
    KEEPALIVE = SO_KEEPALIVE,
#endif
#ifdef SO_LINGER
    LINGER = SO_LINGER,
#endif
#ifdef SO_OOBINLINE
    OOBINLINE = SO_OOBINLINE,
#endif
#ifdef SO_RCVBUF
    RCVBUF = SO_RCVBUF,
#endif
#ifdef SO_SNDBUF
    SNDBUF = SO_SNDBUF,
#endif
#ifdef SO_RCVLOWAT
    RCVLOWAT = SO_RCVLOWAT,
#endif
#ifdef SO_SNDLOWAT
    SNDLOWAT = SO_SNDLOWAT,
#endif
#ifdef SO_RCVTIMEO
    RCVTIMEO = SO_RCVTIMEO,
#endif
#ifdef SO_SNDTIMEO
    SNDTIMEO = SO_SNDTIMEO,
#endif
#ifdef SO_REUSEADDR
    REUSEADDR = SO_REUSEADDR,
#endif
#ifdef SO_REUSEPORT
    REUSEPORT = SO_REUSEPORT,
#endif
#ifdef SO_TYPE
    TYPE = SO_TYPE,
#endif
#ifdef SO_USELOOPBACK
    USELOOPBACK = SO_USELOOPBACK,
#endif
#ifdef TCP_MAXSEG
    MAXSEG = TCP_MAXSEG,
#endif
#ifdef TCP_NODELAY
    NODELAY = TCP_NODELAY
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
    SockOpt() = delete;

public:
    SockOpt(const int _n) : i(_n), type(INT) {}
    SockOpt(const bool _on, const int _linger) : type(LINGER)
    {
        l.l_onoff  = _on ? 1 : 0;
        l.l_linger = _linger;
    }
    SockOpt(const decltype(t.tv_sec) _seconds,
            const decltype(t.tv_usec) _microseconds)
        : type(TIME)
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
