#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "socket_family.hpp"
#include <memory>
#include <stdexcept>
#include <string>


namespace net {

/**
* @class net::Socket
* Socket class to create POSIX sockets.
* Uses socket domains from domain enum in SF namespace from socket_family.hpp
* Uses socket types from type enum in SF namespace from socket_family.hpp
*/
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

	/** @method low_write
	* @access private
	* Writes given _msg using _sockfd by calling _fn with args having flags and
	* destination socket address.
	*
	* @param {callable} _fn Some callable that writes using socket descriptor.
	* @param {int} _sockfd Descriptor representing a socket.
	* @param {string} _msg Msg to write on _sockfd.
	* @param {parameter_pack} args Flags, destination sockaddr objects and their
	* lengths.
	* @returns {ssize_t} Status of writing _msg using socket descriptor / Number
	* of bytes written using socket descriptor.
	*/
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

	/** @method low_read
	* @access private
	* Reads using _sockfd by calling _fn  with args having flags and destination
	* socket address.
	*
	* @param {callable} _fn Some callable that reads using socket descriptor.
	* @param {int} _sockfd Descriptor representing a socket.
	* @param {string} _str String to store the data.
	* @param {parameter_pack} args Flags, destination sockaddr objects and their
	* lengths.
	* @returns {ssize_t} Status of reading data using socket descriptor / Number
	* of bytes read using socket descriptor.
	*/
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

	/** @construct Socket
	* @access private
	* @param {int} _sockfd Descriptor representing a socket.
	* @param {SF::domain} _domain Socket domain.
	* @param {void *} _addr Pointer to initialize appropriate member of Union of
	* net::Socket.
	*/
	Socket(const int, SF::domain, SF::type, const void *);

	Socket(const Socket &) = delete;
	Socket &operator=(const Socket &) = delete;

public:
	/** @construct net::Socket.
	* @access public
	* @param {SF::domain} _domain Socket domain.
	* @param {SF::type} _type Socket type.
	* @param {int} _proto Socket protocol.
	*/
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

	/** @construct net::Socket using another net::Socket.
	* @access public
	* @param {Socket} s Rvalue of type socket.
	*/
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

	/** @method getSocket
	* @access public
	* Get the socket descriptor in net::Socket.
	*
	* @returns {int} Socket descriptor for net::Socket.
	*/
	auto getSocket() const noexcept { return sockfd; }


	// Bind method calls

	/** @method bind
	* @access public
	* Binds net::Socket to local address if successful else if Address argument
	* is invalid then throws invalid_argument exception
	* else throws runtime_error exception signalling that bind failed. Invokes
	* the callable provided to fill addrIpv4 object.
	*
	* @param {callable} _fn Some callable that takes arg of type addrIpv4 or
	* void.
	*/
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

	/** @method bind
	* @access public
	* Binds net::Socket to local address if successful else if Address argument
	* is invalid then throws invalid_argument exception
	* else throws runtime_error exception signalling that bind failed. Invokes
	* the callable provided to fill addrIpv6 object.
	*
	* @param {callable} _fn Some callable that takes arg of type addrIpv6 or
	* void.
	*/
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

	/** @method bind
	* @access public
	* Binds net::Socket to local address if successful else if Address argument
	* is invalid then throws invalid_argument exception
	* else throws runtime_error exception signalling that bind failed. Invokes
	* the callable provided to fill addrUnix object.
	*
	* @param {callable} _fn Some callable that takes arg of type addrUnix or
	* void.
	*/
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

	/** @method connect
	* @access public
	* Connects net::Socket to address _addr:_port if successful else throws
	* invalid_argument exception.
	*
	* @param {char []} _addr Ip address in case of ipv4 or ipv6 domain, and Path
	* in case of unix domain.
	* @param {int} _port Port number in case of addrIpv4 or addrIpv6
	* @param {bool *} _errorNB To signal error in case of non-blocking connect.
	*/
	void connect(const char[], const int = 0, bool * = nullptr);

