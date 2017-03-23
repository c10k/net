#include "socket.hpp"
#include <gtest/gtest.h>
#include <thread>
#include <string>
#include <algorithm>

using namespace net;
using namespace std::chrono_literals;


namespace sendTest {

const auto msgLen = 60000;
const std::string someString(sendTest::msgLen, 'a');

void udpIPv4ServerProcessing(Socket &serverSocket)
{
	const auto res = serverSocket.recv(sendTest::msgLen, [](AddrIPv4 &s) {
		std::string str(100, ' ');
		const auto ptr
		  = inet_ntop(AF_INET, &s.sin_addr.s_addr, &str.front(), str.size());
		if (ptr != nullptr) {
			str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
			ASSERT_EQ(str.substr(0, str.size() - 1), "127.0.0.1");
		} else {
			throw std::invalid_argument("Peer address not valid");
		}
	});
	EXPECT_EQ(res, sendTest::someString);
}

void udpIPv6ServerProcessing(Socket &serverSocket)
{
	const auto res = serverSocket.recv(sendTest::msgLen, [](AddrIPv6 &s) {
		std::string str(100, ' ');
		const auto ptr
		  = inet_ntop(AF_INET6, &s.sin6_addr.s6_addr, &str.front(), str.size());
		if (ptr != nullptr) {
			str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
			ASSERT_EQ(str.substr(0, str.size() - 1), "::1");
		} else {
			throw std::invalid_argument("Peer address not valid");
		}
	});
	EXPECT_EQ(res, sendTest::someString);
}

void tcpServerProcessing(Socket &serverSocket)
{
	const auto peer = serverSocket.accept();

	auto recvd = 0;
	while (!(recvd >= sendTest::msgLen)) {
		auto msg = peer.read(1024);
		recvd += msg.size();
	}
	auto response = std::to_string(recvd);
	peer.write(response);

	recvd = 0;
	while (!(recvd >= sendTest::msgLen)) {
		auto msg = peer.read(1024);
		recvd += msg.size();
	}
	response = std::to_string(recvd);
	peer.send(response);
	std::this_thread::sleep_for(1s);
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
	myServerSocket.start("::1", 16000);  // just binding
	udpIPv6ServerProcessing(myServerSocket);
}


void startUDPServerIPv4()
{
	Socket myServerSocket(Domain::IPv4, Type::UDP);
	myServerSocket.start("127.0.0.1", 15000);  // just binding
	udpIPv4ServerProcessing(myServerSocket);
}

TEST(Socket, IPv4Send)
{
	std::thread tcpServerThreadIPv4(startTCPServerIPv4);
	std::thread udpServerThreadIPv4(startUDPServerIPv4);
	std::this_thread::sleep_for(1s);

	Socket tcpClient1(Domain::IPv4, Type::TCP);
	tcpClient1.connect("127.0.0.1", 15000);
	EXPECT_NO_THROW(tcpClient1.send(sendTest::someString));
	EXPECT_EQ(tcpClient1.read(5), std::to_string(sendTest::msgLen));
	EXPECT_NO_THROW(tcpClient1.send(sendTest::someString, [](AddrIPv4 &s) {
		s.sin_family      = AF_INET;
		s.sin_port        = htons(15000);
		s.sin_addr.s_addr = htonl(INADDR_ANY);
		return 1;
	}));
	EXPECT_EQ(tcpClient1.read(5), std::to_string(sendTest::msgLen));
	tcpClient1.close();

	Socket udpClient1(Domain::IPv4, Type::UDP);
	EXPECT_NO_THROW(udpClient1.send(sendTest::someString, [](AddrIPv4 &s) {
		return methods::construct(s, "127.0.0.1", 15000);
	}));

	tcpServerThreadIPv4.join();
	udpServerThreadIPv4.join();
}

TEST(Socket, IPv6Send)
{

	std::thread tcpServerThreadIPv6(startTCPServerIPv6);
	std::thread udpServerThreadIPv6(startUDPServerIPv6);
	std::this_thread::sleep_for(1s);

	Socket tcpClient2(Domain::IPv6, Type::TCP);
	tcpClient2.connect("::1", 16000);
	EXPECT_NO_THROW(tcpClient2.send(sendTest::someString));
	EXPECT_EQ(tcpClient2.read(5), std::to_string(sendTest::msgLen));
	EXPECT_NO_THROW(tcpClient2.send(sendTest::someString, [](AddrIPv6 &s) {
		s.sin6_family = AF_INET6;
		s.sin6_port   = htons(16000);
		s.sin6_addr   = in6addr_any;
		return 1;
	}));
	EXPECT_EQ(tcpClient2.read(5), std::to_string(sendTest::msgLen));
	tcpClient2.close();

	Socket udpClient2(Domain::IPv6, Type::UDP);
	EXPECT_NO_THROW(udpClient2.send(sendTest::someString, [](AddrIPv6 &s) {
		return methods::construct(s, "::1", 16000);
	}));

	tcpServerThreadIPv6.join();
	udpServerThreadIPv6.join();
}
}
