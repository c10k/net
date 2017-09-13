
## **low_write**

Writes given _msg using _sockfd by calling _fn with args havingflags and destination socket address.

```
	template <typename Fn, typename... Args>
	auto low_write(Fn &&_fn, const std::string &_msg, Args &&... args) const
	
```

### PARAMETERS:
| NAME | TYPE | DESCRIPTION |
|------ | ------ | -------------|
|_fn|callable|Some callable that writes using socket descriptor.|
|_msg|string|Msg to write on sockfd.|
|args|parameter_pack|Flags, destination sockaddr objects and theirlengths.|

### RETURN VALUE
|TYPE | DESCRIPTION |
|------|-------------|
|ssize_t|Status of writing _msg using socket descriptor / Numberof bytes written using socket descriptor.|



___
        
## **low_read**

Reads using sockfd by calling _fn  with args having flags anddestination socket address.

```
	template <typename Fn, typename... Args>
	auto low_read(Fn &&_fn, std::string &_str, Args &&... args) const
	
```

### PARAMETERS:
| NAME | TYPE | DESCRIPTION |
|------ | ------ | -------------|
|_fn|callable|Some callable that reads using socket descriptor.|
|_str|string|String to store the data.|
|args|parameter_pack|Flags, destination sockaddr objects and theirlengths.|

### RETURN VALUE
|TYPE | DESCRIPTION |
|------|-------------|
|ssize_t|Status of reading data using socket descriptor / Numberof bytes read using socket descriptor.|



___
        
## **Socket**

None

```
	Socket(const int, Domain, Type, const void *)
```

### PARAMETERS:
| NAME | TYPE | DESCRIPTION |
|------ | ------ | -------------|
|_sockfd|int|Descriptor representing a socket.|
|_domain|Domain|Socket domain.|
|_domain|Type|Socket type.|
|_addr|void *|Pointer to initialize appropriate member of Union ofnet::Socket.|

### RETURN VALUE
[]


___
        
## **net::Socket**

None

```
	Socket(Domain _domain, Type _type, const int _proto = 0)
	    : sock_domain(_domain), sock_type(_type)
	
```

### PARAMETERS:
| NAME | TYPE | DESCRIPTION |
|------ | ------ | -------------|
|_domain|domain|Socket domain.|
|_type|type|Socket type.|
|_proto|int|Socket protocol.|

### RETURN VALUE
[]


___
        
## **net::Socket**

None

```
	Socket(Socket &&s)
	
```

### PARAMETERS:
| NAME | TYPE | DESCRIPTION |
|------ | ------ | -------------|
|s|Socket|Rvalue of type socket.|

### RETURN VALUE
[]


___
        
## **getSocket**

Get the socket descriptor in net::Socket.

```
	auto getSocket() const noexcept 
```

### PARAMETERS:
[]
### RETURN VALUE
|TYPE | DESCRIPTION |
|------|-------------|
|int|Socket descriptor for net::Socket.|



___
        
## **getDomain**

Get the Domain type of Socket.

```
	auto getDomain() const noexcept 
```

### PARAMETERS:
[]
### RETURN VALUE
|TYPE | DESCRIPTION |
|------|-------------|
|Domain|for net::Socket.|



___
        
## **getType**

Get the Protocol Type of Socket.

```
	auto getType() const noexcept 
```

### PARAMETERS:
[]
### RETURN VALUE
|TYPE | DESCRIPTION |
|------|-------------|
|Type|for net::Socket.|



___
        
## **bind**

Binds net::Socket to local address if successful else if Addressargument is invalid then throws invalid_argument exception else throwsruntime_error exception signalling that bind failed. Invokes the callableprovided to fill AddrIPv4 object.

```
	template <typename F>
	auto bind(F _fn) -> decltype(_fn(std::declval<AddrIPv4 &>()), void()) const
	
```

### PARAMETERS:
| NAME | TYPE | DESCRIPTION |
|------ | ------ | -------------|
|_fn|callable|Some callable that takes arg of type AddrIPv4.|

### RETURN VALUE
[]


___
        
## **bind**

Binds net::Socket to local address if successful else if Addressargumentis invalid then throws invalid_argument exception else throwsruntime_error exception signalling that bind failed. Invokes the callableprovided to fill AddrIPv6 object.

```
	template <typename F>
	auto bind(F _fn) -> decltype(_fn(std::declval<AddrIPv6 &>()), void()) const
	
```

### PARAMETERS:
| NAME | TYPE | DESCRIPTION |
|------ | ------ | -------------|
|_fn|callable|Some callable that takes arg of type AddrIPv6.|

### RETURN VALUE
[]


___
        
## **bind**

