#include "socket.hpp"
#include <gtest/gtest.h>
#include <thread>
#include <functional>
#include <iostream>


using namespace net;
using namespace std::chrono_literals;

namespace {
void runServer(Socket &s, const unsigned int port)
{
	std::string address;
	switch (s.getDomain()) {
		case Domain::IPv4: address = "127.0.0.1"; break;
		case Domain::IPv6: address = "::1"; break;

		default: address = " ";
	}

	try {
		s.start(address.c_str(), port);
		const auto peer = s.accept();
	} catch (std::exception &e) {
		std::cerr << "Exception: " << e.what();
	}
}
}

TEST(Socket, Connectv4)
{
	Socket server4(Domain::IPv4, Type::TCP);

	std::thread serverThread1(runServer, std::ref(server4), 15010);
	serverThread1.detach();
	std::this_thread::sleep_for(1s);

	Socket client4(Domain::IPv4, Type::TCP);
	ASSERT_NO_THROW(client4.connect("127.0.0.1", 15010));
	client4.close();
}


TEST(Socket, Connectv6)
{
	Socket server6(Domain::IPv6, Type::TCP);
	std::thread serverThread1(runServer, std::ref(server6), 15020);
	serverThread1.detach();
	std::this_thread::sleep_for(1s);

	Socket client6(Domain::IPv6, Type::TCP);
	ASSERT_NO_THROW(client6.connect("::1", 15020));
	client6.close();
}
