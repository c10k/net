#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "socket_family.hpp"
#include <memory>
#include <stdexcept>
#include <string>


namespace net {

class Socket final {
private:
	union {
		addrStore store;
		addrIpv4 ipv4;
		addrIpv6 ipv6;
		addrUnix unix;
	};
	int sockfd;
	SF::domain domain;
	SF::type type;


	template <typename Fn, typename... Args>
	auto low_write(Fn &&_fn, const int _sockfd, const std::string &_msg,
	               Args &&... args) const
	{
		auto written = 0, count = 0;
		do {
			written = std::forward<Fn>(_fn)(_sockfd, _msg.c_str() + count,
			                                _msg.length() - count,
			                                std::forward<Args>(args)...);
			count += written;
		} while (written > 0);

		return written;
	}


	template <typename Fn, typename... Args>
	auto low_read(Fn &&_fn, const int _sockfd, std::string &_str,
	              Args &&... args) const
	{
		ssize_t recvd = 0;
		size_t count  = 0;

		const auto bufSize = _str.capacity();
		const auto buffer  = std::make_unique<char[]>(bufSize + 1);

		do {
			recvd = std::forward<Fn>(_fn)(_sockfd, buffer.get() + count,
			                              bufSize - count,
			                              std::forward<Args>(args)...);
			count += recvd;
			if (count == bufSize) {
				_str.append(buffer.get());
				_str.reserve(_str.length() + bufSize);
				std::memset(buffer.get(), 0, bufSize + 1);
				count = 0;
			}
		} while (recvd > 0);

		_str.append(buffer.get(), count);
		return recvd;
	}


	Socket(const int _sockfd, SF::domain _domain, SF::type _type,
	       const void *_addr)
	    : sockfd(_sockfd), domain(_domain), type(_type)
	{
		std::memset(&store, 0, sizeof(store));

		switch (domain) {
			case SF::domain::IPv4:
				ipv4.sin_family = AF_INET;
				std::memcpy(&ipv4, _addr, sizeof(ipv4));
				break;

			case SF::domain::IPv6:
				ipv6.sin6_family = AF_INET6;
				std::memcpy(&ipv6, _addr, sizeof(ipv6));
				break;

			case SF::domain::UNIX:
				unix.sun_family = AF_UNIX;
				std::memcpy(&unix, _addr, sizeof(unix));
				break;

			default: std::memcpy(&store, _addr, sizeof(store)); break;
		}
	}

	Socket(const Socket &) = delete;
	Socket &operator=(const Socket &) = delete;

public:
	Socket(SF::domain _domain, SF::type _type, const int _proto = 0)
	    : domain(_domain), type(_type)
	{
		const auto d = static_cast<int>(domain);
		const auto t = static_cast<int>(type);

		sockfd = socket(d, t, _proto);
		if (sockfd < 0) {
			const auto currErrno = errno;
			throw std::runtime_error(net::methods::getErrorMsg(currErrno));
		}

		switch (domain) {
			case SF::domain::IPv4: ipv4.sin_family  = AF_INET; break;
			case SF::domain::IPv6: ipv6.sin6_family = AF_INET6; break;
			case SF::domain::UNIX: unix.sun_family  = AF_UNIX; break;

			default: store.ss_family = d;
		}
	}


	Socket(Socket &&s)
	{
		sockfd = s.sockfd;
		domain = s.domain;
		type   = s.type;

		switch (s.domain) {
			case SF::domain::IPv4: ipv4 = s.ipv4; break;
			case SF::domain::IPv6: ipv6 = s.ipv6; break;
			case SF::domain::UNIX: unix = s.unix; break;

			default: store = s.store;
		}
	}

	auto getSocket() const noexcept { return sockfd; }


	// Bind method calls
	template <typename F>
	auto bind(F _fn) -> decltype(_fn(std::declval<addrIpv4 &>()), void()) const
	{
		addrIpv4 addr;

		auto res = _fn(addr);
		if (res == 1) {
			res = ::bind(sockfd, (sockaddr *) &addr, sizeof(addr));
			res = (res == 0) ? 1 : res;
		}

		const auto currErrno = errno;
		if (res == -1) {
			throw std::runtime_error(net::methods::getErrorMsg(currErrno));
		} else if (res == 0) {
			throw std::invalid_argument("Address argument invalid");
		}

		ipv4 = addr;
	}


	template <typename F>
	auto bind(F _fn) -> decltype(_fn(std::declval<addrIpv6 &>()), void()) const
	{
		addrIpv6 addr;

		auto res = _fn(addr);
		if (res == 1) {
			res = ::bind(sockfd, (sockaddr *) &addr, sizeof(addr));
			res = (res == 0) ? 1 : res;
		}

		const auto currErrno = errno;
		if (res == -1) {
			throw std::runtime_error(net::methods::getErrorMsg(currErrno));
		} else if (res == 0) {
			throw std::invalid_argument("Address argument invalid");
		}

		ipv6 = addr;
	}