Binds net::Socket to local address if successful else if Addressargument is invalid then throws invalid_argument exception else throwsruntime_error exception signalling that bind failed. Invokes the callableprovided to fill AddrUnix object.

```
	template <typename F>
	auto bind(F _fn) -> decltype(_fn(std::declval<AddrUnix &>()), void()) const
	
```

### PARAMETERS:
| NAME | TYPE | DESCRIPTION |
|------ | ------ | -------------|
|_fn|callable|Some callable that takes arg of type AddrUnix.|

### RETURN VALUE
[]


___
        
## **connect**

Connects net::Socket to address _addr:_port if successful elsethrows invalid_argument exception.

```
	void connect(const char[], const int = 0, bool * = nullptr)
```

### PARAMETERS:
| NAME | TYPE | DESCRIPTION |
|------ | ------ | -------------|
|_addr|char []|Ip address in case of ipv4 or ipv6 domain, and Pathin case of unix domain.|
|_port|int|Port number in case of AddrIPv4 or AddrIPv6.|
|_errorNB|bool *|To signal error in case of non-blocking connect.|

### RETURN VALUE
[]


___
        
## **connect**

Connects net::Socket to ipv4 peer if successful else throwsruntime_error exception.Throws invalid_argument exception in case of non-blocking net::Socket if_errorNB is missing.Throws invalid_argument exception if destination address given is invalid.

```
	template <typename F>
	auto connect(F _fn, bool *_errorNB = nullptr)
	  -> decltype(_fn(std::declval<AddrIPv4 &>()), void()) const
	
```

### PARAMETERS:
| NAME | TYPE | DESCRIPTION |
|------ | ------ | -------------|
|_fn|callable|Some callable that takes arg of type AddrIPv4.|
|_errorNB|bool *|To signal error in case of non-blocking connect.|

### RETURN VALUE
[]


___
        
## **connect**

Connects net::Socket to ipv6 peer if successful else throwsruntime_error exception.Throws invalid_argument exception in case of non-blocking net::Socket if_errorNB is missing.Throws invalid_argument exception if destination address given is invalid.Invokes the callable provided to fill AddrIPv4 object.

```
	template <typename F>
	auto connect(F _fn, bool *_errorNB = nullptr)
	  -> decltype(_fn(std::declval<AddrIPv6 &>()), void()) const
	
```

### PARAMETERS:
| NAME | TYPE | DESCRIPTION |
|------ | ------ | -------------|
|_fn|callable|Some callable that takes arg of type AddrIPv6.|
|_errorNB|bool *|To signal error in case of non-blocking connect.|

### RETURN VALUE
[]


___
        
## **connect**

Connects net::Socket to unix socket peer if successful else throwsruntime_error exception.Throws invalid_argument exception in case of non-blocking net::Socket if_errorNB is missing.Throws invalid_argument exception if destination address given is invalid.Invokes the callable provided to fill AddrIPv4 object.

```
	template <typename F>
	auto connect(F _fn, bool *_errorNB = nullptr)
	  -> decltype(_fn(std::declval<AddrUnix &>()), void()) const
	
```

### PARAMETERS:
| NAME | TYPE | DESCRIPTION |
|------ | ------ | -------------|
|_fn|callable|Some callable that takes arg of type AddrUnix.|
|_errorNB|bool *|To signal error in case of non-blocking connect.|

### RETURN VALUE
[]


___
        
## **start**

Starts the net::Socket in listen mode on given ip address and givenport with given backlog if successful else throws runtime_error exception.Throws invalid_argument exception if given ip address or port are notvalid.

```
	void start(const char[], const int = 0, const int = SOMAXCONN)
```

### PARAMETERS:
| NAME | TYPE | DESCRIPTION |
|------ | ------ | -------------|
|_addr|char []|Ip address in case of ipv4 or ipv6 domain, and Pathin case of unix domain.|
|_port|int|Port number in case of ipv4 or ipv6.|
|_q|int|Size of backlog of listening socket.|

### RETURN VALUE
[]


___
        
## **accept**

Returns Socket object from connected sockets queue if successfulelse throws runtime_error exception.Throws invalid_argument exception in case of non-blocking net::Socket if_errorNB is missing.

```
	Socket accept(bool * = nullptr) const
```

### PARAMETERS:
| NAME | TYPE | DESCRIPTION |
|------ | ------ | -------------|
|_errorNB|bool *|To signal error in case of non-blocking accept.|

### RETURN VALUE
|TYPE | DESCRIPTION |
|------|-------------|
|net::Socket||



___
        
## **write**

Writes given string to Socket if successful else throwsruntime_error exception.Throws invalid_argument exception in case of non-blocking net::Socket if_errorNB is missing.

