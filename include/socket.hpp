#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "socket_family.hpp"
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>


namespace net {

class Socket {
private:
	std::unique_ptr<sockaddr_storage> address;
	int sockfd;
	SF::domain domain;
	SF::type type;

	template <typename Fn, typename... Args>
	auto low_write(Fn &&, const int, const std::string &, Args &&...) const;

	template <typename Fn, typename... Args>
	auto low_read(Fn &&, const int, std::string &, Args &&...) const;

	Socket(const int _sockfd, std::unique_ptr<sockaddr_storage> &_addr)
	{
		sockfd = _sockfd;
		domain = static_cast<SF::domain>(_addr.get()->ss_family);
		address.reset(_addr.release());
	}

	Socket(const Socket &) = delete;
	Socket &operator=(const Socket &) = delete;

public:
	Socket(SF::domain _domain, SF::type _type, int _proto = 0)
	    : domain(_domain), type(_type)
	{
		const auto d = static_cast<int>(domain);
		const auto t = static_cast<int>(type);

		sockfd = socket(d, t, _proto);
		if (sockfd < 0) {
			const auto currErrno = errno;
			throw std::runtime_error(net::methods::getErrorMsg(currErrno));
		}
	}


	Socket(Socket &&s)
	{
		sockfd = s.sockfd;
		domain = s.domain;
		type   = s.type;
		address.reset(s.address.release());
	}

	auto getSocket() const noexcept { return sockfd; }


	// Bind method calls
	template <typename F>
	auto bind(F _fn)
	  -> decltype(_fn(std::declval<sockaddr_in &>()), void()) const
	{
		sockaddr_in addr;

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
	}


	template <typename F>
	auto bind(F _fn)
	  -> decltype(_fn(std::declval<sockaddr_in6 &>()), void()) const
	{
		sockaddr_in6 addr;

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
	}


	template <typename F>
	auto bind(F _fn)
	  -> decltype(_fn(std::declval<sockaddr_un &>()), void()) const
	{
		sockaddr_un addr;

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
	}
	// Bind method calls


	// Connect method calls
	template <typename F>
	auto connect(F _fn, bool *_errorNB = nullptr)
	  -> decltype(_fn(std::declval<sockaddr_in &>()), void()) const
	{
		sockaddr_in addr;

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
	  -> decltype(_fn(std::declval<sockaddr_in6 &>()), void()) const
	{
		sockaddr_in6 addr;

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
	  -> decltype(_fn(std::declval<sockaddr_un &>()), void()) const
	{
		sockaddr_un addr;

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

	void connect(const char[], const int = 0, bool * = nullptr);
	// Connect method calls


	void start(const char[], const int = 0, const int = SOMAXCONN);

	Socket accept(bool * = nullptr) const;


	void write(const std::string &, bool * = nullptr) const;
	std::string read(const int = 1024, bool * = nullptr) const;


	// Send method calls
	void send(
	  const std::string &, SF::send = SF::send::NONE, bool * = nullptr) const;


	template <typename F>
	auto send(const std::string &_msg, F _fn, SF::send _flags = SF::send::NONE,
	  bool *_errorNB = nullptr) const
	  -> decltype(_fn(std::declval<sockaddr_in &>()), void()) const
	{
		sockaddr_in addr;

		const auto flags = static_cast<int>(_flags);
		const auto res   = _fn(addr);

		if (res == 0) {
			throw std::invalid_argument("Address argument invalid");
		}

		ssize_t sent = -1;
		if (res == 1) {
			sent = low_write(
			  ::sendto, sockfd, _msg, flags, (sockaddr *) &addr, sizeof(addr));
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
	  -> decltype(_fn(std::declval<sockaddr_in6 &>()), void()) const
	{
		sockaddr_in6 addr;

		const auto flags = static_cast<int>(_flags);
		const auto res   = _fn(addr);

		if (res == 0) {
			throw std::invalid_argument("Address argument invalid");
		}

		ssize_t sent = -1;
		if (res == 1) {
			sent = low_write(
			  ::sendto, sockfd, _msg, flags, (sockaddr *) &addr, sizeof(addr));
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
	  -> decltype(_fn(std::declval<sockaddr_un &>()), void()) const
	{
		sockaddr_un addr;

		const auto flags = static_cast<int>(_flags);
		const auto res   = _fn(addr);

		if (res == 0) {
			throw std::invalid_argument("Address argument invalid");
		}

		ssize_t sent = -1;
		if (res == 1) {
			sent = low_write(
			  ::sendto, sockfd, _msg, flags, (sockaddr *) &addr, sizeof(addr));
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

	std::string recv(
	  const int = 1024, SF::recv = SF::recv::NONE, bool * = nullptr) const;
	std::string recv(sockaddr_storage &, const int = 1024,
	  SF::recv = SF::recv::NONE, bool * = nullptr) const;


	void setOpt(SF::opt, void *) const;
	void getOpt(SF::opt, void *) const;

	void stop(SF::shut s) const noexcept
	{
		shutdown(sockfd, static_cast<int>(s));
	}

	void close() const noexcept { ::close(sockfd); }

	~Socket()
	{
		// TODO: unlink() the path in case of sockaddr_un
		// unlink(addr.sun_path);
		::close(sockfd);
	}
};


template <typename Fn, typename... Args>
inline auto Socket::low_write(
  Fn &&_fn, const int _sockfd, const std::string &_msg, Args &&... args) const
{
	auto written = 0, count = 0;
	do {
		written = std::forward<Fn>(_fn)(_sockfd, _msg.c_str() + count,
		  _msg.length() - count, std::forward<Args>(args)...);
		count += written;
	} while (written > 0);

	return written;
}


template <typename Fn, typename... Args>
inline auto Socket::low_read(
  Fn &&_fn, const int _sockfd, std::string &_str, Args &&... args) const
{
	ssize_t recvd = 0;
	size_t count  = 0;

	const auto bufSize = _str.capacity();
	const auto buffer  = std::make_unique<char[]>(bufSize + 1);

	do {
		recvd = std::forward<Fn>(_fn)(_sockfd, buffer.get() + count,
		  bufSize - count, std::forward<Args>(args)...);
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
}

#endif
