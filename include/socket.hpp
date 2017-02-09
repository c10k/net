#include "socket_family.hpp"
#include <memory>
#include <stdexcept>
#include <string>
#include <unistd.h>

namespace net {

class Socket {
private:
	sockaddr_storage address;
	int sockfd;
	SF::domain domain;
	SF::type type;

	Socket &operator=(const Socket &) = delete;

public:
	Socket(SF::domain _domain, SF::type _type, int _proto = 0)
	    : domain(_domain), type(_type)
	{
		std::memset(&address, 0, sizeof(address));
		const auto d = static_cast<int>(domain);
		const auto t = static_cast<int>(type);

		// TODO: fill sockaddr_storage address

		sockfd = socket(d, t, _proto);
		if (sockfd < 0) {
			throw std::runtime_error("Something went wrong with socket call");
		}
	}

	Socket(const int _sockfd, const sockaddr_storage &_addr)
	{
		sockfd  = _sockfd;
		address = _addr;

		// TODO: fill domain and type information
	}

	void bind(const char[], const int = 0) const;
	void start(const char[], const int = 0, const int = SOMAXCONN) const;
	void connect(const char[], const int = 0) const;
	Socket accept() const;

	void write(std::string) const;
	std::string read(const int = 1024) const;

	void set(/* options */) const {}
	void get(/* options */) const {}

	void stop(SF::shut s) const noexcept
	{
		shutdown(sockfd, static_cast<int>(s));
	}

	void close() const noexcept { ::close(sockfd); }

	~Socket() { ::close(sockfd); }
};
}
