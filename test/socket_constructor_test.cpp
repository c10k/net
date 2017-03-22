#include "socket.hpp"
#include <gtest/gtest.h>

using namespace net;


TEST(Socket, ConstructorIPv4)
{
	EXPECT_NO_THROW(Socket s(Domain::IPv4, Type::TCP));
	EXPECT_NO_THROW(Socket s(Domain::IPv4, Type::UDP));
	EXPECT_NO_THROW(Socket s(Domain::IPv4, Type::TCP, 6));
	EXPECT_NO_THROW(Socket s(Domain::IPv4, Type::SEQPACKET, 132));

	EXPECT_ANY_THROW(Socket s(Domain::IPv4, Type::TCP, 41));
	EXPECT_ANY_THROW(Socket s(Domain::IPv4, Type::UDP, 6));
}


TEST(Socket, ConstructorIPv6)
{
	EXPECT_NO_THROW(Socket s(Domain::IPv6, Type::TCP));
	EXPECT_NO_THROW(Socket s(Domain::IPv6, Type::UDP));
	EXPECT_NO_THROW(Socket s(Domain::IPv6, Type::TCP, 6));
	EXPECT_NO_THROW(Socket s(Domain::IPv6, Type::SEQPACKET, 132));

	EXPECT_ANY_THROW(Socket s(Domain::IPv6, Type::TCP, 41));
	EXPECT_ANY_THROW(Socket s(Domain::IPv6, Type::UDP, 6));
}


TEST(socket, ConstructorUnix)
{
	EXPECT_NO_THROW(Socket s(Domain::UNIX, Type::TCP));
	EXPECT_NO_THROW(Socket s(Domain::UNIX, Type::UDP));
	EXPECT_NO_THROW(Socket s(Domain::UNIX, Type::SEQPACKET));

	EXPECT_ANY_THROW(Socket s(Domain::UNIX, Type::TCP, 3));
}
