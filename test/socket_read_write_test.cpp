#include "socket.hpp"
#include <gtest/gtest.h>
#include <thread>
#include <string>
#include <algorithm>

using namespace net;
using namespace std::chrono_literals;


namespace readTest {

const auto msgLen2 = 15000;
const std::string msg1(readTest::msgLen2, 'a');

void udpIPv4ServerProcessing(Socket &serverSocket)
{
	const auto res = serverSocket.read(readTest::msgLen2);
	EXPECT_EQ(res, readTest::msg1);
}

void udpIPv6ServerProcessing(Socket &serverSocket)
{
	const auto res = serverSocket.read(readTest::msgLen2);
	EXPECT_EQ(res, readTest::msg1);
}

void tcpIPv4ServerProcessing(Socket &serverSocket)
{
	const auto peer = serverSocket.accept();

	const auto msg = peer.read(readTest::msgLen2);
	if (msg == readTest::msg1) {
		peer.write("readTest::msg1");
	} else {
		peer.write(" ");
	}
	std::this_thread::sleep_for(1s);
}

void tcpIPv6ServerProcessing(Socket &serverSocket)
{
	const auto peer = serverSocket.accept();

	const auto msg = peer.read(readTest::msgLen2);
	if (msg == readTest::msg1) {
		peer.write("readTest::msg1");
	} else {
		peer.write(" ");
	}
	std::this_thread::sleep_for(1s);
}

void startTCPServerIPv6()
{
	Socket myServerSocket1(Domain::IPv6, Type::TCP);
	myServerSocket1.start("::1", 20000);
	tcpIPv6ServerProcessing(myServerSocket1);
}


void startTCPServerIPv4()
{
	Socket myServerSocket1(Domain::IPv4, Type::TCP);
	myServerSocket1.start("127.0.0.1", 19000);
	tcpIPv4ServerProcessing(myServerSocket1);
}

void startUDPServerIPv6()
{
	Socket myServerSocket1(Domain::IPv6, Type::UDP);
	myServerSocket1.start("::1", 20000);
	udpIPv6ServerProcessing(myServerSocket1);
}


void startUDPServerIPv4()
{
	Socket myServerSocket1(Domain::IPv4, Type::UDP);
	myServerSocket1.start("127.0.0.1", 19000);
	udpIPv4ServerProcessing(myServerSocket1);
}
}


TEST(Socket, IPv4ReadWrite)
{
	std::thread tcpServerThreadIPv4(readTest::startTCPServerIPv4);
	std::thread udpServerThreadIPv4(readTest::startUDPServerIPv4);
	std::this_thread::sleep_for(1s);

	Socket tcpClient1(Domain::IPv4, Type::TCP);
	tcpClient1.connect("127.0.0.1", 19000);

	EXPECT_NO_THROW(tcpClient1.write(readTest::msg1));
	EXPECT_EQ(tcpClient1.read(4), "readTest::msg1");

	tcpClient1.close();

	Socket udpClient1(Domain::IPv4, Type::UDP);
	udpClient1.connect("127.0.0.1", 19000);
	EXPECT_NO_THROW(udpClient1.write(readTest::msg1));

	tcpServerThreadIPv4.join();
	udpServerThreadIPv4.join();
}

TEST(Socket, IPv6ReadWrite)
{

	std::thread tcpServerThreadIPv6(readTest::startTCPServerIPv6);
	std::thread udpServerThreadIPv6(readTest::startUDPServerIPv6);
	std::this_thread::sleep_for(1s);

	Socket tcpClient2(Domain::IPv6, Type::TCP);
	tcpClient2.connect("::1", 20000);

	EXPECT_NO_THROW(tcpClient2.write(readTest::msg1));
	EXPECT_EQ(tcpClient2.read(4), "readTest::msg1");

	tcpClient2.close();

	Socket udpClient2(Domain::IPv6, Type::UDP);
	udpClient2.connect("::1", 20000);
	EXPECT_NO_THROW(udpClient2.write(readTest::msg1));

	tcpServerThreadIPv6.join();
	udpServerThreadIPv6.join();
}