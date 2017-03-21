#include "socket.hpp"
#include <gtest/gtest.h>

using namespace net;

TEST(SocketOptions, getType)
{
	SockOpt s1(1), s2(true, 5), s3(5L, 500L);
	enum { TIME = 0, LINGER = 1, INT = 2 } type;

	ASSERT_EQ(s1.getType(), INT);
	ASSERT_EQ(s2.getType(), LINGER);
	ASSERT_EQ(s3.getType(), TIME);
}


TEST(SocketOptions, CastError)
{
	SockOpt s1(1), s2(true, 5), s3(5L, 500L);

	ASSERT_NO_THROW(s1.getValue());
	ASSERT_NO_THROW(s2.getLinger());
	ASSERT_NO_THROW(s3.getTime());

	ASSERT_THROW(s1.getTime(), std::bad_cast);
	ASSERT_THROW(s1.getLinger(), std::bad_cast);

	ASSERT_THROW(s2.getTime(), std::bad_cast);
	ASSERT_THROW(s2.getValue(), std::bad_cast);

	ASSERT_THROW(s3.getValue(), std::bad_cast);
	ASSERT_THROW(s3.getLinger(), std::bad_cast);
}


TEST(SocketOptions, Broadcast)
{
	Socket s4(Domain::IPv4, Type::UDP);
	Socket s6(Domain::IPv4, Type::UDP);

	SockOpt option(1);
	int optval;
	socklen_t optlen = sizeof(optval);

	ASSERT_EQ(1, option.getValue());

	s4.setOpt(Opt::BROADCAST, option);
	s6.setOpt(Opt::BROADCAST, option);

	EXPECT_EQ(0, getsockopt(s4.getSocket(), SOL_SOCKET, SO_BROADCAST, &optval,
	                        &optlen));
	ASSERT_EQ(1, optval);

	EXPECT_EQ(0, getsockopt(s6.getSocket(), SOL_SOCKET, SO_BROADCAST, &optval,
	                        &optlen));
	ASSERT_EQ(1, optval);


	optval = 0;
	EXPECT_EQ(
	  0, setsockopt(s4.getSocket(), SOL_SOCKET, SO_BROADCAST, &optval, optlen));
	EXPECT_EQ(
	  0, setsockopt(s6.getSocket(), SOL_SOCKET, SO_BROADCAST, &optval, optlen));

	auto opt4 = s4.getOpt(Opt::BROADCAST);
	auto opt6 = s6.getOpt(Opt::BROADCAST);

	ASSERT_EQ(0, opt4.getValue());
	ASSERT_EQ(0, opt6.getValue());

	Socket tcpSocket(Domain::IPv4, Type::TCP);
	SockOpt badOpt(1);
	ASSERT_EQ(1, badOpt.getValue());

	// Does nothing to TCP still valid
	ASSERT_NO_THROW(tcpSocket.setOpt(Opt::BROADCAST, badOpt));
}


TEST(SocketOptions, Linger)
{
	Socket s(Domain::IPv4, Type::TCP);
	SockOpt opt(true, 30);
	s.setOpt(Opt::LINGER, opt);

	linger lin;
	socklen_t len = sizeof(lin);

	EXPECT_EQ(0, getsockopt(s.getSocket(), SOL_SOCKET, SO_LINGER, &lin, &len));
	ASSERT_EQ(lin, opt);

	lin.l_onoff  = 1;
	lin.l_linger = 2;
	EXPECT_EQ(0, setsockopt(s.getSocket(), SOL_SOCKET, SO_LINGER, &lin, len));

	SockOpt opt2(true, 2);
	ASSERT_EQ(lin, opt2);
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


TEST(SocketOptions, DontRoute)
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


TEST(SocketOptions, KeepAlive)
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


TEST(SocketOptions, OOBInline)
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


TEST(SocketOptions, RCVLOWAT)
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