```
	void write(const std::string &, bool * = nullptr) const
```

### PARAMETERS:
| NAME | TYPE | DESCRIPTION |
|------ | ------ | -------------|
|_msg|string|String to be written to Socket.|
|_errorNB|bool *|To signal error in case of non-blocking write.|

### RETURN VALUE
[]


___
        
## **read**

Reads given number of bytes using Socket if successful else throwsruntime_error exception.Throws invalid_argument exception in case of non-blocking net::Socket if_errorNB is missing.

```
	std::string read(const int, bool * = nullptr) const
```

### PARAMETERS:
| NAME | TYPE | DESCRIPTION |
|------ | ------ | -------------|
|_bufSize|int|Number of bytes to be read using Socket.|
|_errorNB|bool *|To signal error in case of non-blocking read.|

### RETURN VALUE
|TYPE | DESCRIPTION |
|------|-------------|
|string|String of _bufSize bytes read using Socket.|



___
        
## **send**

Sends given string using Socket if successful else throwsruntime_error exception.Throws invalid_argument exception in case of non-blocking net::Socket if_errorNB is missing.

```
	void send(const std::string &, Send = Send::NONE, bool * = nullptr) const
```

### PARAMETERS:
| NAME | TYPE | DESCRIPTION |
|------ | ------ | -------------|
|_msg|string|String to be sent using Socket.|
|_flags|send|Modify default behaviour of send.|
|_errorNB|bool *|To signal error in case of non-blocking send.|

### RETURN VALUE
[]


___
        
## **send**

Sends given string using Socket if successful else throwsruntime_error exception.Throws invalid_argument exception in case of non-blocking net::Socket if_errorNB is missing.Throws invalid_argument exception if destination address given is invalid.Invokes the callable provided to fill AddrIPv4 object.

```
	template <typename F>
	auto send(const std::string &_msg, F _fn, Send _flags = Send::NONE,
	          bool *_errorNB = nullptr) const
	  -> decltype(_fn(std::declval<AddrIPv4 &>()), void()) const
	
```

### PARAMETERS:
| NAME | TYPE | DESCRIPTION |
|------ | ------ | -------------|
|_msg|string|String to be sent using Socket.|
|_fn|callable|Some callable that takes arg of type AddrIPv4 orvoid.|
|_flags|send|Modify default behaviour of send.|
|_errorNB|bool *|To signal error in case of non-blocking send.|

### RETURN VALUE
[]


___
        
## **send**

Sends given string using Socket if successful else throwsruntime_error exception.Throws invalid_argument exception in case of non-blocking net::Socket if_errorNB is missing.Throws invalid_argument exception if destination address given is invalid.Invokes the callable provided to fill AddrIPv6 object.

```
	template <typename F>
	auto send(const std::string &_msg, F _fn, Send _flags = Send::NONE,
	          bool *_errorNB = nullptr) const
	  -> decltype(_fn(std::declval<AddrIPv6 &>()), void()) const
	
```

### PARAMETERS:
| NAME | TYPE | DESCRIPTION |
|------ | ------ | -------------|
|_msg|string|String to be sent using Socket.|
|_fn|callable|Some callable that takes arg of type AddrIPv6 orvoid.|
|_flags|send|Modify default behaviour of send.|
|_errorNB|bool *|To signal error in case of non-blocking send.|

### RETURN VALUE
[]


___
        
## **send**

Sends given string using Socket if successful else throwsruntime_error exception.Throws invalid_argument exception in case of non-blocking net::Socket if_errorNB is missing.Throws invalid_argument exception if destination address given is invalid.Invokes the callable provided to fill AddrUnix object.

```
	template <typename F>
	auto send(const std::string &_msg, F _fn, Send _flags = Send::NONE,
	          bool *_errorNB = nullptr) const
	  -> decltype(_fn(std::declval<AddrUnix &>()), void()) const
	
```

### PARAMETERS:
| NAME | TYPE | DESCRIPTION |
|------ | ------ | -------------|
|_msg|string|String to be sent using Socket.|
|_fn|callable|Some callable that takes arg of type AddrUnix orvoid.|
|_flags|send|Modify default behaviour of send.|
|_errorNB|bool *|To signal error in case of non-blocking send.|

### RETURN VALUE
[]


___
        
## **recv**

Reads given number of bytes using Socket if successful else throwsruntime_error exception.Throws invalid_argument exception in case of non-blocking net::Socket if_errorNB is missing.

```
	std::string recv(const int, Recv = Recv::NONE, bool * = nullptr) const
```

