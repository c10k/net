#include "socket.hpp"


namespace net {

Socket::Socket(const int _sockfd, SF::domain _domain, SF::type _type,
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


void Socket::start(const char _addr[], const int _port, const int _q)
{
	try {
		switch (domain) {
			case SF::domain::IPv4:
				bind([&](addrIPv4 &s) {
					return net::methods::construct(s, _addr, _port);
				});
				break;

			case SF::domain::IPv6:
				bind([&](addrIPv6 &s) {
					return net::methods::construct(s, _addr, _port);
				});
				break;

			case SF::domain::UNIX:
				bind([&](addrUnix &s) {
					return net::methods::construct(s, _addr);
				});
				break;

			default:
				throw std::invalid_argument("Protocol family not implemented");
		}

		if (type == SF::type::TCP || type == SF::type::SEQPACKET) {
			if (listen(sockfd, _q) < 0) {
				const auto currErrno = errno;
				throw std::runtime_error(net::methods::getErrorMsg(currErrno));
			}
		}
	} catch (...) {
		throw;
	}
}


void Socket::connect(const char _addr[], const int _port, bool *_errorNB)
{
	try {
		switch (domain) {
			case SF::domain::IPv4:
				connect([&](addrIPv4 &s) {
					return net::methods::construct(s, _addr, _port);
				}, _errorNB);
				break;

			case SF::domain::IPv6:
				connect([&](addrIPv6 &s) {
					return net::methods::construct(s, _addr, _port);
				}, _errorNB);
				break;

			case SF::domain::UNIX:
				connect([&](addrUnix &s) {
					return net::methods::construct(s, _addr);
				}, _errorNB);
				break;

			default:
				throw std::invalid_argument("Protocol family not implemented");
		}
	} catch (...) {
		throw;
	}
}


Socket Socket::accept(bool *_errorNB) const
{
	union {
		addrIPv4 ipv4;
		addrIPv6 ipv6;
		addrUnix unix;
		addrStore store;
	};

	std::memset(&store, 0, sizeof(store));

	socklen_t addrSize = 0;
	sockaddr *addrPtr  = nullptr;

	switch (domain) {
		case SF::domain::IPv4:
			ipv4.sin_family = AF_INET;
			std::memset(&ipv4, 0, sizeof(ipv4));
			addrSize = sizeof(ipv4);
			addrPtr  = reinterpret_cast<sockaddr *>(&ipv4);
			break;

		case SF::domain::IPv6:
			ipv6.sin6_family = AF_INET6;
			std::memset(&ipv6, 0, sizeof(ipv6));
			addrSize = sizeof(ipv6);
			addrPtr  = reinterpret_cast<sockaddr *>(&ipv6);
			break;

		case SF::domain::UNIX:
			unix.sun_family = AF_UNIX;
			std::memset(&unix, 0, sizeof(unix));
			addrSize = sizeof(unix);
			addrPtr  = reinterpret_cast<sockaddr *>(&unix);
			break;

		default:
			store.ss_family = static_cast<int>(domain);
			addrSize        = sizeof(store);
			addrPtr         = reinterpret_cast<sockaddr *>(&store);
			break;
	}

	const auto client    = ::accept(sockfd, addrPtr, &addrSize);
	const auto currErrno = errno;

	if (client == -1) {
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

	return Socket(client, domain, type, addrPtr);
}


void Socket::write(const std::string &_msg, bool *_errorNB) const
{
	const auto written = low_write(::write, _msg);

	const auto currErrno = errno;
	if (written == -1) {
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


void Socket::send(const std::string &_msg, SF::send _flags,
                  bool *_errorNB) const
{
	const auto flags = static_cast<int>(_flags);
	const auto sent  = low_write(::send, _msg, flags);

	const auto currErrno = errno;
	if (sent == -1) {
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


std::string Socket::read(const int _numBytes, bool *_errorNB) const
{
	std::string str;
	str.reserve(_numBytes);

	const auto recvd = low_read(::read, str);

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

	return str;
}


std::string Socket::recv(const int _numBytes, SF::recv _flags,
                         bool *_errorNB) const
{
	std::string str;
	str.reserve(_numBytes);

	const auto flags = static_cast<int>(_flags);
	const auto recvd = low_read(::recv, str, flags);

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

	return str;
}


void Socket::setOpt(SF::opt _opType, SF::sockOpt _opValue) const
{
	enum type {
		TIME   = _opValue.TIME,
		LINGER = _opValue.LINGER,
		INT    = _opValue.INT
	};

	void *ptr     = nullptr;
	socklen_t len = 0;


	switch (_opType) {

		case SF::opt::LINGER:
			if (_opValue.getType() != type::LINGER) {
				throw std::invalid_argument("Invalid socket option");
			}
			ptr = (void *) &_opValue.l;
			len = sizeof(_opValue.l);
			break;

		case SF::opt::RCVTIMEO:
		case SF::opt::SNDTIMEO:
			if (_opValue.getType() != type::TIME) {
				throw std::invalid_argument("Invalid socket option");
			}
			ptr = (void *) &_opValue.t;
			len = sizeof(_opValue.t);
			break;

		default:
			if (_opValue.getType() != type::INT) {
				throw std::invalid_argument("Invalid socket option");
			}
			ptr = (void *) &_opValue.i;
			len = sizeof(_opValue.i);
	}

	const auto optname   = static_cast<int>(_opType);
	const auto res       = setsockopt(sockfd, SOL_SOCKET, optname, ptr, len);
	const auto currErrno = errno;
	if (res == -1) {
		throw std::runtime_error(net::methods::getErrorMsg(currErrno));
	}
}


SF::sockOpt Socket::getOpt(SF::opt _opType) const
{
	SF::sockOpt opt;
	void *ptr     = nullptr;
	socklen_t len = 0;

	switch (_opType) {

		case SF::opt::LINGER:
			opt.setLinger(false, 0);
			ptr = (void *) &opt.l;
			len = sizeof(opt.l);
			break;

		case SF::opt::RCVTIMEO:
		case SF::opt::SNDTIMEO:
			opt.setTime(0, 0);
			ptr = (void *) &opt.t;
			len = sizeof(opt.t);
			break;

		default:
			opt.setValue(0);
			ptr = (void *) &opt.i;
			len = sizeof(opt.i);
	}

	const auto optname   = static_cast<int>(_opType);
	const auto res       = getsockopt(sockfd, SOL_SOCKET, optname, ptr, &len);
	const auto currErrno = errno;
	if (res == -1) {
		throw std::runtime_error(net::methods::getErrorMsg(currErrno));
	}

	return opt;
}
}
