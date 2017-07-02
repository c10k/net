#include "socket.hpp"
#include <gtest/gtest.h>

using namespace net;

TEST(SocketOptions, GetType)
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


TEST(SocketOptions, EqualityTest)
{
	int value = 1;
	linger l;
	l.l_onoff  = 1;
	l.l_linger = 5;
	timeval t;
	t.tv_sec  = 5;
	t.tv_usec = 500;

	SockOpt opt1(1), opt2(true, 5), opt3(5L, 500L);

	ASSERT_EQ(value, opt1);
	ASSERT_EQ(value, opt1.getValue());
	ASSERT_FALSE(l == opt1);
	ASSERT_FALSE(t == opt1);

	ASSERT_EQ(l, opt2);
	ASSERT_EQ(l.l_onoff, opt2.getLinger().first);
	ASSERT_EQ(l.l_linger, opt2.getLinger().second);
	ASSERT_FALSE(value == opt2);
	ASSERT_FALSE(t == opt2);

	ASSERT_EQ(t, opt3);
	ASSERT_EQ(t.tv_sec, opt3.getTime().first);
	ASSERT_EQ(t.tv_usec, opt3.getTime().second);
	ASSERT_FALSE(value == opt3);
	ASSERT_FALSE(l == opt3);
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

	const auto opt4 = s4.getOpt(Opt::BROADCAST);
	const auto opt6 = s6.getOpt(Opt::BROADCAST);

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

	const auto opt3 = s.getOpt(Opt::LINGER);
	ASSERT_EQ(lin, opt3);
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
	const auto opt2 = s.getOpt(Opt::DONTROUTE);
	ASSERT_EQ(0, opt2);
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
	const auto opt2 = s.getOpt(Opt::KEEPALIVE);
	ASSERT_EQ(0, opt2);
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

	const auto opt2 = s.getOpt(Opt::OOBINLINE);
	ASSERT_EQ(0, opt2);
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

	const auto opt2 = s.getOpt(Opt::RCVLOWAT);
	ASSERT_EQ(10, opt2);
}


TEST(SocketOptions, RCVTIMEO)
{
	Socket s(Domain::IPv4, Type::TCP);
	SockOpt opt(2L, 5000L);
	s.setOpt(Opt::RCVTIMEO, opt);

	timeval t;
	socklen_t len = sizeof(t);
	EXPECT_EQ(0, getsockopt(s.getSocket(), SOL_SOCKET, SO_RCVTIMEO, &t, &len));

	// No gurantee about tv_usec to be equal to what is set
	ASSERT_EQ(t.tv_sec, opt.getTime().first);

	t.tv_sec  = 3;
	t.tv_usec = 500;
	EXPECT_EQ(0, setsockopt(s.getSocket(), SOL_SOCKET, SO_RCVTIMEO, &t, len));

	SockOpt opt2(3L, 500L);
	ASSERT_EQ(t, opt2);

	const auto opt3 = s.getOpt(Opt::RCVTIMEO);
	ASSERT_EQ(t.tv_sec, opt3.getTime().first);
}


TEST(SocketOptions, SNDTIMEO)
{
	Socket s(Domain::IPv4, Type::TCP);

	SockOpt opt(2L, 500L);
	s.setOpt(Opt::SNDTIMEO, opt);

	timeval t;
	socklen_t len = sizeof(t);
	EXPECT_EQ(0, getsockopt(s.getSocket(), SOL_SOCKET, SO_SNDTIMEO, &t, &len));

	// No gurantee about tv_usec to be equal to what is set
	ASSERT_EQ(t.tv_sec, opt.getTime().first);

	t.tv_sec  = 30;
	t.tv_usec = 1000;
	EXPECT_EQ(0, setsockopt(s.getSocket(), SOL_SOCKET, SO_SNDTIMEO, &t, len));

	SockOpt opt2(30L, 1000L);
	ASSERT_EQ(t, opt2);

	const auto opt3 = s.getOpt(Opt::SNDTIMEO);
	ASSERT_EQ(t.tv_sec, opt3.getTime().first);
}


TEST(SocketOptions, MAXSEG)
{
	// This option ignores any value being set, atleast on my machine!
	Socket s(Domain::IPv4, Type::TCP);
	SockOpt opt(1024);
	ASSERT_NO_THROW(s.setOpt(Opt::MAXSEG, opt));
	ASSERT_NO_THROW(s.getOpt(Opt::MAXSEG));
}


TEST(SocketOptions, NODELAY)
{
	Socket s(Domain::IPv4, Type::TCP);

	int optval;
	socklen_t optlen = sizeof(optval);

	SockOpt opt(1);
	s.setOpt(Opt::NODELAY, opt);
	ASSERT_EQ(
	  0, getsockopt(s.getSocket(), IPPROTO_TCP, TCP_NODELAY, &optval, &optlen));
	ASSERT_EQ(1, optval);
	optval = 0;

	ASSERT_EQ(
	  0, setsockopt(s.getSocket(), IPPROTO_TCP, TCP_NODELAY, &optval, optlen));
	const auto opt2 = s.getOpt(Opt::NODELAY);
	ASSERT_EQ(0, opt2);
}
