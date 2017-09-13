#include "socket.hpp"
#include <gtest/gtest.h>

using namespace net;

TEST(Socket, GetSocket)
{
    try {
        Socket s1(Domain::IPv4, Type::TCP);
        Socket s2(Domain::IPv4, Type::UDP);
        Socket s3(Domain::IPv4, Type::RAW, 4);

        Socket s4(Domain::IPv6, Type::TCP);
        Socket s5(Domain::IPv6, Type::UDP);
        Socket s6(Domain::IPv6, Type::RAW, 4);

        Socket s7(Domain::UNIX, Type::TCP);
        Socket s8(Domain::UNIX, Type::UDP);

        ASSERT_GT(s1.getSocket(), 0);
        ASSERT_GT(s2.getSocket(), 0);
        ASSERT_GT(s3.getSocket(), 0);
        ASSERT_GT(s4.getSocket(), 0);
        ASSERT_GT(s5.getSocket(), 0);
        ASSERT_GT(s6.getSocket(), 0);
        ASSERT_GT(s7.getSocket(), 0);
        ASSERT_GT(s8.getSocket(), 0);

    } catch (std::exception &e) {
        FAIL() << e.what();
    }
}
