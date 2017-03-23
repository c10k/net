#include "socket.hpp"
#include <gtest/gtest.h>
#include <thread>
#include <string>
#include <algorithm>

using namespace net;
using namespace std::chrono_literals;


namespace readTest {

const auto msgLen = 15000;
const std::string msg(readTest::msgLen, 'a');
const std::string unixServerPath1("/tmp/unixServerPath10");
const std::string unixServerPath2("/tmp/unixServerPath20");
const std::string unixClientPath1("/tmp/unixClientPath10");
const std::string unixClientPath2("/tmp/unixClientPath20");

void startUNIXServerTCP(const std::string path)
{
	Socket unixServer(Domain::UNIX, Type::TCP);
	EXPECT_NO_THROW(unixServer.start(path.c_str()));
	const auto peer = unixServer.accept();
	const auto res  = peer.read(readTest::msgLen);
	if (res == readTest::msg) {
		peer.write("readTest::msg");
	} else {
		peer.write(" ");
	}
	std::this_thread::sleep_for(1s);
}

void startUNIXServerUDP(const std::string path)
{
	Socket unixServer(Domain::UNIX, Type::UDP);
	EXPECT_NO_THROW(unixServer.bind(
	  [&](AddrUnix &s) { return methods::construct(s, path.c_str()); }));

	const auto res = unixServer.read(readTest::msgLen);
	EXPECT_EQ(msg, res);
}

void udpIPv4ServerProcessing(Socket &serverSocket)
{
	const auto res = serverSocket.read(readTest::msgLen);
	EXPECT_EQ(res, readTest::msg);
}

void udpIPv6ServerProcessing(Socket &serverSocket)
{
	const auto res = serverSocket.read(readTest::msgLen);
	EXPECT_EQ(res, readTest::msg);
}

void tcpIPv4ServerProcessing(Socket &serverSocket)
{
	const auto peer = serverSocket.accept();

	const auto msg = peer.read(readTest::msgLen);
	if (msg == readTest::msg) {
		peer.write("readTest::msg");
	} else {
		peer.write(" ");
	}
	std::this_thread::sleep_for(1s);
}

void tcpIPv6ServerProcessing(Socket &serverSocket)
{
	const auto peer = serverSocket.accept();

	const auto msg = peer.read(readTest::msgLen);
	if (msg == readTest::msg) {
		peer.write("readTest::msg");
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
	EXPECT_EQ(tcpClient1.getType(), Type::TCP);
	tcpClient1.connect("127.0.0.1", 19000);

	EXPECT_NO_THROW(tcpClient1.write(readTest::msg));
	EXPECT_EQ(tcpClient1.read(std::string("readTest::msg").size()),
	          "readTest::msg");

	tcpClient1.close();

	Socket udpClient1(Domain::IPv4, Type::UDP);
	EXPECT_EQ(udpClient1.getType(), Type::UDP);
	udpClient1.connect("127.0.0.1", 19000);
	EXPECT_NO_THROW(udpClient1.write(readTest::msg));

	tcpServerThreadIPv4.join();
	udpServerThreadIPv4.join();
}

TEST(Socket, IPv6ReadWrite)
{

	std::thread tcpServerThreadIPv6(readTest::startTCPServerIPv6);
	std::thread udpServerThreadIPv6(readTest::startUDPServerIPv6);
	std::this_thread::sleep_for(1s);

	Socket tcpClient2(Domain::IPv6, Type::TCP);
	EXPECT_EQ(tcpClient2.getType(), Type::TCP);
	tcpClient2.connect("::1", 20000);

	EXPECT_NO_THROW(tcpClient2.write(readTest::msg));
	EXPECT_EQ(tcpClient2.read(std::string("readTest::msg").size()),
	          "readTest::msg");

	tcpClient2.close();

	Socket udpClient2(Domain::IPv6, Type::UDP);
	EXPECT_EQ(udpClient2.getType(), Type::UDP);
	udpClient2.connect("::1", 20000);
	EXPECT_NO_THROW(udpClient2.write(readTest::msg));

	tcpServerThreadIPv6.join();
	udpServerThreadIPv6.join();
}

TEST(Socket, UNIXReadWrite)
{

	std::thread tcpServerThreadUnix(readTest::startUNIXServerTCP,
	                                std::ref(readTest::unixServerPath1));
	std::thread udpServerThreadUnix(readTest::startUNIXServerUDP,
	                                std::ref(readTest::unixServerPath2));
	std::this_thread::sleep_for(1s);

	Socket unixClient1(Domain::UNIX, Type::TCP);
	EXPECT_EQ(unixClient1.getType(), Type::TCP);
	EXPECT_NO_THROW(unixClient1.bind([&](AddrUnix &s) {
		return methods::construct(s, readTest::unixClientPath1.c_str());
	}));
	EXPECT_NO_THROW(unixClient1.connect(readTest::unixServerPath1.c_str()));

	EXPECT_NO_THROW(unixClient1.write(readTest::msg));
	EXPECT_EQ(unixClient1.read(std::string("readTest::msg").size()),
	          "readTest::msg");

	unixClient1.close();

	Socket unixClient2(Domain::UNIX, Type::UDP);
	EXPECT_EQ(unixClient2.getType(), Type::UDP);
	EXPECT_NO_THROW(unixClient2.bind([&](AddrUnix &s) {
		return methods::construct(s, readTest::unixClientPath2.c_str());
	}));
	EXPECT_NO_THROW(unixClient2.connect(readTest::unixServerPath2.c_str()));

	EXPECT_NO_THROW(unixClient2.write(readTest::msg));

	tcpServerThreadUnix.join();
	udpServerThreadUnix.join();
}