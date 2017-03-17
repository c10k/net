#include "socket.hpp"
#include <gtest/gtest.h>
#include <thread>
#include <string>
#include <algorithm>

using namespace net;
using namespace std::chrono_literals;

auto msgLen2 = 15000;
std::string msg1(msgLen2, 'a');
std::string msg2(msgLen2, 'b');
std::string msg3(msgLen2, 'c');


namespace recvTest {
void udpIPv4ServerProcessing(Socket &serverSocket)
{
	for (int i = 1; i <= 1; i++) {
		auto res = serverSocket.recv(msgLen2, [](AddrIPv4 &s) {
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
		auto res = serverSocket.recv(msgLen2, [](AddrIPv6 &s) {
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

std::string getPeerIp(Socket &peer, Domain d)
{
	std::string actualIp(100, ' ');
	struct sockaddr_in actualPeerIp;
	socklen_t actualPeerIpLen = sizeof(actualPeerIp);

	auto status = getpeername(peer.getSocket(), (sockaddr *) &actualPeerIp,
	                          &actualPeerIpLen);

	auto status2 = inet_ntop(static_cast<int>(d), &actualPeerIp.sin_addr.s_addr,
	                         &actualIp.front(), actualIp.size());

	if (status != -1 && status2 != nullptr) {
		actualIp.erase(std::remove(actualIp.begin(), actualIp.end(), ' '),
		               actualIp.end());
		return actualIp;
	} else
		throw std::runtime_error("Error in getPeerIp");
}

void tcpIPv4ServerProcessing(Socket &serverSocket)
{
	for (int i = 1; i <= 1; i++) {
		auto peer = serverSocket.accept();

		auto msg = peer.read(msgLen2);
		if (msg == msg1)
			peer.send("msg1");
		else
			peer.send(" ");

		auto otherMsg = peer.recv(msgLen2, [&](AddrIPv4 &s) {

			auto actualPeerIp = getPeerIp(peer, Domain::IPv4);

			std::string str(100, ' ');
			auto ptr = inet_ntop(AF_INET, &s.sin_addr.s_addr, &str.front(),
			                     str.size());
			if (ptr != nullptr) {
				str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
				// The following statement is so bcoz, in case of recvfrom being
				// used with TCP socket, recvfrom doesn't fill the passed
				// AddrIPv4/AddrIPv6 structure and leaves it as it was passed to
				// it.
				EXPECT_NE(str.substr(0, str.size() - 1), "127.0.0.1");
			} else {
				throw std::invalid_argument("Peer address not valid");
			}
		});

		if (otherMsg == msg2)
			peer.send("msg2");
		else
			peer.send(" ");

		auto yetAnotherMsg = peer.recv(msgLen2);
		if (yetAnotherMsg == msg3)
			peer.send("msg3");
		else
			peer.send(" ");
	}
}

void tcpIPv6ServerProcessing(Socket &serverSocket)
{
	for (int i = 1; i <= 1; i++) {
		auto peer = serverSocket.accept();

		auto msg = peer.read(msgLen2);
		if (msg == msg1)
			peer.send("msg1");
		else
			peer.send(" ");

		auto otherMsg = peer.recv(msgLen2, [&](AddrIPv6 &s) {

			auto actualPeerIp = getPeerIp(peer, Domain::IPv6);

			std::string str(100, ' ');
			auto ptr = inet_ntop(AF_INET6, &s.sin6_addr.s6_addr, &str.front(),
			                     str.size());
			if (ptr != nullptr) {
				str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
				// The following statement is so bcoz, in case of recvfrom being
				// used with TCP socket, recvfrom doesn't fill the passed
				// AddrIPv4/AddrIPv6 structure and leaves it as it was passed to
				// it.
				EXPECT_NE(str.substr(0, str.size() - 1), "::1");
			} else {
				throw std::invalid_argument("Peer address not valid");
			}
		});
		if (otherMsg == msg2)
			peer.send("msg2");
		else
			peer.send(" ");

		auto yetAnotherMsg = peer.recv(msgLen2);
		if (yetAnotherMsg == msg3)
			peer.send("msg3");
		else
			peer.send(" ");
	}
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
	myServerSocket1.start("::1", 18000);
	udpIPv6ServerProcessing(myServerSocket1);
}


void startUDPServerIPv4()
{
	Socket myServerSocket1(Domain::IPv4, Type::UDP);
	myServerSocket1.start("127.0.0.1", 17000);
	udpIPv4ServerProcessing(myServerSocket1);
}
}


TEST(Socket, IPv4Recv)
{
	std::thread tcpServerThreadIPv4(recvTest::startTCPServerIPv4);
	std::thread udpServerThreadIPv4(recvTest::startUDPServerIPv4);
	std::this_thread::sleep_for(2s);

	Socket client1(Domain::IPv4, Type::TCP);
	client1.connect("127.0.0.1", 17000);

	EXPECT_NO_THROW(client1.send(msg1));
	EXPECT_EQ(client1.read(4), "msg1");

	EXPECT_NO_THROW(client1.send(msg2));
	EXPECT_EQ(client1.read(4), "msg2");

	EXPECT_NO_THROW(client1.send(msg3));
	EXPECT_EQ(client1.read(4), "msg3");

	client1.close();

	Socket udpClient1(Domain::IPv4, Type::UDP);
	EXPECT_NO_THROW(udpClient1.send(msg1, [](AddrIPv4 &s) {
		return methods::construct(s, "127.0.0.1", 17000);
	}));

	tcpServerThreadIPv4.join();
	udpServerThreadIPv4.join();
}

TEST(Socket, IPv6Recv)
{

	std::thread tcpServerThreadIPv6(recvTest::startTCPServerIPv6);
	std::thread udpServerThreadIPv6(recvTest::startUDPServerIPv6);
	std::this_thread::sleep_for(2s);

	Socket client2(Domain::IPv6, Type::TCP);
	client2.connect("::1", 18000);

	EXPECT_NO_THROW(client2.send(msg1));
	EXPECT_EQ(client2.read(4), "msg1");

	EXPECT_NO_THROW(client2.send(msg2));
	EXPECT_EQ(client2.read(4), "msg2");

	EXPECT_NO_THROW(client2.send(msg3));
	EXPECT_EQ(client2.read(4), "msg3");

	client2.close();

	Socket udpClient2(Domain::IPv6, Type::UDP);
	EXPECT_NO_THROW(udpClient2.send(
	  msg1, [](AddrIPv6 &s) { return methods::construct(s, "::1", 18000); }));

	tcpServerThreadIPv6.join();
	udpServerThreadIPv6.join();
}