#include "socket.hpp"


namespace net {

void Socket::start(const char _addr[], const int _port, const int _q)
{
	try {
		switch (domain) {
			case SF::domain::IPv4:
				bind([&](addrIpv4 &s) {
					return net::methods::construct(s, _addr, _port);
				});
				break;

			case SF::domain::IPv6:
				bind([&](addrIpv6 &s) {
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
				connect([&](addrIpv4 &s) {
					return net::methods::construct(s, _addr, _port);
				}, _errorNB);
				break;

			case SF::domain::IPv6:
				connect([&](addrIpv6 &s) {
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
		addrIpv4 ipv4;
		addrIpv6 ipv6;
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
	const auto written = low_write(::write, sockfd, _msg);

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
	const auto sent  = low_write(::send, sockfd, _msg, flags);

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


std::string Socket::read(const int _bufSize, bool *_errorNB) const
{
	std::string str;
	str.reserve(_bufSize);

	const auto recvd = low_read(::read, sockfd, str);

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


std::string Socket::recv(const int _bufSize, SF::recv _flags,
                         bool *_errorNB) const
{
	std::string str;
	str.reserve(_bufSize);

	const auto flags = static_cast<int>(_flags);
	const auto recvd = low_read(::recv, sockfd, str, flags);

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
}
