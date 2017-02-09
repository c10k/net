#include "socket.hpp"

namespace net {

void Socket::bind(const char _addr[], const int _port) const
{
	auto res = -1;

	switch (domain) {

		case SF::domain::IPv4: {
			sockaddr_in servaddr;
			res = net::method::construct(servaddr, domain, _addr, _port);
			if (res == 1) {
				res = ::bind(sockfd, (sockaddr *) &servaddr, sizeof(servaddr));
				res = (res == 0) ? 1 : res;
			}
		} break;

		case SF::domain::IPv6: {
			sockaddr_in6 servaddr;
			res = net::method::construct(servaddr, domain, _addr, _port);
			if (res == 1) {
				res = ::bind(sockfd, (sockaddr *) &servaddr, sizeof(servaddr));
				res = (res == 0) ? 1 : res;
			}
		} break;

		default: throw std::logic_error("Protocol yet not implemented");
	}

	if (res == -1) {
		throw std::runtime_error("Error while binding address");
	} else if (res == 0) {
		throw std::invalid_argument("Address argument invalid");
	}
}


void Socket::start(const char _addr[], const int _port, const int _q) const
{
	try {
		bind(_addr, _port);

		if (type == SF::type::TCP || type == SF::type::SCTP) {
			if (listen(sockfd, _q) < 0) {
				throw std::runtime_error("Error on call to listen");
			}
		}
	} catch (...) {
		throw;
	}
}

void Socket::connect(const char _addr[], const int _port) const
{
	auto res = -1;
	switch (domain) {
		case SF::domain::IPv4: {
			sockaddr_in addr;
			res = net::method::construct(addr, domain, _addr, _port);
			if (res == 1) {
				res = ::connect(sockfd, (sockaddr *) &addr, sizeof(addr));
				res = (res == 0) ? 1 : res;
			}
		} break;

		case SF::domain::IPv6: {
			sockaddr_in6 addr;
			res = net::method::construct(addr, domain, _addr, _port);
			if (res == 1) {
				res = ::connect(sockfd, (sockaddr *) &addr, sizeof(addr));
				res = (res == 0) ? 1 : res;
			}
		} break;

		default: throw std::logic_error("Protocol yet not implemented");
	}

	// TODO: Check for error == EINPROGRESS and don't throw exception in that
	// case.

	if (res == -1) {
		throw std::runtime_error("Error while connecting");
	} else if (res == 0) {
		throw std::invalid_argument("Address argument invalid");
	}
}

Socket Socket::accept() const
{
	sockaddr_storage peerAddr;
	socklen_t peerAddrSize = sizeof(peerAddr);
	std::memset(&peerAddr, 0, sizeof(peerAddr));

	auto client = ::accept(sockfd, (sockaddr *) &peerAddr, &peerAddrSize);

	if (client == -1) {
		// TODO: Check for client == EAGAIN or EWOULDBLOCK and
		// don't throw exception in that case.
		throw std::runtime_error("Error while accepting connection");
	}

	return Socket(client, peerAddr);
}


void Socket::write(const std::string msg) const
{
	auto written = 0, count = 0;
	do {
		written = ::write(sockfd, msg.c_str() + count, msg.length() - count);
		count += written;
	} while (written > 0 || count < msg.length());

	if (written == -1) {
		// TODO: Check for written == EAGAIN or EWOULDBLOCK and
		// don't throw exception in that case.
		throw std::runtime_error("Error occurred while writing message");
	}
}

std::string Socket::read(const int bufSize) const
{
	auto buffer = std::make_unique<char[]>(bufSize);
	auto bytes = 0, count = 0;

	std::string str;
	str.reserve(bufSize);

	do {
		bytes = ::read(sockfd, buffer.get() + count, bufSize - count);
		count += bytes;
		if (count == bufSize) {
			str.append(buffer.get());
			str.reserve(str.length() + bufSize);
			std::memset(buffer.get(), 0, bufSize);
			count = 0;
		}
	} while (bytes > 0);

	if (bytes == -1) {
		// TODO: Check for written == EAGAIN or EWOULDBLOCK and
		// don't throw exception in that case.
		throw std::runtime_error("Error occurred while writing message");
	}

	return str;
}
}
