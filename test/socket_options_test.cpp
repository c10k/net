#include "socket.hpp"
#include <iostream>
#include <gtest/gtest.h>

using namespace net;

TEST(socket, options)
{
	{
		Socket s(SF::domain::IPv4, SF::type::UDP);
		SF::sockOpt s1;
		s1.setValue(1);
		int optval;
		socklen_t optlen = sizeof(optval);
		// EXPECT_EQ(1,s1.getValue());
		s.setOpt(SF::opt::BROADCAST, s1);
		EXPECT_EQ(0, getsockopt(s.getSocket(), SOL_SOCKET, SO_BROADCAST,
		                        &optval, &optlen));
		EXPECT_EQ(1, optval);
		optval = 0;

		EXPECT_EQ(0, setsockopt(s.getSocket(), SOL_SOCKET, SO_BROADCAST,
		                        &optval, optlen));
		SF::sockOpt s2;
		s2 = s.getOpt(SF::opt::BROADCAST);
		EXPECT_EQ(0, s2.getValue());
	}

	std::cout << " test 1 completed\n";
	{
		Socket s(SF::domain::IPv4, SF::type::TCP);
		SF::sockOpt s1;
		linger lin;
		socklen_t len = sizeof(lin);
		s1.setLinger(true, 30);
		s.setOpt(SF::opt::LINGER, s1);
		EXPECT_EQ(0,
		          getsockopt(s.getSocket(), SOL_SOCKET, SO_LINGER, &lin, &len));
		linger test;
		test.l_onoff  = true;
		test.l_linger = 30;
		EXPECT_EQ(test.l_onoff, lin.l_onoff);
		EXPECT_EQ(test.l_linger, lin.l_linger);
	}

	std::cout << " test 2 completed\n";
	{
		Socket s(SF::domain::IPv4, SF::type::TCP);
		SF::sockOpt s1;
		linger lin;
		socklen_t len = sizeof(lin);
		lin.l_onoff   = true;
		lin.l_linger  = 20;
		EXPECT_EQ(0,
		          setsockopt(s.getSocket(), SOL_SOCKET, SO_LINGER, &lin, len));
		linger test;
		test.l_onoff  = true;
		test.l_linger = 20;
		s1            = s.getOpt(SF::opt::LINGER);
		EXPECT_EQ(
		  std::make_pair(static_cast<bool>(test.l_onoff), test.l_linger),
		  s1.getLinger());
	}

	std::cout << " test 3 completed\n";
	// {
	// 	Socket s(SF::domain::IPv4, SF::type::TCP);
	// 	SF::sockOpt s1;
	// 	s1.setValue(1);
	// 	int optval;
	// 	socklen_t optlen = sizeof(optval);
	// 	// EXPECT_EQ(1,s1.getValue());
	// 	s.setOpt(SF::opt::DEBUG, s1);
	// 	EXPECT_EQ(
	// 	  0, getsockopt(s.getSocket(), SOL_SOCKET, SO_DEBUG, &optval, &optlen));
	// 	EXPECT_EQ(1, optval);
	// 	optval = 0;

	// 	EXPECT_EQ(
	// 	  0, setsockopt(s.getSocket(), SOL_SOCKET, SO_DEBUG, &optval, optlen));
	// 	SF::sockOpt s2;
	// 	s2 = s.getOpt(SF::opt::DEBUG);
	// 	EXPECT_EQ(0, s2.getValue());
	// }

	// std::cout << " test 4 completed\n";
	// {
	// 	Socket s(SF::domain::IPv4, SF::type::UDP);
	// 	SF::sockOpt s1;
	// 	s1.setValue(1);
	// 	int optval;
	// 	socklen_t optlen = sizeof(optval);
	// 	// EXPECT_EQ(1,s1.getValue());
	// 	s.setOpt(SF::opt::DONTROUTE, s1);
	// 	EXPECT_EQ(0, getsockopt(s.getSocket(), SOL_SOCKET, SO_DONTROUTE,
	// 	                        &optval, &optlen));
	// 	EXPECT_EQ(1, optval);
	// 	optval = 0;

	// 	EXPECT_EQ(0, setsockopt(s.getSocket(), SOL_SOCKET, SO_DONTROUTE,
	// 	                        &optval, optlen));
	// 	SF::sockOpt s2;
	// 	s2 = s.getOpt(SF::opt::DONTROUTE);
	// 	EXPECT_EQ(0, s2.getValue());
	// }

	// std::cout << " test 5 completed\n";
	// {
	// 	Socket s(SF::domain::IPv4, SF::type::UDP);
	// 	SF::sockOpt s1;
	// 	s1.setValue(1);
	// 	int optval;
	// 	socklen_t optlen = sizeof(optval);
	// 	// EXPECT_EQ(1,s1.getValue());
	// 	s.setOpt(SF::opt::KEEPALIVE, s1);
	// 	EXPECT_EQ(0, getsockopt(s.getSocket(), SOL_SOCKET, SO_KEEPALIVE,
	// 	                        &optval, &optlen));
	// 	EXPECT_EQ(1, optval);
	// 	optval = 0;

	// 	EXPECT_EQ(0, setsockopt(s.getSocket(), SOL_SOCKET, SO_KEEPALIVE,
	// 	                        &optval, optlen));
	// 	SF::sockOpt s2;
	// 	s2 = s.getOpt(SF::opt::KEEPALIVE);
	// 	EXPECT_EQ(0, s2.getValue());
	// }

	// std::cout << " test 6 completed\n";
	// {
	// 	Socket s(SF::domain::IPv4, SF::type::UDP);
	// 	SF::sockOpt s1;
	// 	s1.setValue(1);
	// 	int optval;
	// 	socklen_t optlen = sizeof(optval);
	// 	// EXPECT_EQ(1,s1.getValue());
	// 	s.setOpt(SF::opt::OOBINLINE, s1);
	// 	EXPECT_EQ(0, getsockopt(s.getSocket(), SOL_SOCKET, SO_OOBINLINE,
	// 	                        &optval, &optlen));
	// 	EXPECT_EQ(1, optval);
	// 	optval = 0;

	// 	EXPECT_EQ(0, setsockopt(s.getSocket(), SOL_SOCKET, SO_OOBINLINE,
	// 	                        &optval, optlen));
	// 	SF::sockOpt s2;
	// 	s2 = s.getOpt(SF::opt::OOBINLINE);
	// 	EXPECT_EQ(0, s2.getValue());
	// }

	// std::cout << " test 7 completed\n";
	// {
	// 	Socket s(SF::domain::IPv4, SF::type::TCP);
	// 	SF::sockOpt s1;
	// 	s1.setValue(10);
	// 	int optval;
	// 	socklen_t optlen = sizeof(optval);
	// 	// EXPECT_EQ(1,s1.getValue());
	// 	s.setOpt(SF::opt::RCVLOWAT, s1);
	// 	EXPECT_EQ(0, getsockopt(s.getSocket(), SOL_SOCKET, SO_RCVLOWAT, &optval,
	// 	                        &optlen));
	// 	EXPECT_EQ(10, optval);
	// 	optval = 10;

	// 	EXPECT_EQ(0, setsockopt(s.getSocket(), SOL_SOCKET, SO_RCVLOWAT, &optval,
	// 	                        optlen));
	// 	SF::sockOpt s2;
	// 	s2 = s.getOpt(SF::opt::RCVLOWAT);
	// 	EXPECT_EQ(10, s2.getValue());
	// }
	// std::cout << " test 8 completed\n";
}
