#include "socket.hpp"

namespace net {


void Socket::start(const char _addr[], const int _port, const int _q)
{
	try {
		switch (domain) {
			case SF::domain::IPv4:
				bind([&](sockaddr_in &s) {
					return net::methods::construct(s, _addr, _port);
				});
				break;

			case SF::domain::IPv6:
				bind([&](sockaddr_in6 &s) {
					return net::methods::construct(s, _addr, _port);
				});
				break;

			case SF::domain::UNIX:
				bind([&](sockaddr_un &s) {
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
				connect([&](sockaddr_in &s) {
					return net::methods::construct(s, _addr, _port);
				});
				break;

			case SF::domain::IPv6:
				connect([&](sockaddr_in6 &s) {
					return net::methods::construct(s, _addr, _port);
				});
				break;

			case SF::domain::UNIX:
				connect([&](sockaddr_un &s) {
					return net::methods::construct(s, _addr);
				});
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
	auto peerAddr = std::make_unique<sockaddr_storage>();

	socklen_t peerAddrSize = sizeof(*peerAddr.get());
	std::memset(peerAddr.get(), 0, peerAddrSize);

	auto client = ::accept(sockfd, (sockaddr *) peerAddr.get(), &peerAddrSize);

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

	return Socket(client, peerAddr);
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


void Socket::send(
  const std::string &_msg, SF::send _flags, bool *_errorNB) const
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


std::string Socket::recv(
  const int _bufSize, SF::recv _flags, bool *_errorNB) const
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


std::string Socket::recv(sockaddr_storage &_addr, const int _bufSize,
  SF::recv _flags, bool *_errorNB) const
{
	std::string str;
	str.reserve(_bufSize);

	const auto flags = static_cast<int>(_flags);
	socklen_t length = sizeof(_addr);
	const auto recvd
	  = low_read(::recvfrom, sockfd, str, flags, (sockaddr *) &_addr, &length);

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