	/** @method connect
	* @access public
	* Connects net::Socket to ipv4 peer if successful else throws runtime_error
	* exception.
	* Throws invalid_argument exception in case of non-blocking net::Socket if
	* _errorNB is missing.
	* Throws invalid_argument exception if destination address given is invalid.
	*
	* @param {callable} _fn Some callable that takes arg of type addrIpv4 or
	* void.
	* @param {bool *} _errorNB To signal error in case of non-blocking connect.
	*/
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

	/** @method connect
	* @access public
	* Connects net::Socket to ipv6 peer if successful else throws runtime_error
	* exception.
	* Throws invalid_argument exception in case of non-blocking net::Socket if
	* _errorNB is missing.
	* Throws invalid_argument exception if destination address given is invalid.
	* Invokes the callable provided to fill addrIpv4 object.
	*
	* @param {callable} _fn Some callable that takes arg of type addrIpv6 or
	* void.
	* @param {bool *} _errorNB To signal error in case of non-blocking connect.
	*/
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

	/** @method connect
	* @access public
	* Connects net::Socket to unix socket peer if successful else throws
	* runtime_error exception.
	* Throws invalid_argument exception in case of non-blocking net::Socket if
	* _errorNB is missing.
	* Throws invalid_argument exception if destination address given is invalid.
	* Invokes the callable provided to fill addrIpv4 object.
	*
	* @param {callable} _fn Some callable that takes arg of type addrUnix or
	* void.
	* @param {bool *} _errorNB To signal error in case of non-blocking connect.
	*/
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

	/** @method start
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

	/** @method accept
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


	/** @method write
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

	/** @method read
	* @access public
	* Reads given number of bytes using Socket if successful else throws
	* runtime_error exception.
	* Throws invalid_argument exception in case of non-blocking net::Socket if
	* _errorNB is missing.
	*
	* @param {int} _bufSize Number of bytes to be read using Socket.
	* @param {bool *} _errorNB To signal error in case of non-blocking read.
	* @Returns {string} String of _bufSize bytes read using Socket.
	*/
	std::string read(const int = 1024, bool * = nullptr) const;


	// Send method calls

	/** @method send
	* @access public
	* Sends given string using Socket if successful else throws runtime_error
	* exception.
	* Throws invalid_argument exception in case of non-blocking net::Socket if
	* _errorNB is missing.
	*
	* @param {string} _msg String to be sent using Socket.
	* @param {SF::send} _flags Modify default behaviour of send.
	* @param {bool *} _errorNB To signal error in case of non-blocking send.
	*/
	void send(const std::string &, SF::send = SF::send::NONE,
	          bool * = nullptr) const;


	/** @method send
	* @access public
	* Sends given string using Socket if successful else throws runtime_error
	* exception.
	* Throws invalid_argument exception in case of non-blocking net::Socket if
	* _errorNB is missing.
	* Throws invalid_argument exception if destination address given is invalid.
	* Invokes the callable provided to fill addrIpv4 object.
	*
	* @param {string} _msg String to be sent using Socket.
	* @param {callable} _fn Some callable that takes arg of type addrIpv4 or
	* void.
	* @param {SF::send} _flags Modify default behaviour of send.
	* @param {bool *} _errorNB To signal error in case of non-blocking send.
	*/
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


	/** @method send
   * @access public
   * Sends given string using Socket if successful else throws runtime_error
   * exception.
   * Throws invalid_argument exception in case of non-blocking net::Socket if
   * _errorNB is missing.
   * Throws invalid_argument exception if destination address given is invalid.
   * Invokes the callable provided to fill addrIpv6 object.
   *
   * @param {string} _msg String to be sent using Socket.
   * @param {callable} _fn Some callable that takes arg of type addrIpv6 or
   * void.
   * @param {SF::send} _flags Modify default behaviour of send.
   * @param {bool *} _errorNB To signal error in case of non-blocking send.
   */
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


