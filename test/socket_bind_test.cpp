#include "socket.hpp"
#include <gtest/gtest.h>

using namespace net;

TEST(Socket, Bind)
{
	Socket ipv4Socket(Domain::IPv4, Type::TCP);
	ASSERT_NO_THROW(ipv4Socket.bind(
	  [](AddrIPv4 &s) { return methods::construct(s, "127.0.0.1", 0); }));

	Socket ipv4Socket2(Domain::IPv4, Type::TCP);
	ASSERT_ANY_THROW(ipv4Socket2.bind(
	  [](AddrIPv4 &s) { return methods::construct(s, "127.0.0.1", 130000); }));


	Socket sock_invalid_addr(Domain::IPv4, Type::TCP);
	ASSERT_THROW(sock_invalid_addr.bind([](AddrIPv4 &s) {
		return methods::construct(s, "256.0.0.0", 8000);
	}),
	             std::invalid_argument);

	Socket sock_invalid_addr2(Domain::IPv4, Type::TCP);
	ASSERT_THROW(sock_invalid_addr2.bind([](AddrIPv4 &s) {
		return methods::construct(s, "255.0444.0.0", 8001);
	}),
	             std::invalid_argument);

	Socket sock_bad_addr(Domain::IPv4, Type::TCP);
	ASSERT_THROW(sock_bad_addr.bind([](AddrIPv4 &s) { return 0; }),
	             std::invalid_argument);

	Socket sock_bad_addr2(Domain::IPv4, Type::TCP);
	ASSERT_THROW(sock_bad_addr2.bind([](AddrIPv4 &s) { return -1; }),
	             std::runtime_error);


	Socket sock_bad_port(Domain::IPv4, Type::TCP);
	ASSERT_ANY_THROW(sock_bad_port.bind(
	  [](AddrIPv4 &s) { return methods::construct(s, "0.0.0.0", 130000000); }));

	Socket sock_neg_port(Domain::IPv4, Type::TCP);
	ASSERT_ANY_THROW(sock_neg_port.bind(
	  [](AddrIPv4 &s) { return methods::construct(s, "0.0.0.0", -1300000); }));


	Socket sock_udp(Domain::IPv4, Type::UDP);
	ASSERT_NO_THROW(sock_udp.bind(
	  [](AddrIPv4 &s) { return methods::construct(s, "127.0.0.0", 8000); }));


	Socket sock_manual_fill(Domain::IPv4, Type::TCP);
	ASSERT_NO_THROW(sock_manual_fill.bind([](AddrIPv4 &s) {
		s.sin_family      = AF_INET;
		s.sin_addr.s_addr = htonl(INADDR_ANY);
		s.sin_port        = htons(8000);
		return 1;
	}));


	Socket sock_liar_ipv4(Domain::IPv4, Type::TCP);
	ASSERT_THROW(sock_liar_ipv4.bind([](AddrIPv6 &s) {
		return methods::construct(s, "0:0:0:0:0:0:0:1", 14000);
	}),
	             std::runtime_error);


	Socket sock_bad_addr6(Domain::IPv6, Type::TCP);
	ASSERT_THROW(sock_bad_addr6.bind([](AddrIPv6 &s) { return 0; }),
	             std::invalid_argument);


	Socket sock_bad_addr62(Domain::IPv6, Type::TCP);
	ASSERT_THROW(sock_bad_addr62.bind([](AddrIPv6 &s) { return -1; }),
	             std::runtime_error);

	Socket sock_correct6(Domain::IPv6, Type::TCP);
	ASSERT_NO_THROW(sock_correct6.bind(
	  [](AddrIPv6 &s) { return methods::construct(s, "::1", 13000); }));


	Socket badIpv6addr(Domain::IPv6, Type::TCP);
	ASSERT_THROW(badIpv6addr.bind([](AddrIPv4 &s) {
		return methods::construct(s, "0.0.0.0", 130000000);
	}),
	             std::invalid_argument);


	Socket badIpv6_4(Domain::IPv6, Type::TCP);
	ASSERT_ANY_THROW(badIpv6_4.bind(
	  [](AddrIPv4 &s) { return methods::construct(s, "0:0:0:0:0:0:0:0", 0); }));


	Socket badIpv6_42(Domain::IPv6, Type::TCP);
	ASSERT_THROW(badIpv6_42.bind([](AddrIPv4 &s) {
		return methods::construct(s, "::::::127.0.0.1", -1300000);
	}),
	             std::invalid_argument);


	Socket unixSocket(Domain::UNIX, Type::TCP);
	std::string unixSocketPath("/tmp/unixSocketFile");
	unixSocket.bind([&](AddrUnix &s) {
		return methods::construct(s, unixSocketPath.c_str());
	});
	AddrUnix actualUnixSocket;
	socklen_t actualUnixSocketSize = sizeof(actualUnixSocket);
	ASSERT_NE(
	  getsockname(unixSocket.getSocket(), (sockaddr *) &actualUnixSocket,
	              &actualUnixSocketSize),
	  -1);
	EXPECT_EQ(actualUnixSocket.sun_path, unixSocketPath);
}
