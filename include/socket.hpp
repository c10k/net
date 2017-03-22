#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "socket_family.hpp"
#include <memory>
#include <stdexcept>
#include <string>


namespace net {

/**
* @class net::Socket
* Socket class to create Berkeley sockets.
* Uses socket domains from domain enum in SF namespace from socket_family.hpp
* Uses socket types from type enum in SF namespace from socket_family.hpp
*/
class Socket final {
private:
	union {
		AddrStore store;
		AddrIPv4 ipv4;
		AddrIPv6 ipv6;
		AddrUnix unix;
	};
	int sockfd;
	Domain sock_domain;
	Type sock_type;


	/**
	* @method low_write
	* @access private
	* Writes given _msg using _sockfd by calling _fn with args having flags and
	* destination socket address.
	*
	* @param {callable} _fn Some callable that writes using socket descriptor.
	* @param {string} _msg Msg to write on sockfd.
	* @param {parameter_pack} args Flags, destination sockaddr objects and their
	* lengths.
	* @returns {ssize_t} Status of writing _msg using socket descriptor / Number
	* of bytes written using socket descriptor.
	*/
	template <typename Fn, typename... Args>
	auto low_write(Fn &&_fn, const std::string &_msg, Args &&... args) const
	{
		ssize_t written              = 0;
		std::string::size_type count = 0;
		do {
			written = std::forward<Fn>(_fn)(sockfd, _msg.c_str() + count,
			                                _msg.length() - count,
			                                std::forward<Args>(args)...);
			count += written;
		} while (count < _msg.length() && written > 0);

		return written;
	}


	/**
	* @method low_read
	* @access private
	* Reads using sockfd by calling _fn  with args having flags and destination
	* socket address.
	*
	* @param {callable} _fn Some callable that reads using socket descriptor.
	* @param {string} _str String to store the data.
	* @param {parameter_pack} args Flags, destination sockaddr objects and their
	* lengths.
	* @returns {ssize_t} Status of reading data using socket descriptor / Number
	* of bytes read using socket descriptor.
	*/
	template <typename Fn, typename... Args>
	auto low_read(Fn &&_fn, std::string &_str, Args &&... args) const
	{
		const auto bufSize = _str.capacity();
		const auto buffer  = std::make_unique<char[]>(bufSize);

		const auto recvd = std::forward<Fn>(_fn)(sockfd, buffer.get(), bufSize,
		                                         std::forward<Args>(args)...);
		_str.append(buffer.get(), (recvd > 0) ? recvd : 0);
		return recvd;
	}


	/**
	* @construct Socket
	* @access private
	* @param {int} _sockfd Descriptor representing a socket.
	* @param {Domain} _domain Socket domain.
	* @param {Type} _domain Socket type.
	* @param {void *} _addr Pointer to initialize appropriate member of Union of
	* net::Socket.
	*/
	Socket(const int, Domain, Type, const void *);

	Socket(const Socket &) = delete;
	Socket &operator=(const Socket &) = delete;


public:
	/**
	* @construct net::Socket
	* @access public
	* @param {domain} _domain Socket domain.
	* @param {type} _type Socket type.
	* @param {int} _proto Socket protocol.
	*/
	Socket(Domain _domain, Type _type, const int _proto = 0)
	    : sock_domain(_domain), sock_type(_type)
	{
		const auto d = static_cast<int>(sock_domain);
		const auto t = static_cast<int>(sock_type);

		sockfd = socket(d, t, _proto);
		if (sockfd < 0) {
			const auto currErrno = errno;
			throw std::runtime_error(net::methods::getErrorMsg(currErrno));
		}

		switch (sock_domain) {
			case Domain::IPv4: ipv4.sin_family  = AF_INET; break;
			case Domain::IPv6: ipv6.sin6_family = AF_INET6; break;
			case Domain::UNIX: unix.sun_family  = AF_UNIX; break;

			default: store.ss_family = d;
		}
	}


	/**
	* @construct net::Socket using another net::Socket.
	* @access public
	* @param {Socket} s Rvalue of type socket.
	*/
	Socket(Socket &&s)
	{
		sockfd      = s.sockfd;
		sock_domain = s.sock_domain;
		sock_type   = s.sock_type;

		switch (s.sock_domain) {
			case Domain::IPv4: ipv4 = s.ipv4; break;
			case Domain::IPv6: ipv6 = s.ipv6; break;
			case Domain::UNIX: unix = s.unix; break;

			default: store = s.store;
		}
	}


