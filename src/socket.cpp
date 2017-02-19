#include "socket.hpp"

namespace net {

void Socket::bind(const char _addr[], const int _port) const
{
	auto res = -1;
	switch (domain) {

		case SF::domain::IPv4: {
			sockaddr_in servaddr;
			res = net::method::construct(servaddr, _addr, _port);
			if (res == 1) {
				res = ::bind(sockfd, (sockaddr *) &servaddr, sizeof(servaddr));
				res = (res == 0) ? 1 : res;
			}
		} break;

		case SF::domain::IPv6: {
			sockaddr_in6 servaddr;
			res = net::method::construct(servaddr, _addr, _port);
			if (res == 1) {
				res = ::bind(sockfd, (sockaddr *) &servaddr, sizeof(servaddr));
				res = (res == 0) ? 1 : res;
			}
		} break;

		case SF::domain::UNIX: {
			sockaddr_un servaddr;
			res = net::method::construct(servaddr, _addr);
			unlink(servaddr.sun_path);
			res = ::bind(sockfd, (sockaddr *) &servaddr, sizeof(servaddr));
			res = (res == 0) ? 1 : res;
		} break;

		default: throw std::logic_error("Protocol yet not implemented");
	}

	const auto currErrno = errno;
	if (res == -1) {
		throw std::runtime_error(net::method::getErrorMsg(currErrno));
	} else if (res == 0) {
		throw std::invalid_argument("Address argument invalid");
	}
}


void Socket::start(const char _addr[], const int _port, const int _q) const
{
	try {
		bind(_addr, _port);

		if (type == SF::type::TCP || type == SF::type::SEQPACKET) {
			if (listen(sockfd, _q) < 0) {
				const auto currErrno = errno;
				throw std::runtime_error(net::method::getErrorMsg(currErrno));
			}
		}
	} catch (...) {
		throw;
	}
}


void Socket::connect(const char _addr[], const int _port, bool *_errorNB) const
{
	auto res = -1;
	switch (domain) {
		case SF::domain::IPv4: {
			sockaddr_in addr;
			res = net::method::construct(addr, _addr, _port);
			if (res == 1) {
				res = ::connect(sockfd, (sockaddr *) &addr, sizeof(addr));
				res = (res == 0) ? 1 : res;
			}
		} break;

		case SF::domain::IPv6: {
			sockaddr_in6 addr;
			res = net::method::construct(addr, _addr, _port);
			if (res == 1) {
				res = ::connect(sockfd, (sockaddr *) &addr, sizeof(addr));
				res = (res == 0) ? 1 : res;
			}
		} break;

		case SF::domain::UNIX: {
			sockaddr_un addr;
			res = net::method::construct(addr, _addr);
			res = ::connect(sockfd, (sockaddr *) &addr, sizeof(addr));
			res = (res == 0) ? 1 : res;
		} break;

		default: throw std::logic_error("Protocol yet not implemented");
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
			throw std::runtime_error(net::method::getErrorMsg(currErrno));
		}
	} else if (res == 0) {
		throw std::invalid_argument("Address argument invalid");
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
			throw std::runtime_error(net::method::getErrorMsg(currErrno));
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
			throw std::runtime_error(net::method::getErrorMsg(currErrno));
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
			throw std::runtime_error(net::method::getErrorMsg(currErrno));
		}
	}
}


void Socket::send(const std::string &_msg, sockaddr_storage &_addr,
  SF::send _flags, bool *_errorNB) const
{
	const auto flags = static_cast<int>(_flags);
	const auto sent  = low_write(
	  ::sendto, sockfd, _msg, flags, (sockaddr *) &_addr, sizeof(_addr));

	const auto currErrno = errno;
	if (sent == -1) {
		if (currErrno == EAGAIN || currErrno == EWOULDBLOCK) {
			if (_errorNB != nullptr) {
				*_errorNB = true;
			} else {
				throw std::invalid_argument("errorNB argument missing");
			}
		} else {
			throw std::runtime_error(net::method::getErrorMsg(currErrno));
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
			throw std::runtime_error(net::method::getErrorMsg(currErrno));
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
			throw std::runtime_error(net::method::getErrorMsg(currErrno));
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
			throw std::runtime_error(net::method::getErrorMsg(currErrno));
		}
	}

	return str;
}
}