### PARAMETERS:
| NAME | TYPE | DESCRIPTION |
|------ | ------ | -------------|
|_bufSize|int|Number of bytes to be read using Socket.|
|_flags|recv|Modify default behaviour of recv.|
|_errorNB|bool *|To signal error in case of non-blocking recv.|

### RETURN VALUE
|TYPE | DESCRIPTION |
|------|-------------|
|string|String of _bufSize bytes read from socket.|



___
        
## **recv**

Reads given number of bytes using Socket if successful else throwsruntime_error exception.Throws invalid_argument exception in case of non-blocking net::Socket if_errorNB is missing.Throws invalid_argument exception if destination address given is invalid.Invokes the callable provided to return AddrIPv4 object from where msg hasbeen received.

```
	template <typename F>
	auto recv(const int _numBytes, F _fn, Recv _flags = Recv::NONE,
	          bool *_errorNB = nullptr) const
	  -> decltype(_fn(std::declval<AddrIPv4 &>()), std::string()) const
	
```

### PARAMETERS:
| NAME | TYPE | DESCRIPTION |
|------ | ------ | -------------|
|_numBytes|int|Number of bytes to read.|
|_fn|callable|Some callable that takes arg of type AddrIPv4 orvoid.|
|_flags|recv|Modify default behaviour of recv.|
|_errorNB|bool *|To signal error in case of non-blocking recv.|

### RETURN VALUE
[]


___
        
## **recv**

Reads given number of bytes using Socket if successful else throwsruntime_error exception.Throws invalid_argument exception in case of non-blocking net::Socket if_errorNB is missing.Throws invalid_argument exception if destination address given is invalid.Invokes the callable provided to return AddrIPv6 object from where msg hasbeen received.

```
	template <typename F>
	auto recv(const int _numBytes, F _fn, Recv _flags = Recv::NONE,
	          bool *_errorNB = nullptr) const
	  -> decltype(_fn(std::declval<AddrIPv6 &>()), std::string()) const
	
```

### PARAMETERS:
| NAME | TYPE | DESCRIPTION |
|------ | ------ | -------------|
|_numBytes|int|Number of bytes to read.|
|_fn|callable|Some callable that takes arg of type AddrIPv6 orvoid.|
|_flags|recv|Modify default behaviour of recv.|
|_errorNB|bool *|To signal error in case of non-blocking recv.|

### RETURN VALUE
[]


___
        
## **recv**

Reads given number of bytes using Socket if successful else throwsruntime_error exception.Throws invalid_argument exception in case of non-blocking net::Socket if_errorNB is missing.Throws invalid_argument exception if destination address given is invalid.Invokes the callable provided to return AddrUnix object from where msg hasbeen received.

```
	template <typename F>
	auto recv(const int _numBytes, F _fn, Recv _flags = Recv::NONE,
	          bool *_errorNB = nullptr) const
	  -> decltype(_fn(std::declval<AddrUnix &>()), std::string()) const
	
```

### PARAMETERS:
| NAME | TYPE | DESCRIPTION |
|------ | ------ | -------------|
|_numBytes|int|Number of bytes to read.|
|_fn|callable|Some callable that takes arg of type AddrUnix orvoid.|
|_flags|recv|Modify default behaviour of recv.|
|_errorNB|bool *|To signal error in case of non-blocking recv.|

### RETURN VALUE
[]


___
        
## **setOpt**

Set a socket option from net::Opt for Socket using object of typenet::SockOpt.

```
	void setOpt(Opt, SockOpt) const
```

### PARAMETERS:
| NAME | TYPE | DESCRIPTION |
|------ | ------ | -------------|
|_opType|net::Opt|Option to set for Socket.|
|_opValue|net::SockOpt|socket option structure. Present insidesocket_family.hpp.|

### RETURN VALUE
[]


___
        
## **getOpt**

Get value of some socket option for Socket.

```
	SockOpt getOpt(Opt) const
```

### PARAMETERS:
| NAME | TYPE | DESCRIPTION |
|------ | ------ | -------------|
|_opType|net::Opt|Option of Socket whose value to get.|

### RETURN VALUE
[]


___
        
## **stop**

Shutdown Socket using net::shut.

```
	void stop(Shut _s) const noexcept
	
```

### PARAMETERS:
| NAME | TYPE | DESCRIPTION |
|------ | ------ | -------------|
|_s|net::shut|Option specifying which side of connection toshutdown for Socket.|

### RETURN VALUE
[]


___
        
## **unlink**

Unlinks the unix socket path.

```
	bool unlink() const noexcept
	
```

### PARAMETERS:
[]
### RETURN VALUE
[]


___
        
## **close**

Closes the Socket for terminating connection.

```
	bool close() const noexcept
	
```

### PARAMETERS:
[]
### RETURN VALUE
[]


___
        