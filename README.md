# NET [![codecov](https://codecov.io/gh/c10k/net/branch/master/graph/badge.svg)](https://codecov.io/gh/c10k/net)

![Logo](https://cloud.githubusercontent.com/assets/7630575/23608808/1e1de904-0291-11e7-8c6a-003f7a29e59c.png)

#### Net is a High-level & Efficient Modern C++ library for Network Programming.

[![wercker status](https://app.wercker.com/status/945fa8542bfa8066980fc234e4e91aee/m/ "wercker status")](https://app.wercker.com/project/byKey/945fa8542bfa8066980fc234e4e91aee)

It uses standard library functions with no other dependencies and provides a safer yet expressive version of original Berkeley Sockets API. Common mistakes are abstracted away giving a boost in productivity for library user.

A simple iterative TCP/IPv4 server is given below -

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
