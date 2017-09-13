# NET [![codecov](https://codecov.io/gh/c10k/net/branch/master/graph/badge.svg)](https://codecov.io/gh/c10k/net)

![Logo](https://cloud.githubusercontent.com/assets/7630575/23608808/1e1de904-0291-11e7-8c6a-003f7a29e59c.png)

#### Net is a High-level & Efficient Modern C++ library for Network Programming.

[![wercker status](https://app.wercker.com/status/945fa8542bfa8066980fc234e4e91aee/m/ "wercker status")](https://app.wercker.com/project/byKey/945fa8542bfa8066980fc234e4e91aee)

# Introduction

It uses standard library functions with no other dependencies and provides a safer yet expressive version of original Berkeley Sockets API. Common mistakes are abstracted away giving a boost in productivity for library user.

```cpp
#include "socket.hpp"
#include <iostream>

using namespace net;

int main()
{
    try {
        Socket s(Domain::IPv4, Type::TCP);
        s.start("127.0.0.1", 24000);
        while (true) {
            const auto peer = s.accept();
            const auto msg  = peer.recv(15);
            std::cout << msg << '\n';
        }
    } catch (std::exception &e) {
        std::cerr << e.what() << '\n';
    }
}
```


# Getting Started

You don't need anything else other than a supported platform, a C++14 compiler and a few seconds to spare. The library is currently uses meson build system so first class dependency management is included for those using same. Otherwise just `git clone` the library, and pass the includes and sources to your compiler and you're good to go.

## Documentation

Please take a look inside `docs/` directory to find API and other documentation.

## Build with Meson

```bash
git clone https://github.com/c10k/net.git
cd net
mkdir build && cd build
meson .. && ninja  # This will also create a dynamic lib
```

## Testing with GTest

```bash
git clone https://github.com/c10k/net.git
cd net
mkdir build && cd build
meson .. && ninja
./test/testexe
```


# Examples

You can see more examples under `examples/` directory.

## Echo TCP server

```cpp
#include "socket.hpp"
#include <iostream>

using namespace net;

int main()
{
    try {
        Socket s(Domain::IPv4, Type::TCP);
        s.start("127.0.0.1", 24000);
        while (true) {
            const auto peer = s.accept();
            const auto msg  = peer.recv(15);
            std::cout << msg << '\n';
        }
    } catch (std::exception &e) {
        std::cerr << e.what() << '\n';
    }
}
```

## Hello World TCP client

```cpp
#include "socket.hpp"
#include <iostream>

using namespace net;

int main()
{
	try {
		Socket s(Domain::IPv4, Type::TCP);
		s.connect("0.0.0.0", 24000);
		s.send("Hello World!");
	} catch (std::exception &e) {
		std::cerr << e.what() << '\n';
	}
}
```

## Concurrent TCP server with fork

```cpp
#include "socket.hpp"
#include <iostream>
#include <signal.h>

using namespace net;

int main()
{
	try {
		signal(SIGCHLD, SIG_IGN);
		Socket s(Domain::IPv4, Type::TCP);
		s.start("127.0.0.1", 24001);

		while (true) {
			const auto peer = s.accept();
			if (!fork()) {
				std::cout << peer.recv(10) << '\n';
				return 0;
			}
		}
	} catch (std::exception &e) {
		std::cerr << e.what() << '\n';
	}
}
```

# TODO
- [ ] Proper namespacing in socket options
- [ ] Add more unit tests, increase coverage
- [ ] Add examples for non-blocking socket operations
- [ ] Add support for more protocols other than TCP, UDP, UNIX
- [ ] Add benchmarks against popular alternatives
