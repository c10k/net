#include "socket.hpp"
#include <gtest/gtest.h>

using namespace net;

TEST(socket, bind)
{


	//****************** IPv4 ******************
	{
		Socket mySocket(SF::domain::IPv4, SF::type::TCP);
		EXPECT_NO_THROW(mySocket.bind(
		  [](addrIPv4 &s) { return methods::construct(s, "127.0.0.1", 0); }));
	}

	{
		Socket s(SF::domain::IPv4, SF::type::TCP);
		EXPECT_NO_THROW(s.bind([](addrIPv4 &s) {
			return methods::construct(s, "127.0.0.1", 13000);
		}));
	}

	{
		Socket s(SF::domain::IPv4, SF::type::TCP);
		EXPECT_THROW(s.bind([](addrIPv4 &s) {
			return methods::construct(s, "256.0.0.0", 8000);
		}),
		             std::invalid_argument);
	}

	{
		Socket s(SF::domain::IPv4, SF::type::TCP);
		EXPECT_THROW(s.bind([](addrIPv4 &s) {
			return methods::construct(s, "255.0444.0.0", 8001);
		}),
		             std::invalid_argument);
	}

	{
		Socket s(SF::domain::IPv4, SF::type::TCP);
		EXPECT_THROW(s.bind([](addrIPv4 &s) { return 0; }),
		             std::invalid_argument);
	}

	{
		Socket s(SF::domain::IPv4, SF::type::TCP);
		EXPECT_THROW(s.bind([](addrIPv4 &s) { return -1; }),
		             std::runtime_error);
	}

	{
		// will not throw exception bcoz port number is getting converted
		// internally.
		Socket s(SF::domain::IPv4, SF::type::TCP);
		EXPECT_NO_THROW(s.bind([](addrIPv4 &s) {
			return methods::construct(s, "0.0.0.0", 130000000);
		}));
	}

	{
		// will not throw exception bcoz port number is getting converted
		// internally.
		Socket s(SF::domain::IPv4, SF::type::TCP);
		EXPECT_NO_THROW(s.bind([](addrIPv4 &s) {
			return methods::construct(s, "0.0.0.0", -1300000);
		}));
	}

	{
		Socket s(SF::domain::IPv4, SF::type::UDP);
		EXPECT_NO_THROW(s.bind([](addrIPv4 &s) {
			return methods::construct(s, "127.0.0.0", 8000);
		}));
	}

	{

		Socket s(SF::domain::IPv4, SF::type::TCP);
		EXPECT_NO_THROW(s.bind([](addrIPv4 &s) {

			s.sin_family      = AF_INET;
			s.sin_addr.s_addr = htonl(INADDR_ANY);
			s.sin_port        = htons(8000);
			return 1;
		}));
	}

	{
		// when given callable doesn't fill it's structure object argument.
		Socket mySocket(SF::domain::IPv4, SF::type::TCP);
		EXPECT_NO_THROW(mySocket.bind([](addrIPv4 &s) { return 5; }));
		EXPECT_ANY_THROW(mySocket.connect("127.0.0.1", 15000));
	}

	{
		// should throw bcoz ::bind will not be able to bind a ipv4 Socket to
		// IPv6 address.
		Socket mySocket(SF::domain::IPv4, SF::type::TCP);
		EXPECT_THROW(mySocket.bind([](addrIPv6 &s) {
			return methods::construct(s, "0:0:0:0:0:0:0:1", 14000);
		}),
		             std::runtime_error);
	}

	//****************** IPv6 ******************

	{
		// may throw as machine might not be having the specified IPv6 address.
		Socket s(SF::domain::IPv6, SF::type::TCP);
		EXPECT_THROW(s.bind([](addrIPv6 &s) {
			return methods::construct(
			  s, "2001:0db8:85a3:0000:0000:8a2e:0370:7334", 8000);
		}),
		             std::runtime_error);
	}

	{
		Socket s(SF::domain::IPv6, SF::type::TCP);
		EXPECT_THROW(s.bind([](addrIPv6 &s) { return 0; }),
		             std::invalid_argument);
	}

	{
		// Bug with the error string that will be displayed, bcoz some last
		// error's error number will be set in errno.
		Socket s(SF::domain::IPv6, SF::type::TCP);
		EXPECT_THROW(s.bind([](addrIPv6 &s) { return -1; }),
		             std::runtime_error);
	}
	{
		Socket s(SF::domain::IPv6, SF::type::TCP);
		EXPECT_NO_THROW(s.bind([](addrIPv6 &s) {
			return methods::construct(s, "0:0:0:0:0:0:0:1", 13000);
		}));
	}

	{
		// should throw bcoz socket is IPv6, ip address to fill is in IPv4
		// format so when construct(compatible with IPv4) is called, then
		// inet_pton fails.
		Socket s(SF::domain::IPv6, SF::type::TCP);
		EXPECT_THROW(s.bind([](addrIPv4 &s) {
			return methods::construct(s, "0.0.0.0", 130000000);
		}),
		             std::runtime_error);
	}

	{
		// should throw bcoz socket is IPv6, ip address to fill is also in IPv6
		// format but the construct which is called is IPv4 compatible.
		Socket mySocket(SF::domain::IPv6, SF::type::TCP);
		EXPECT_ANY_THROW(mySocket.bind([](addrIPv4 &s) {
			return methods::construct(s, "0:0:0:0:0:0:0:0", 0);
		}));
	}

	{
		// should throw as given IPv4 mapped IPv6 address to be filled in IPv4
		// socket address structure.
		Socket s(SF::domain::IPv6, SF::type::TCP);
		EXPECT_THROW(s.bind([](addrIPv4 &s) {
			return methods::construct(s, "::::::127.0.0.1", -1300000);
		}),
		             std::invalid_argument);
	}

	{
		// Don't know why this should throw !! Improve This !!
		Socket s(SF::domain::IPv6, SF::type::TCP);
		EXPECT_THROW(s.bind([](addrIPv6 &s) {
			return methods::construct(s, "0:0:0:0:0:0:127.0.0.1", 15000);
		}),
		             std::runtime_error);
	}

	{
		// Segmentation fault occuring. !! Improve This !!
		Socket s(SF::domain::IPv6, SF::type::TCP);
		EXPECT_NO_THROW(s.bind([](addrIPv6 &s) {

			s.sin6_family = AF_INET6;
			inet_pton(AF_INET6, "0:0:0:0:0:0:0:0", &s.sin6_addr.s6_addr);
			// s.sin6_addr.s6_addr = INADDR_ANY;
			s.sin6_port = htons(8000);
			return 1;
		}));
	}
}