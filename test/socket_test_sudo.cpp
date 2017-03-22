#include "socket.hpp"
#include <gtest/gtest.h>

using namespace net;


TEST(Socket, ConstructorRaw)
{
	EXPECT_NO_THROW(Socket s(Domain::IPv4, Type::RAW, 4));
	EXPECT_NO_THROW(Socket s(Domain::IPv6, Type::RAW, 4));
}


TEST(Socket, GetSocket)
{
	Socket s3(Domain::IPv4, Type::RAW, 4);
	Socket s6(Domain::IPv6, Type::RAW, 4);
	ASSERT_GT(s3.getSocket(), 0);
	ASSERT_GT(s6.getSocket(), 0);
}


TEST(SocketOptions, Debug)
{
	Socket s(Domain::IPv4, Type::UDP);

	int optval;
	socklen_t optlen = sizeof(optval);

	SockOpt opt(1);
	ASSERT_EQ(1, opt.getValue());

	s.setOpt(Opt::DEBUG, opt);
	ASSERT_EQ(
	  0, getsockopt(s.getSocket(), SOL_SOCKET, SO_DEBUG, &optval, &optlen));
	ASSERT_EQ(1, optval);

	optval = 0;
	ASSERT_EQ(0,
	          setsockopt(s.getSocket(), SOL_SOCKET, SO_DEBUG, &optval, optlen));

	auto s2 = s.getOpt(Opt::DEBUG);
	ASSERT_EQ(0, s2.getValue());
}
