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
			throw std::runtime_error("Something went wrong with socket call");
		}
	}

	Socket(const int _sockfd, std::unique_ptr<sockaddr_storage> &_addr)
	{
		sockfd = _sockfd;
		domain = static_cast<SF::domain>(_addr.get()->ss_family);
		address.reset(_addr.release());
	}

	Socket(Socket &&s)
	{
		sockfd = s.sockfd;
		domain = s.domain;
		type   = s.type;
		address.reset(s.address.release());
	}


	void bind(const char[], const int = 0) const;
	void start(const char[], const int = 0, const int = SOMAXCONN) const;
	void connect(const char[], const int = 0, bool * = nullptr) const;
	Socket accept(bool * = nullptr) const;


	void write(const std::string &, bool * = nullptr) const;
	std::string read(const int = 1024, bool * = nullptr) const;

	void send(
	  const std::string &, SF::send = SF::send::NONE, bool * = nullptr) const;
	void send(const std::string &, sockaddr_storage &,
	  SF::send = SF::send::NONE, bool * = nullptr) const;

	std::string recv(
	  const int = 1024, SF::recv = SF::recv::NONE, bool * = nullptr) const;
	std::string recv(sockaddr_storage &, const int = 1024,
	  SF::recv = SF::recv::NONE, bool * = nullptr) const;


	void set(/* options */) const {}
	void get(/* options */) const {}

	void stop(SF::shut s) const noexcept
	{
		shutdown(sockfd, static_cast<int>(s));
	}

	void close() const noexcept { ::close(sockfd); }

	~Socket() { ::close(sockfd); }
};


template <typename Fn, typename... Args>
inline auto Socket::low_write(
  Fn &&fn, const int _sockfd, const std::string &_msg, Args &&... args) const
{
	auto written = 0, count = 0;
	do {
		written = std::forward<Fn>(fn)(_sockfd, _msg.c_str() + count,
		  _msg.length() - count, std::forward<Args>(args)...);
		count += written;
	} while (written > 0);

	return written;
}


template <typename Fn, typename... Args>
inline auto Socket::low_read(
  Fn &&fn, const int _sockfd, std::string &_str, Args &&... args) const
{
	ssize_t recvd = 0;
	size_t count  = 0;

	const auto bufSize = _str.capacity();
	const auto buffer  = std::make_unique<char[]>(bufSize + 1);

	do {
		recvd = std::forward<Fn>(fn)(_sockfd, buffer.get() + count,
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