	template <typename F>
	auto bind(F _fn) -> decltype(_fn(std::declval<addrUnix &>()), void()) const
	{
		addrUnix addr;

		auto res = _fn(addr);
		if (res == 1) {
			res = ::bind(sockfd, (sockaddr *) &addr, sizeof(addr));
			res = (res == 0) ? 1 : res;
		}

		const auto currErrno = errno;
		if (res == -1) {
			throw std::runtime_error(net::methods::getErrorMsg(currErrno));
		} else if (res == 0) {
			throw std::invalid_argument("Address argument invalid");
		}

		unix = addr;
	}
	// Bind method calls


	// Connect method calls
	void connect(const char[], const int = 0, bool * = nullptr);

	template <typename F>
	auto connect(F _fn, bool *_errorNB = nullptr)
	  -> decltype(_fn(std::declval<addrIpv4 &>()), void()) const
	{
		addrIpv4 addr;

		auto res = _fn(addr);
		if (res == 1) {
			res = ::connect(sockfd, (sockaddr *) &addr, sizeof(addr));
			res = (res == 0) ? 1 : res;
		}

		const auto currErrno = errno;
		if (res == -1) {
			if (currErrno == EINPROGRESS) {
				if (_errorNB != nullptr) {
					*_errorNB = true;
				} else {
					throw std::invalid_argument("errorNB argument missing");
				}
			} else {
				throw std::runtime_error(net::methods::getErrorMsg(currErrno));
			}
		} else if (res == 0) {
			throw std::invalid_argument("Address argument invalid");
		}
	}


	template <typename F>
	auto connect(F _fn, bool *_errorNB = nullptr)
	  -> decltype(_fn(std::declval<addrIpv6 &>()), void()) const
	{
		addrIpv6 addr;

		auto res = _fn(addr);
		if (res == 1) {
			res = ::connect(sockfd, (sockaddr *) &addr, sizeof(addr));
			res = (res == 0) ? 1 : res;
		}

		const auto currErrno = errno;
		if (res == -1) {
			if (currErrno == EINPROGRESS) {
				if (_errorNB != nullptr) {
					*_errorNB = true;
				} else {
					throw std::invalid_argument("errorNB argument missing");
				}
			} else {
				throw std::runtime_error(net::methods::getErrorMsg(currErrno));
			}
		} else if (res == 0) {
			throw std::invalid_argument("Address argument invalid");
		}
	}


	template <typename F>
	auto connect(F _fn, bool *_errorNB = nullptr)
	  -> decltype(_fn(std::declval<addrUnix &>()), void()) const
	{
		addrUnix addr;

		auto res = _fn(addr);
		if (res == 1) {
			res = ::connect(sockfd, (sockaddr *) &addr, sizeof(addr));
			res = (res == 0) ? 1 : res;
		}

		const auto currErrno = errno;
		if (res == -1) {
			if (currErrno == EINPROGRESS) {
				if (_errorNB != nullptr) {
					*_errorNB = true;
				} else {
					throw std::invalid_argument("errorNB argument missing");
				}
			} else {
				throw std::runtime_error(net::methods::getErrorMsg(currErrno));
			}
		} else if (res == 0) {
			throw std::invalid_argument("Address argument invalid");
		}
	}
	// Connect method calls


	void start(const char[], const int = 0, const int = SOMAXCONN);

	Socket accept(bool * = nullptr) const;


	void write(const std::string &, bool * = nullptr) const;
	std::string read(const int = 1024, bool * = nullptr) const;


	// Send method calls
	void send(const std::string &, SF::send = SF::send::NONE,
	          bool * = nullptr) const;


	template <typename F>
	auto send(const std::string &_msg, F _fn, SF::send _flags = SF::send::NONE,
	          bool *_errorNB = nullptr) const
	  -> decltype(_fn(std::declval<addrIpv4 &>()), void()) const
	{
		addrIpv4 addr;

		const auto flags = static_cast<int>(_flags);
		const auto res   = _fn(addr);

		if (res == 0) {
			throw std::invalid_argument("Address argument invalid");
		}

		ssize_t sent = -1;
		if (res == 1) {
			sent = low_write(::sendto, sockfd, _msg, flags, (sockaddr *) &addr,
			                 sizeof(addr));
		}

		const auto currErrno = errno;
		if (res == -1 || sent == -1) {
			if (currErrno == EAGAIN || currErrno == EWOULDBLOCK) {
				if (_errorNB != nullptr) {
					*_errorNB = true;
				} else {
					throw std::invalid_argument("errorNB argument missing");
				}
			} else {
				throw std::runtime_error(net::methods::getErrorMsg(currErrno));
			}
		}
	}


