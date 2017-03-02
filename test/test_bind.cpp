#include "socket.hpp"
#include <gtest/gtest.h>

using namespace net;

TEST(socket, bind)
{


	//*******************************************************IPv4**********************************************************
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


	//*******************************************************IPv6**********************************************************

	// {
	// 	Socket s(SF::domain::IPv6,SF::type::TCP);
	// 	EXPECT_THROW(s.bind([](addrIPv6 &s){
	// 						return
	// methods::construct(s,"2001:0db8:85a3:0000:0000:8a2e:0370:7334",8000);
	// 					}),	std::invalid_argument);
	// }
	// {
	// 	Socket s(SF::domain::IPv6,SF::type::TCP);
	// 	EXPECT_THROW(s.bind([](addrIPv6 &s){
	// 						return
	// methods::construct(s,"2001:0db8:85a3:0000:0000:8a2e:0370:7334",8001);
	// 					}),	std::invalid_argument);
	// }
	{
		Socket s(SF::domain::IPv6, SF::type::TCP);
		EXPECT_THROW(s.bind([](addrIPv6 &s) { return 0; }),
		             std::invalid_argument);
	}
	{
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
	// {
	// 	Socket s(SF::domain::IPv6,SF::type::TCP);
	// 	EXPECT_THROW(s.bind([](addrIPv4 &s){
	// 						return methods::construct(s,"0.0.0.0",130000000);
	// 					}),std::invalid_argument);
	// }

	// {
	// 	Socket s(SF::domain::IPv6,SF::type::TCP);
	// 	EXPECT_THROW(s.bind([](addrIPv4 &s){
	// 						return methods::construct(s,"0.0.0.0",-1300000);
	// 					}),std::invalid_argument);
	// }


	// compile time error
	// {

	// 	Socket s(SF::domain::IPv6,SF::type::TCP);
	// 	EXPECT_NO_THROW(s.bind([](addrIPv6 &s){

	// 						s.sin6_family = AF_INET6;
	// 						s.sin6_addr.s6_addr = htonl(INADDR_ANY);
	// 						s.sin6_port = htons(8000);
	// 						return 1;
	// 					}));
	// }
}