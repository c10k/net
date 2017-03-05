#include "socket.hpp"
#include <gtest/gtest.h>

using namespace net;

TEST(socketOptions, Broadcast)
{
	Socket s4(Domain::IPv4, Type::UDP);
	Socket s6(Domain::IPv4, Type::UDP);

	SockOpt option(1);
	int optval;
	socklen_t optlen = sizeof(optval);

	ASSERT_EQ(1, option.getValue());

	s4.setOpt(Opt::BROADCAST, option);
	s6.setOpt(Opt::BROADCAST, option);

	ASSERT_EQ(0, getsockopt(s4.getSocket(), SOL_SOCKET, SO_BROADCAST, &optval,
	                        &optlen));
	ASSERT_EQ(1, optval);

	ASSERT_EQ(0, getsockopt(s6.getSocket(), SOL_SOCKET, SO_BROADCAST, &optval,
	                        &optlen));
	ASSERT_EQ(1, optval);


	optval = 0;
	ASSERT_EQ(
	  0, setsockopt(s4.getSocket(), SOL_SOCKET, SO_BROADCAST, &optval, optlen));
	ASSERT_EQ(
	  0, setsockopt(s6.getSocket(), SOL_SOCKET, SO_BROADCAST, &optval, optlen));

	auto opt4 = s4.getOpt(Opt::BROADCAST);
	auto opt6 = s6.getOpt(Opt::BROADCAST);

	ASSERT_EQ(0, opt4.getValue());
	ASSERT_EQ(0, opt6.getValue());

	Socket tcpSocket(Domain::IPv4, Type::TCP);
	SockOpt badOpt(1);
	ASSERT_EQ(1, badOpt.getValue());

	ASSERT_ANY_THROW(tcpSocket.setOpt(Opt::BROADCAST, badOpt));
}


TEST(socketOptions, Linger)
{
	Socket s(Domain::IPv4, Type::TCP);

	linger lin;
	socklen_t len = sizeof(lin);

	SockOpt opt(true, 30);

	s.setOpt(Opt::LINGER, opt);
	ASSERT_EQ(0, getsockopt(s.getSocket(), SOL_SOCKET, SO_LINGER, &lin, &len));

	linger test;
	test.l_onoff  = 1;
	test.l_linger = 30;

	ASSERT_EQ(test.l_onoff, lin.l_onoff);
	ASSERT_EQ(test.l_linger, lin.l_linger);
}


TEST(socketOptions, Debug)
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


TEST(socketOptions, DontRoute)
{
	Socket s(Domain::IPv4, Type::UDP);

	int optval;
	socklen_t optlen = sizeof(optval);

	SockOpt opt(1);
	s.setOpt(Opt::DONTROUTE, opt);
	ASSERT_EQ(
	  0, getsockopt(s.getSocket(), SOL_SOCKET, SO_DONTROUTE, &optval, &optlen));
	ASSERT_EQ(1, optval);
	optval = 0;

	ASSERT_EQ(
	  0, setsockopt(s.getSocket(), SOL_SOCKET, SO_DONTROUTE, &optval, optlen));
	auto opt2 = s.getOpt(Opt::DONTROUTE);
	ASSERT_EQ(0, opt2.getValue());
}


TEST(socketOptions, KeepAlive)
{
	Socket s(Domain::IPv4, Type::UDP);
	SockOpt opt(1);

	ASSERT_EQ(1, opt.getValue());
	s.setOpt(Opt::KEEPALIVE, opt);

	int optval;
	socklen_t optlen = sizeof(optval);

	ASSERT_EQ(
	  0, getsockopt(s.getSocket(), SOL_SOCKET, SO_KEEPALIVE, &optval, &optlen));
	ASSERT_EQ(1, optval);
	optval = 0;

	ASSERT_EQ(
	  0, setsockopt(s.getSocket(), SOL_SOCKET, SO_KEEPALIVE, &optval, optlen));
	auto opt2 = s.getOpt(Opt::KEEPALIVE);
	ASSERT_EQ(0, opt2.getValue());
}


TEST(socketOptions, OOBInline)
{
	Socket s(Domain::IPv4, Type::UDP);
	SockOpt opt(1);
	ASSERT_EQ(1, opt.getValue());
	s.setOpt(Opt::OOBINLINE, opt);
	int optval;
	socklen_t optlen = sizeof(optval);
	ASSERT_EQ(
	  0, getsockopt(s.getSocket(), SOL_SOCKET, SO_OOBINLINE, &optval, &optlen));
	ASSERT_EQ(1, optval);
	optval = 0;

	ASSERT_EQ(
	  0, setsockopt(s.getSocket(), SOL_SOCKET, SO_OOBINLINE, &optval, optlen));

	auto opt2 = s.getOpt(Opt::OOBINLINE);
	ASSERT_EQ(0, opt2.getValue());
}


TEST(socketOptions, RCVLOWAT)
{
	Socket s(Domain::IPv4, Type::TCP);
	SockOpt opt(100);
	int optval;
	socklen_t optlen = sizeof(optval);
	ASSERT_EQ(100, opt.getValue());
	s.setOpt(Opt::RCVLOWAT, opt);
	ASSERT_EQ(
	  0, getsockopt(s.getSocket(), SOL_SOCKET, SO_RCVLOWAT, &optval, &optlen));
	ASSERT_EQ(100, optval);
	optval = 10;

	ASSERT_EQ(
	  0, setsockopt(s.getSocket(), SOL_SOCKET, SO_RCVLOWAT, &optval, optlen));

	auto opt2 = s.getOpt(Opt::RCVLOWAT);
	ASSERT_EQ(10, opt2.getValue());
}
