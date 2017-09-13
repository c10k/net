#include "socket.hpp"
#include <gtest/gtest.h>
#include <thread>
#include <functional>


using namespace net;
using namespace std::chrono_literals;

namespace {

void runUnixServer(Socket &s, const char path[])
{
    s.start(path);
    if (s.getType() == Type::TCP) {
        auto peer = s.accept();
    }
}

void runNonUnixServer(Socket &s, const unsigned int port)
{
    std::string address;
    switch (s.getDomain()) {
        case Domain::IPv4: address = "127.0.0.1"; break;
        case Domain::IPv6: address = "::1"; break;

        default: address = " ";
    }

    try {
        s.start(address.c_str(), port);
        const auto peer = s.accept();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what();
    }
}
}

TEST(Socket, Connectv4)
{
    Socket server4(Domain::IPv4, Type::TCP);

    std::thread serverThread1(runNonUnixServer, std::ref(server4), 15010);
    serverThread1.detach();
    std::this_thread::sleep_for(1s);

    Socket client4(Domain::IPv4, Type::TCP);
    ASSERT_NO_THROW(client4.connect("127.0.0.1", 15010));
    client4.close();
}


TEST(Socket, Connectv6)
{
    Socket server6(Domain::IPv6, Type::TCP);
    std::thread serverThread1(runNonUnixServer, std::ref(server6), 15020);
    serverThread1.detach();
    std::this_thread::sleep_for(1s);

    Socket client6(Domain::IPv6, Type::TCP);
    ASSERT_NO_THROW(client6.connect("::1", 15020));
    client6.close();
}

TEST(Socket, ConnectUnixTCP)
{
    std::string unixPathServer("/tmp/unixSocketFileServer7");
    Socket serverUnixTCP(Domain::UNIX, Type::TCP);
    std::thread serverThread1(runUnixServer, std::ref(serverUnixTCP),
                              unixPathServer.c_str());
    serverThread1.detach();
    std::this_thread::sleep_for(1s);

    std::string unixPathClient("/tmp/unixSocketFileClient7");
    Socket clientUnixTCP(Domain::UNIX, Type::TCP);
    EXPECT_NO_THROW(clientUnixTCP.bind([&](AddrUnix &s) {
        return methods::construct(s, unixPathClient.c_str());
    }));
    EXPECT_NO_THROW(clientUnixTCP.connect([&](AddrUnix &s) {
        return methods::construct(s, unixPathServer.c_str());
    }););
    clientUnixTCP.close();
}

TEST(Socket, ConnectUnixUDP)
{
    std::string unixPathServer("/tmp/unixSocketFileServer2");
    Socket serverUnixUDP(Domain::UNIX, Type::UDP);
    std::thread serverThread1(
      [&]() { runUnixServer(serverUnixUDP, unixPathServer.c_str()); });
    serverThread1.detach();
    std::this_thread::sleep_for(1s);

    std::string unixPathClient("/tmp/unixSocketFileClient2");
    Socket clientUnixUDP(Domain::UNIX, Type::UDP);
    EXPECT_NO_THROW(clientUnixUDP.bind([&](AddrUnix &s) {
        return methods::construct(s, unixPathClient.c_str());
    }));
    EXPECT_NO_THROW(clientUnixUDP.connect([&](AddrUnix &s) {
        return methods::construct(s, unixPathServer.c_str());
    }));
}