	template <typename F>
	auto send(const std::string &_msg, F _fn, SF::send _flags = SF::send::NONE,
	          bool *_errorNB = nullptr) const
	  -> decltype(_fn(std::declval<addrIpv6 &>()), void()) const
	{
		addrIpv6 addr;

		const auto flags = static_cast<int>(_flags);
		const auto res   = _fn(addr);

		if (res == 0) {
			throw std::invalid_argument("Address argument invalid");
		}

		ssize_t sent = -1;
		if (res == 1) {
			sent = low_write(::sendto, sockfd, _msg, flags, (sockaddr *) &addr,
			                 sizeof(addr));
		}

		const auto currErrno = errno;
		if (res == -1 || sent == -1) {
			if (currErrno == EAGAIN || currErrno == EWOULDBLOCK) {
				if (_errorNB != nullptr) {
					*_errorNB = true;
				} else {
					throw std::invalid_argument("errorNB argument missing");
				}
			} else {
				throw std::runtime_error(net::methods::getErrorMsg(currErrno));
			}
		}
	}


	template <typename F>
	auto send(const std::string &_msg, F _fn, SF::send _flags = SF::send::NONE,
	          bool *_errorNB = nullptr) const
	  -> decltype(_fn(std::declval<addrUnix &>()), void()) const
	{
		addrUnix addr;

		const auto flags = static_cast<int>(_flags);
		const auto res   = _fn(addr);

		if (res == 0) {
			throw std::invalid_argument("Address argument invalid");
		}

		ssize_t sent = -1;
		if (res == 1) {
			sent = low_write(::sendto, sockfd, _msg, flags, (sockaddr *) &addr,
			                 sizeof(addr));
		}

		const auto currErrno = errno;
		if (res == -1 || sent == -1) {
			if (currErrno == EAGAIN || currErrno == EWOULDBLOCK) {
				if (_errorNB != nullptr) {
					*_errorNB = true;
				} else {
					throw std::invalid_argument("errorNB argument missing");
				}
			} else {
				throw std::runtime_error(net::methods::getErrorMsg(currErrno));
			}
		}
	}
	// Send method calls


	// recv method calls

	std::string recv(const int = 1024, SF::recv = SF::recv::NONE,
	                 bool * = nullptr) const;

	template <typename F>
	auto recv(const int _bufSize, F _fn, SF::recv _flags, bool *_errorNB) const
	  -> decltype(_fn(std::declval<const addrIpv4 &>()), std::string()) const
	{
		addrIpv4 addr;
		std::string str;
		str.reserve(_bufSize);

		const auto flags = static_cast<int>(_flags);
		socklen_t length = sizeof(addr);

		const auto recvd = low_read(::recvfrom, sockfd, str, flags,
		                            (sockaddr *) &addr, &length);

		const auto currErrno = errno;
		if (recvd == -1) {
			if (currErrno == EAGAIN || currErrno == EWOULDBLOCK) {
				if (_errorNB != nullptr) {
					*_errorNB = true;
				} else {
					throw std::invalid_argument("errorNB argument missing");
				}
			} else {
				throw std::runtime_error(net::methods::getErrorMsg(currErrno));
			}
		}

		_fn(addr);
		return str;
	}


	template <typename F>
	auto recv(const int _bufSize, F _fn, SF::recv _flags, bool *_errorNB) const
	  -> decltype(_fn(std::declval<const addrIpv6 &>()), std::string()) const
	{
		addrIpv6 addr;
		std::string str;
		str.reserve(_bufSize);

		const auto flags = static_cast<int>(_flags);
		socklen_t length = sizeof(addr);

		const auto recvd = low_read(::recvfrom, sockfd, str, flags,
		                            (sockaddr *) &addr, &length);

		const auto currErrno = errno;
		if (recvd == -1) {
			if (currErrno == EAGAIN || currErrno == EWOULDBLOCK) {
				if (_errorNB != nullptr) {
					*_errorNB = true;
				} else {
					throw std::invalid_argument("errorNB argument missing");
				}
			} else {
				throw std::runtime_error(net::methods::getErrorMsg(currErrno));
			}
		}

		_fn(addr);
		return str;
	}


	template <typename F>
	auto recv(const int _bufSize, F _fn, SF::recv _flags, bool *_errorNB) const
	  -> decltype(_fn(std::declval<const addrUnix &>()), std::string()) const
	{
		addrUnix addr;
		std::string str;
		str.reserve(_bufSize);

		const auto flags = static_cast<int>(_flags);
		socklen_t length = sizeof(addr);

		const auto recvd = low_read(::recvfrom, sockfd, str, flags,
		                            (sockaddr *) &addr, &length);

		const auto currErrno = errno;
		if (recvd == -1) {
			if (currErrno == EAGAIN || currErrno == EWOULDBLOCK) {
				if (_errorNB != nullptr) {
					*_errorNB = true;
				} else {
					throw std::invalid_argument("errorNB argument missing");
				}
			} else {
				throw std::runtime_error(net::methods::getErrorMsg(currErrno));
			}
		}

		_fn(addr);
		return str;
	}
	// recv method calls


	void setOpt(SF::opt, void *) const;
	void getOpt(SF::opt, void *) const;

	void stop(SF::shut s) const noexcept
	{
		shutdown(sockfd, static_cast<int>(s));
	}

	void close() const noexcept { ::close(sockfd); }

	~Socket()
	{
		// TODO: unlink() the path in case of addrUnix
		// unlink(unix.sun_path);
		::close(sockfd);
	}
};
}

#endif
