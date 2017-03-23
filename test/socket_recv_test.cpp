#include "socket.hpp"
#include <gtest/gtest.h>
#include <thread>
#include <string>
#include <algorithm>

using namespace net;
using namespace std::chrono_literals;


namespace recvTest {

const auto msgLen2 = 15000;
const std::string msg1(recvTest::msgLen2, 'a');
const std::string msg2(recvTest::msgLen2, 'b');
const std::string msg3(recvTest::msgLen2, 'c');


void udpIPv4ServerProcessing(Socket &serverSocket)
{
	const auto res = serverSocket.recv(recvTest::msgLen2, [](AddrIPv4 &s) {
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
	EXPECT_EQ(res, recvTest::msg1);
}

void udpIPv6ServerProcessing(Socket &serverSocket)
{
	const auto res = serverSocket.recv(recvTest::msgLen2, [](AddrIPv6 &s) {
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
	EXPECT_EQ(res, recvTest::msg1);
}

std::string getPeerIp(const Socket &peer)
{
	std::string actualIp(100, ' ');

	sockaddr_in actualPeerIp4;
	sockaddr_in6 actualPeerIp6;

	int status          = 0;
	const char *status2 = nullptr;
	auto d              = peer.getDomain();

	switch (d) {

		case Domain::IPv4: {
			socklen_t actualPeerIpLen4 = sizeof(actualPeerIp4);

			status = getpeername(peer.getSocket(), (sockaddr *) &actualPeerIp4,
			                     &actualPeerIpLen4);

			status2
			  = inet_ntop(static_cast<int>(d), &actualPeerIp4.sin_addr.s_addr,
			              &actualIp.front(), actualIp.size());
		} break;

		case Domain::IPv6: {
			socklen_t actualPeerIpLen6 = sizeof(actualPeerIp6);

			status = getpeername(peer.getSocket(), (sockaddr *) &actualPeerIp6,
			                     &actualPeerIpLen6);

			status2
			  = inet_ntop(static_cast<int>(d), &actualPeerIp6.sin6_addr.s6_addr,
			              &actualIp.front(), actualIp.size());
		} break;

		default: break;
	}

	if (status == -1) {
		throw std::runtime_error("Error in getpeername in getPeerIp()");
	}
	if (status2 == nullptr) {
		throw std::runtime_error("Error in inet_ntop in getPeerIp()");
	}

	actualIp.erase(std::remove(actualIp.begin(), actualIp.end(), ' '),
	               actualIp.end());
	return actualIp;
}

void tcpIPv4ServerProcessing(Socket &serverSocket)
{
	const auto peer = serverSocket.accept();

	const auto msg = peer.read(recvTest::msgLen2);
	if (msg == recvTest::msg1) {
		peer.write("recvTest::msg1");
	} else {
		peer.write(" ");
	}

	const auto otherMsg = peer.recv(recvTest::msgLen2, [&](AddrIPv4 &s) {

		const auto actualPeerIp = getPeerIp(peer);

		std::string str(100, ' ');
		const auto ptr
		  = inet_ntop(AF_INET, &s.sin_addr.s_addr, &str.front(), str.size());
		if (ptr != nullptr) {
			str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
			EXPECT_NE(str.substr(0, str.size() - 1), actualPeerIp);
		} else {
			throw std::invalid_argument("Peer address not valid");
		}
	});

	if (otherMsg == recvTest::msg2) {
		peer.send("recvTest::msg2");
	} else {
		peer.send(" ");
	}

	const auto yetAnotherMsg = peer.recv(recvTest::msgLen2);
	if (yetAnotherMsg == recvTest::msg3) {
		peer.send("recvTest::msg3");
	} else {
		peer.send(" ");
	}
	std::this_thread::sleep_for(1s);
}

void tcpIPv6ServerProcessing(Socket &serverSocket)
{
	const auto peer = serverSocket.accept();

	const auto msg = peer.read(recvTest::msgLen2);
	if (msg == recvTest::msg1) {
		peer.write("recvTest::msg1");
	} else {
		peer.write(" ");
	}

	const auto otherMsg = peer.recv(recvTest::msgLen2, [&](AddrIPv6 &s) {

		const auto actualPeerIp = getPeerIp(peer);

		std::string str(100, ' ');
		const auto ptr
		  = inet_ntop(AF_INET6, &s.sin6_addr.s6_addr, &str.front(), str.size());
		if (ptr != nullptr) {
			str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
			EXPECT_NE(str.substr(0, str.size() - 1), actualPeerIp);
		} else {
			throw std::invalid_argument("Peer address not valid");
		}
	});
	if (otherMsg == recvTest::msg2) {
		peer.send("recvTest::msg2");
	} else {
		peer.send(" ");
	}

	const auto yetAnotherMsg = peer.recv(recvTest::msgLen2);
	if (yetAnotherMsg == recvTest::msg3) {
		peer.send("recvTest::msg3");
	} else {
		peer.send(" ");
	}
	std::this_thread::sleep_for(1s);
}

void startTCPServerIPv6()
{
	Socket myServerSocket1(Domain::IPv6, Type::TCP);
	myServerSocket1.start("::1", 18000);
	tcpIPv6ServerProcessing(myServerSocket1);
}


void startTCPServerIPv4()
{
	Socket myServerSocket1(Domain::IPv4, Type::TCP);
	myServerSocket1.start("127.0.0.1", 17000);
	tcpIPv4ServerProcessing(myServerSocket1);
}

void startUDPServerIPv6()
{
	Socket myServerSocket1(Domain::IPv6, Type::UDP);
	myServerSocket1.start("::1", 18000);  // just binding
	udpIPv6ServerProcessing(myServerSocket1);
}


void startUDPServerIPv4()
{
	Socket myServerSocket1(Domain::IPv4, Type::UDP);
	myServerSocket1.start("127.0.0.1", 17000);  // just binding
	udpIPv4ServerProcessing(myServerSocket1);
}
}


TEST(Socket, IPv4Recv)
{
	std::thread tcpServerThreadIPv4(recvTest::startTCPServerIPv4);
	std::thread udpServerThreadIPv4(recvTest::startUDPServerIPv4);
	std::this_thread::sleep_for(1s);

	Socket tcpClient1(Domain::IPv4, Type::TCP);
	tcpClient1.connect("127.0.0.1", 17000);

	EXPECT_NO_THROW(tcpClient1.send(recvTest::msg1));
	EXPECT_EQ(tcpClient1.read(4), "recvTest::msg1");

	EXPECT_NO_THROW(tcpClient1.send(recvTest::msg2));
	EXPECT_EQ(tcpClient1.read(4), "recvTest::msg2");

	EXPECT_NO_THROW(tcpClient1.send(recvTest::msg3));
	EXPECT_EQ(tcpClient1.read(4), "recvTest::msg3");

	tcpClient1.close();

	Socket udpClient1(Domain::IPv4, Type::UDP);
	EXPECT_NO_THROW(udpClient1.send(recvTest::msg1, [](AddrIPv4 &s) {
		return methods::construct(s, "127.0.0.1", 17000);
	}));

	tcpServerThreadIPv4.join();
	udpServerThreadIPv4.join();
}

TEST(Socket, IPv6Recv)
{

	std::thread tcpServerThreadIPv6(recvTest::startTCPServerIPv6);
	std::thread udpServerThreadIPv6(recvTest::startUDPServerIPv6);
	std::this_thread::sleep_for(1s);

	Socket tcpClient2(Domain::IPv6, Type::TCP);
	tcpClient2.connect("::1", 18000);

	EXPECT_NO_THROW(tcpClient2.send(recvTest::msg1));
	EXPECT_EQ(tcpClient2.read(4), "recvTest::msg1");

	EXPECT_NO_THROW(tcpClient2.send(recvTest::msg2));
	EXPECT_EQ(tcpClient2.read(4), "recvTest::msg2");

	EXPECT_NO_THROW(tcpClient2.send(recvTest::msg3));
	EXPECT_EQ(tcpClient2.read(4), "recvTest::msg3");

	tcpClient2.close();

	Socket udpClient2(Domain::IPv6, Type::UDP);
	EXPECT_NO_THROW(udpClient2.send(recvTest::msg1, [](AddrIPv6 &s) {
		return methods::construct(s, "::1", 18000);
	}));

	tcpServerThreadIPv6.join();
	udpServerThreadIPv6.join();
}