	/**
	* @method getSocket
	* @access public
	* Get the socket descriptor in net::Socket.
	*
	* @returns {int} Socket descriptor for net::Socket.
	*/
	auto getSocket() const noexcept { return sockfd; }


	/**
	* @method getDomain
	* @access public
	* Get the Domain type of Socket.
	*
	* @returns {Domain} for net::Socket.
	*/
	auto getDomain() const noexcept { return sock_domain; }


	/**
	* @method getType
	* @access public
	* Get the Protocol Type of Socket.
	*
	* @returns {Type} for net::Socket.
	*/
	auto getType() const noexcept { return sock_type; }


	/**
	* @method bind
	* @access public
	* Binds net::Socket to local address if successful else if Address argument
	* is invalid then throws invalid_argument exception
	* else throws runtime_error exception signalling that bind failed. Invokes
	* the callable provided to fill AddrIPv4 object.
	*
	* @param {callable} _fn Some callable that takes arg of type AddrIPv4.
	*/
	template <typename F>
	auto bind(F _fn) -> decltype(_fn(std::declval<AddrIPv4 &>()), void()) const
	{
		AddrIPv4 addr;

		auto res = _fn(addr);
		if (res >= 1) {
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


	/**
	* @method bind
	* @access public
	* Binds net::Socket to local address if successful else if Address argument
	* is invalid then throws invalid_argument exception
	* else throws runtime_error exception signalling that bind failed. Invokes
	* the callable provided to fill AddrIPv6 object.
	*
	* @param {callable} _fn Some callable that takes arg of type AddrIPv6.
	*/
	template <typename F>
	auto bind(F _fn) -> decltype(_fn(std::declval<AddrIPv6 &>()), void()) const
	{
		AddrIPv6 addr;

		auto res = _fn(addr);
		if (res >= 1) {
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


	/**
	* @method bind
	* @access public
	* Binds net::Socket to local address if successful else if Address argument
	* is invalid then throws invalid_argument exception
	* else throws runtime_error exception signalling that bind failed. Invokes
	* the callable provided to fill AddrUnix object.
	*
	* @param {callable} _fn Some callable that takes arg of type AddrUnix.
	*/
	template <typename F>
	auto bind(F _fn) -> decltype(_fn(std::declval<AddrUnix &>()), void()) const
	{
		AddrUnix addr;

		auto res = _fn(addr);
		if (res >= 1) {
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


	/**
	* @method connect
	* @access public
	* Connects net::Socket to address _addr:_port if successful else throws
	* invalid_argument exception.
	*
	* @param {char []} _addr Ip address in case of ipv4 or ipv6 domain, and Path
	* in case of unix domain.
	* @param {int} _port Port number in case of AddrIPv4 or AddrIPv6.
	* @param {bool *} _errorNB To signal error in case of non-blocking connect.
	*/
	void connect(const char[], const int = 0, bool * = nullptr);


	/**
	* @method connect
	* @access public
	* Connects net::Socket to ipv4 peer if successful else throws runtime_error
	* exception.
	* Throws invalid_argument exception in case of non-blocking net::Socket if
	* _errorNB is missing.
	* Throws invalid_argument exception if destination address given is invalid.
	*
	* @param {callable} _fn Some callable that takes arg of type AddrIPv4.
	* @param {bool *} _errorNB To signal error in case of non-blocking connect.
	*/
	template <typename F>
	auto connect(F _fn, bool *_errorNB = nullptr)
	  -> decltype(_fn(std::declval<AddrIPv4 &>()), void()) const
	{
		AddrIPv4 addr;

		auto res = _fn(addr);
		if (res >= 1) {
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


	/**
	* @method connect
	* @access public
	* Connects net::Socket to ipv6 peer if successful else throws runtime_error
	* exception.
	* Throws invalid_argument exception in case of non-blocking net::Socket if
	* _errorNB is missing.
	* Throws invalid_argument exception if destination address given is invalid.
	* Invokes the callable provided to fill AddrIPv4 object.
	*
	* @param {callable} _fn Some callable that takes arg of type AddrIPv6.
	* @param {bool *} _errorNB To signal error in case of non-blocking connect.
	*/
	template <typename F>
	auto connect(F _fn, bool *_errorNB = nullptr)
	  -> decltype(_fn(std::declval<AddrIPv6 &>()), void()) const
	{
		AddrIPv6 addr;

		auto res = _fn(addr);
		if (res >= 1) {
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


	/**
	* @method connect
	* @access public
	* Connects net::Socket to unix socket peer if successful else throws
	* runtime_error exception.
	* Throws invalid_argument exception in case of non-blocking net::Socket if
	* _errorNB is missing.
	* Throws invalid_argument exception if destination address given is invalid.
	* Invokes the callable provided to fill AddrIPv4 object.
	*
	* @param {callable} _fn Some callable that takes arg of type AddrUnix.
	* @param {bool *} _errorNB To signal error in case of non-blocking connect.
	*/
	template <typename F>
	auto connect(F _fn, bool *_errorNB = nullptr)
	  -> decltype(_fn(std::declval<AddrUnix &>()), void()) const
	{
		AddrUnix addr;

		auto res = _fn(addr);
		if (res >= 1) {
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


	/**
	* @method start
	* @access public
	* Starts the net::Socket in listen mode on given ip address and given port
	* with given backlog if successful
	* else throws runtime_error exception.
	* Throws invalid_argument exception if given ip address or port are not
	* valid.
	*
	* @param {char []} _addr Ip address in case of ipv4 or ipv6 domain, and Path
	* in case of unix domain.
	* @param {int} _port Port number in case of ipv4 or ipv6.
	* @param {int} _q Size of backlog of listening socket.
	*/
	void start(const char[], const int = 0, const int = SOMAXCONN);


	/**
	* @method accept
	* @access public
	* Returns Socket object from connected sockets queue if successful else
	* throws runtime_error exception.
	* Throws invalid_argument exception in case of non-blocking net::Socket if
	* _errorNB is missing.
	*
	* @param {bool *} _errorNB To signal error in case of non-blocking accept.
	* @returns {net::Socket}
	*/
	Socket accept(bool * = nullptr) const;


	/**
	* @method write
	* @access public
	* Writes given string to Socket if successful else throws runtime_error
	* exception.
	* Throws invalid_argument exception in case of non-blocking net::Socket if
	* _errorNB is missing.
	*
	* @param {string} _msg String to be written to Socket.
	* @param {bool *} _errorNB To signal error in case of non-blocking write.
	*/
	void write(const std::string &, bool * = nullptr) const;


	/**
	* @method read
	* @access public
	* Reads given number of bytes using Socket if successful else throws
	* runtime_error exception.
	* Throws invalid_argument exception in case of non-blocking net::Socket if
	* _errorNB is missing.
	*
	* @param {int} _bufSize Number of bytes to be read using Socket.
	* @param {bool *} _errorNB To signal error in case of non-blocking read.
	* @returns {string} String of _bufSize bytes read using Socket.
	*/
	std::string read(const int, bool * = nullptr) const;


	/**
	* @method send
	* @access public
	* Sends given string using Socket if successful else throws runtime_error
	* exception.
	* Throws invalid_argument exception in case of non-blocking net::Socket if
	* _errorNB is missing.
	*
	* @param {string} _msg String to be sent using Socket.
	* @param {send} _flags Modify default behaviour of send.
	* @param {bool *} _errorNB To signal error in case of non-blocking send.
	*/
	void send(const std::string &, Send = Send::NONE, bool * = nullptr) const;


	/**
	* @method send
	* @access public
	* Sends given string using Socket if successful else throws runtime_error
	* exception.
	* Throws invalid_argument exception in case of non-blocking net::Socket if
	* _errorNB is missing.
	* Throws invalid_argument exception if destination address given is invalid.
	* Invokes the callable provided to fill AddrIPv4 object.
	*
	* @param {string} _msg String to be sent using Socket.
	* @param {callable} _fn Some callable that takes arg of type AddrIPv4 or
	* void.
	* @param {send} _flags Modify default behaviour of send.
	* @param {bool *} _errorNB To signal error in case of non-blocking send.
	*/
	template <typename F>
	auto send(const std::string &_msg, F _fn, Send _flags = Send::NONE,
	          bool *_errorNB = nullptr) const
	  -> decltype(_fn(std::declval<AddrIPv4 &>()), void()) const
	{
		AddrIPv4 addr;

		const auto flags = static_cast<int>(_flags);
		const auto res   = _fn(addr);

		if (res == 0) {
			throw std::invalid_argument("Address argument invalid");
		}

		ssize_t sent = -1;
		if (res >= 1) {
			sent = low_write(::sendto, _msg, flags, (sockaddr *) &addr,
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


	/**
	* @method send
	* @access public
	* Sends given string using Socket if successful else throws runtime_error
	* exception.
	* Throws invalid_argument exception in case of non-blocking net::Socket if
	* _errorNB is missing.
	* Throws invalid_argument exception if destination address given is invalid.
	* Invokes the callable provided to fill AddrIPv6 object.
	*
	* @param {string} _msg String to be sent using Socket.
	* @param {callable} _fn Some callable that takes arg of type AddrIPv6 or
	* void.
	* @param {send} _flags Modify default behaviour of send.
	* @param {bool *} _errorNB To signal error in case of non-blocking send.
	*/
	template <typename F>
	auto send(const std::string &_msg, F _fn, Send _flags = Send::NONE,
	          bool *_errorNB = nullptr) const
	  -> decltype(_fn(std::declval<AddrIPv6 &>()), void()) const
	{
		AddrIPv6 addr;

		const auto flags = static_cast<int>(_flags);
		const auto res   = _fn(addr);

		if (res == 0) {
			throw std::invalid_argument("Address argument invalid");
		}

		ssize_t sent = -1;
		if (res >= 1) {
			sent = low_write(::sendto, _msg, flags, (sockaddr *) &addr,
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


	/**
	* @method send
	* @access public
	* Sends given string using Socket if successful else throws runtime_error
	* exception.
	* Throws invalid_argument exception in case of non-blocking net::Socket if
	* _errorNB is missing.
	* Throws invalid_argument exception if destination address given is invalid.
	* Invokes the callable provided to fill AddrUnix object.
	*
	* @param {string} _msg String to be sent using Socket.
	* @param {callable} _fn Some callable that takes arg of type AddrUnix or
	* void.
	* @param {send} _flags Modify default behaviour of send.
	* @param {bool *} _errorNB To signal error in case of non-blocking send.
	*/
	template <typename F>
	auto send(const std::string &_msg, F _fn, Send _flags = Send::NONE,
	          bool *_errorNB = nullptr) const
	  -> decltype(_fn(std::declval<AddrUnix &>()), void()) const
	{
		AddrUnix addr;

		const auto flags = static_cast<int>(_flags);
		const auto res   = _fn(addr);

		if (res == 0) {
			throw std::invalid_argument("Address argument invalid");
		}

		ssize_t sent = -1;
		if (res >= 1) {
			sent = low_write(::sendto, _msg, flags, (sockaddr *) &addr,
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


	/**
	* @method recv
	* @access public
	* Reads given number of bytes using Socket if successful else throws
	* runtime_error exception.
	* Throws invalid_argument exception in case of non-blocking net::Socket if
	* _errorNB is missing.
	*
	* @param {int} _bufSize Number of bytes to be read using Socket.
	* @param {recv} _flags Modify default behaviour of recv.
	* @param {bool *} _errorNB To signal error in case of non-blocking recv.
	* @returns {string} String of _bufSize bytes read from socket.
   */
	std::string recv(const int, Recv = Recv::NONE, bool * = nullptr) const;


	/**
	* @method recv
	* @access public
	* Reads given number of bytes using Socket if successful else throws
	* runtime_error exception.
	* Throws invalid_argument exception in case of non-blocking net::Socket if
	* _errorNB is missing.
	* Throws invalid_argument exception if destination address given is invalid.
	* Invokes the callable provided to return AddrIPv4 object from where msg has
	* been received.
	*
	* @param {int} _numBytes Number of bytes to read.
	* @param {callable} _fn Some callable that takes arg of type AddrIPv4 or
	* void.
	* @param {recv} _flags Modify default behaviour of recv.
	* @param {bool *} _errorNB To signal error in case of non-blocking recv.
	*/
	template <typename F>
	auto recv(const int _numBytes, F _fn, Recv _flags = Recv::NONE,
	          bool *_errorNB = nullptr) const
	  -> decltype(_fn(std::declval<AddrIPv4 &>()), std::string()) const
	{
		AddrIPv4 addr;
		std::string str;
		str.reserve(_numBytes);

		const auto flags = static_cast<int>(_flags);
		socklen_t length = sizeof(addr);

		const auto recvd
		  = low_read(::recvfrom, str, flags, (sockaddr *) &addr, &length);

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


	/**
	* @method recv
	* @access public
	* Reads given number of bytes using Socket if successful else throws
	* runtime_error exception.
	* Throws invalid_argument exception in case of non-blocking net::Socket if
	* _errorNB is missing.
	* Throws invalid_argument exception if destination address given is invalid.
	* Invokes the callable provided to return AddrIPv6 object from where msg has
	* been received.
	*
	* @param {int} _numBytes Number of bytes to read.
	* @param {callable} _fn Some callable that takes arg of type AddrIPv6 or
	* void.
	* @param {recv} _flags Modify default behaviour of recv.
	* @param {bool *} _errorNB To signal error in case of non-blocking recv.
	*/
	template <typename F>
	auto recv(const int _numBytes, F _fn, Recv _flags = Recv::NONE,
	          bool *_errorNB = nullptr) const
	  -> decltype(_fn(std::declval<AddrIPv6 &>()), std::string()) const
	{
		AddrIPv6 addr;
		std::string str;
		str.reserve(_numBytes);

		const auto flags = static_cast<int>(_flags);
		socklen_t length = sizeof(addr);

		const auto recvd
		  = low_read(::recvfrom, str, flags, (sockaddr *) &addr, &length);

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


	/**
	* @method recv
	* @access public
	* Reads given number of bytes using Socket if successful else throws
	* runtime_error exception.
	* Throws invalid_argument exception in case of non-blocking net::Socket if
	* _errorNB is missing.
	* Throws invalid_argument exception if destination address given is invalid.
	* Invokes the callable provided to return AddrUnix object from where msg has
	* been received.
	*
	* @param {int} _numBytes Number of bytes to read.
	* @param {callable} _fn Some callable that takes arg of type AddrUnix or
	* void.
	* @param {recv} _flags Modify default behaviour of recv.
	* @param {bool *} _errorNB To signal error in case of non-blocking recv.
	*/
	template <typename F>
	auto recv(const int _numBytes, F _fn, Recv _flags = Recv::NONE,
	          bool *_errorNB = nullptr) const
	  -> decltype(_fn(std::declval<AddrUnix &>()), std::string()) const
	{
		AddrUnix addr;
		std::string str;
		str.reserve(_numBytes);

		const auto flags = static_cast<int>(_flags);
		socklen_t length = sizeof(addr);

		const auto recvd
		  = low_read(::recvfrom, str, flags, (sockaddr *) &addr, &length);

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


	/**
	* @method setOpt
	* @access public
	* Set a socket option from net::Opt for Socket using object of type
	* net::SockOpt.
	*
	* @param {net::Opt} _opType Option to set for Socket.
	* @param {net::SockOpt} _opValue socket option structure. Present inside
	* socket_family.hpp.
	*/
	void setOpt(Opt, SockOpt) const;


	/**
	* @method getOpt
	* @access public
	* Get value of some socket option for Socket.
	*
	* @param {net::Opt} _opType Option of Socket whose value to get.
	*/
	SockOpt getOpt(Opt) const;


	/**
	* @method stop
	* @access public
	* Shutdown Socket using net::shut.
	*
	* @param {net::shut} _s Option specifying which side of connection to
	* shutdown for Socket.
	*/
	void stop(Shut _s) const noexcept
	{
		shutdown(sockfd, static_cast<int>(_s));
	}


	/**
	* method unlink
	* @access public
	* Unlinks the unix socket path.
	*/
	void unlink() const noexcept
	{
		if (sock_domain == Domain::UNIX) {
			::unlink(unix.sun_path);
		}
	}


	/**
	* method close
	* @access public
	* Closes the Socket for terminating connection.
	*/
	void close() const noexcept { ::close(sockfd); }


	~Socket()
	{
		if (sock_domain == Domain::UNIX) {
			::unlink(unix.sun_path);
		}
		::close(sockfd);
	}
};
}

#endif
