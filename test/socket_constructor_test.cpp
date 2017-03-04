#include "socket.hpp"
#include <gtest/gtest.h>

using namespace net;

TEST(socket, constructor)
{
	{
		EXPECT_NO_THROW(Socket mySocket(SF::domain::IPv4, SF::type::TCP););
	}

	{
		// should throw as protocol given is IPv6 but socket is IPv4
		EXPECT_ANY_THROW(Socket mySocket(SF::domain::IPv4, SF::type::TCP, 41););
	}

	{
		// should throw as protocol given is tcp but socket is udp
		EXPECT_ANY_THROW(Socket mySocket(SF::domain::IPv4, SF::type::UDP, 6););
	}

	{
		// should not throw as protocol given is tcp
		EXPECT_NO_THROW(Socket mySocket(SF::domain::IPv4, SF::type::TCP, 6););
	}

	{
		EXPECT_NO_THROW(Socket mySocket(SF::domain::IPv6, SF::type::TCP););
	}

	{
		EXPECT_NO_THROW(Socket mySocket(SF::domain::IPv6, SF::type::UDP););
	}

	{
		EXPECT_NO_THROW(Socket mySocket(SF::domain::UNIX, SF::type::TCP););
	}

	{
		EXPECT_NO_THROW(Socket mySocket(SF::domain::UNIX, SF::type::UDP););
	}

	{
		// Should throw on any protocol which is not either 0 or 1.
		EXPECT_ANY_THROW(Socket mySocket(SF::domain::UNIX, SF::type::TCP, 3););
	}

	{
		Socket s1(SF::domain::IPv4, SF::type::TCP);
		Socket s2(SF::domain::IPv6, SF::type::TCP);
		Socket s3(SF::domain::IPv4, SF::type::UDP);
		Socket s4(SF::domain::IPv6, SF::type::UDP);
		ASSERT_GT(s1.getSocket(), 0);
		ASSERT_GT(s2.getSocket(), 0);
		ASSERT_GT(s3.getSocket(), 0);
		ASSERT_GT(s4.getSocket(), 0);
	}

	{
		EXPECT_NO_THROW(
		  Socket mySocket(SF::domain::IPv4, SF::type::SEQPACKET););
	}
}

// TEST(socket, constructor_within_loop)
// {
// 	{
// 		for (int i = 1; i <= 100000000; i++) {
// 			EXPECT_NO_THROW(
// 			  Socket mySocket(SF::domain::IPv4, SF::type::TCP, 6););
// 		}
// 	}
// }

TEST(socket_constructor, raw)
{
	// should not throw when run with cap_raw capability or run with admin
	// priviliges

	EXPECT_ANY_THROW(Socket mySocket(SF::domain::IPv4, SF::type::RAW, 4););

	EXPECT_ANY_THROW(Socket mySocket(SF::domain::IPv6, SF::type::RAW, 4););
}