	/** @method send
   * @access public
   * Sends given string using Socket if successful else throws runtime_error
   * exception.
   * Throws invalid_argument exception in case of non-blocking net::Socket if
   * _errorNB is missing.
   * Throws invalid_argument exception if destination address given is invalid.
   * Invokes the callable provided to fill addrUnix object.
   *
   * @param {string} _msg String to be sent using Socket.
   * @param {callable} _fn Some callable that takes arg of type addrUnix or
   * void.
   * @param {SF::send} _flags Modify default behaviour of send.
   * @param {bool *} _errorNB To signal error in case of non-blocking send.
   */
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

	/** @method recv
   * @access public
   * Reads given number of bytes using Socket if successful else throws
   * runtime_error exception.
   * Throws invalid_argument exception in case of non-blocking net::Socket if
   * _errorNB is missing.
   *
   * @param {int} _bufSize Number of bytes to be read using Socket.
   * @param {SF::recv} _flags Modify default behaviour of recv.
   * @param {bool *} _errorNB To signal error in case of non-blocking recv.
   * @returns {string} String of _bufSize bytes read from socket.
   */
	std::string recv(const int = 1024, SF::recv = SF::recv::NONE,
	                 bool * = nullptr) const;

	/** @method recv
   * @access public
   * Reads given number of bytes using Socket if successful else throws
   * runtime_error exception.
   * Throws invalid_argument exception in case of non-blocking net::Socket if
   * _errorNB is missing.
   * Throws invalid_argument exception if destination address given is invalid.
   * Invokes the callable provided to fill addrIpv4 object.
   *
   * @param {string} _msg String to be sent using Socket.
   * @param {callable} _fn Some callable that takes arg of type addrIpv4 or
   * void.
   * @param {SF::recv} _flags Modify default behaviour of recv.
   * @param {bool *} _errorNB To signal error in case of non-blocking recv.
   */
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


	/** @method recv
	* @access public
	* Reads given number of bytes using Socket if successful else throws
	* runtime_error exception.
	* Throws invalid_argument exception in case of non-blocking net::Socket if
	* _errorNB is missing.
	* Throws invalid_argument exception if destination address given is invalid.
	* Invokes the callable provided to fill addrIpv6 object.
	*
	* @param {string} _msg String to be sent using Socket.
	* @param {callable} _fn Some callable that takes arg of type addrIpv6 or
	* void.
	* @param {SF::recv} _flags Modify default behaviour of recv.
	* @param {bool *} _errorNB To signal error in case of non-blocking recv.
	*/
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


	/** @method recv
	* @access public
	* Reads given number of bytes using Socket if successful else throws
	* runtime_error exception.
	* Throws invalid_argument exception in case of non-blocking net::Socket if
	* _errorNB is missing.
	* Throws invalid_argument exception if destination address given is invalid.
	* Invokes the callable provided to fill addrUnix object.
	*
	* @param {string} _msg String to be sent using Socket.
	* @param {callable} _fn Some callable that takes arg of type addrUnix or
	* void.
	* @param {SF::recv} _flags Modify default behaviour of recv.
	* @param {bool *} _errorNB To signal error in case of non-blocking recv.
	*/
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


	/** @method setOpt
	* @access public
	* Set a socket option from net::SF::opt for Socket using object of type
	* net::SF::sockOpt.
	*
	* @param {net::SF::opt} _opType Option to set for Socket.
	* @param {net::SF::sockOpt} _opValue socket option structure. Present inside
	* socket_family.hpp.
	*/
	void setOpt(SF::opt, SF::sockOpt) const;

	/** @method getOpt
	* @access public
	* Get value of some socket option for Socket.
	*
	* @param {net::SF::opt} _opType Option of Socket whose value to get.
	*/
	SF::sockOpt getOpt(SF::opt) const;

	/** @method stop
	* @access public
	* Shutdown Socket using net::SF::shut.
	*
	* @param {net::SF::shut} _s Option specifying which side of connection to
	* shutdown for Socket.
	*/
	void stop(SF::shut _s) const noexcept
	{
		shutdown(sockfd, static_cast<int>(_s));
	}

	/** method close
	* @access public
	* Closes the Socket for terminating connection.
	*/
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
