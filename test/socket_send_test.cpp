#include "socket.hpp"
#include <gtest/gtest.h>
#include <thread>
#include <string>
#include <algorithm>

using namespace net;
using namespace std::chrono_literals;

void udpIPv4ServerProcessing(Socket &serverSocket)
{
	for (int i = 1; i <= 1; i++) {
		auto res = serverSocket.recv(15000, [](AddrIPv4 &s) {
			std::string str(100, ' ');
			auto ptr = inet_ntop(AF_INET, &s.sin_addr.s_addr, &str.front(),
			                     str.size());
			if (ptr != nullptr) {
				str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
				ASSERT_EQ(str.substr(0, str.size() - 1), "127.0.0.1");
			} else {
				throw std::invalid_argument("Peer address not valid");
			}
		});
	}
}

void udpIPv6ServerProcessing(Socket &serverSocket)
{
	for (int i = 1; i <= 1; i++) {
		auto res = serverSocket.recv(15000, [](AddrIPv6 &s) {
			std::string str(100, ' ');
			auto ptr = inet_ntop(AF_INET6, &s.sin6_addr.s6_addr, &str.front(),
			                     str.size());
			if (ptr != nullptr) {
				str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
				ASSERT_EQ(str.substr(0, str.size() - 1), "::1");
			} else {
				throw std::invalid_argument("Peer address not valid");
			}
		});
	}
}

void tcpServerProcessing(Socket &serverSocket)
{
	for (int i = 1; i <= 1; i++) {
		auto peer = serverSocket.accept();

		auto recvd = 0;
		while (!(recvd >= 15000)) {
			auto msg = peer.read(1024);
			recvd += msg.size();
			//
		}
		auto response = std::to_string(recvd);
		peer.send(response);

		// recvd = 0;
		// while (!(recvd >= 15000)) {
		// 	auto msg = peer.read(1024);
		// 	recvd += msg.size();
		// 	//
		// }
		// response = std::to_string(recvd);
		// peer.send(response);

		recvd = 0;
		while (!(recvd >= 15000)) {
			auto msg = peer.read(1024);
			recvd += msg.size();
			//
		}
		response = std::to_string(recvd);
		peer.send(response);
	}
}

void startTCPServerIPv6()
{
	Socket myServerSocket(Domain::IPv6, Type::TCP);
	myServerSocket.start("::1", 16000);
	tcpServerProcessing(myServerSocket);
}


void startTCPServerIPv4()
{
	Socket myServerSocket(Domain::IPv4, Type::TCP);
	myServerSocket.start("127.0.0.1", 15000);
	tcpServerProcessing(myServerSocket);
}

void startUDPServerIPv6()
{
	Socket myServerSocket(Domain::IPv6, Type::UDP);
	myServerSocket.start("::1", 16000);
	udpIPv6ServerProcessing(myServerSocket);
}


void startUDPServerIPv4()
{
	Socket myServerSocket(Domain::IPv4, Type::UDP);
	myServerSocket.start("127.0.0.1", 15000);
	udpIPv4ServerProcessing(myServerSocket);
}

TEST(Socket, Send)
{
	std::thread tcpServerThreadIPv4(startTCPServerIPv4);
	std::thread tcpServerThreadIPv6(startTCPServerIPv6);
	std::thread udpServerThreadIPv4(startUDPServerIPv4);
	std::thread udpServerThreadIPv6(startUDPServerIPv6);
	std::this_thread::sleep_for(3s);

	std::string someString(15000, 'a');
	auto msgLen = 15000;

	Socket client1(Domain::IPv4, Type::TCP);
	client1.connect("127.0.0.1", 15000);
	EXPECT_NO_THROW(client1.send(someString));
	EXPECT_EQ(client1.read(5), std::to_string(msgLen));
	// !! Resolve This !! and then accordingly change commented portion of tcp
	// server.
	// EXPECT_THROW(client1.send(someString, [](AddrIPv4 &s) { return 5; }),
	//              std::runtime_error);
	// EXPECT_NE(client1.read(5), std::to_string(msgLen));
	EXPECT_NO_THROW(client1.send(someString, [](AddrIPv4 &s) {
		s.sin_family      = AF_INET;
		s.sin_port        = htons(15000);
		s.sin_addr.s_addr = htonl(INADDR_ANY);
		return 1;
	}));
	EXPECT_NE(client1.read(5), std::to_string(msgLen));
	client1.close();

	Socket client2(Domain::IPv6, Type::TCP);
	client2.connect("::1", 16000);
	EXPECT_NO_THROW(client2.send(someString));
	EXPECT_EQ(client2.read(5), std::to_string(msgLen));
	// !! Resolve This !! and then accordingly change commented portion of tcp
	// server.
	// EXPECT_THROW(client2.send(someString, [](AddrIPv6 &s) { return 5; }),
	//              std::runtime_error);
	// EXPECT_NE(client2.read(5), std::to_string(msgLen));
	EXPECT_NO_THROW(client2.send(someString, [](AddrIPv6 &s) {
		s.sin6_family = AF_INET;
		s.sin6_port   = htons(16000);
		s.sin6_addr   = in6addr_any;
		return 1;
	}));
	EXPECT_NE(client2.read(5), std::to_string(msgLen));
	client2.close();


	Socket udpClient1(Domain::IPv4, Type::UDP);
	EXPECT_NO_THROW(udpClient1.send(someString, [](AddrIPv4 &s) {
		return methods::construct(s, "127.0.0.1", 15000);
	}));


	Socket udpClient2(Domain::IPv6, Type::UDP);
	EXPECT_NO_THROW(udpClient2.send(someString, [](AddrIPv6 &s) {
		return methods::construct(s, "::1", 16000);
	}));

	Socket unixClient(Domain::UNIX, Type::TCP);
	

	tcpServerThreadIPv4.join();
	tcpServerThreadIPv6.join();
	udpServerThreadIPv4.join();
	udpServerThreadIPv6.join();


	// ipv4 client trying to send data to ipv6 server but obviously unsucessful.
	// Socket ipv4Client(Domain::IPv4, Type::TCP);
	// EXPECT_ANY_THROW(ipv4Client.connect("::1", 16000));
	// EXPECT_NO_THROW(ipv4Client.send(someString));
	// EXPECT_EQ(ipv4Client.read(5), std::to_string(msgLen));
	//

	// ipv6 client sending data to ipv4 server but fails
	// Socket ipv6Client(Domain::IPv6, Type::TCP);
	// EXPECT_NO_THROW(ipv6Client.connect("127.0.0.1", 15000));
	// EXPECT_NO_THROW(ipv6Client.send(someString));
	// EXPECT_EQ(ipv6Client.read(5), std::to_string(msgLen));
	//
	// ipv6Client.close();

	// Socket incompatibleClient(Domain::IPv4, Type::UDP);
	// EXPECT_NO_THROW(incompatibleClient.send(someString, [](AddrIPv4 &s) {
	// return methods::construct(s, "127.0.0.1", 15000);
	// }));


	// Socket udpClient3(Domain::IPv4, Type::UDP);
	// udpClient3.send(someString, [](AddrIPv6 &s) {
	// 	return methods::construct(s, "::1", 16000);
	// });
	//

	// Socket udpClient4(Domain::IPv6, Type::UDP);
	// udpClient4.send(someString, [](AddrIPv4 &s) {
	// 	return methods::construct(s, "127.0.0.1", 15000);
	// });
}