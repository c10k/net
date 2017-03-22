#include "socket.hpp"
#include <gtest/gtest.h>

using namespace net;

TEST(Socket, GetSocket)
{
	Socket s1(Domain::IPv4, Type::TCP);
	Socket s2(Domain::IPv4, Type::UDP);

	Socket s3(Domain::IPv6, Type::TCP);
	Socket s4(Domain::IPv6, Type::UDP);

	Socket s5(Domain::UNIX, Type::TCP);
	Socket s6(Domain::UNIX, Type::UDP);

	ASSERT_GT(s1.getSocket(), 0);
	ASSERT_GT(s2.getSocket(), 0);
	ASSERT_GT(s3.getSocket(), 0);
	ASSERT_GT(s4.getSocket(), 0);
	ASSERT_GT(s5.getSocket(), 0);
	ASSERT_GT(s6.getSocket(), 0);
}
