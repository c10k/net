#include "socket.hpp"
#include <iostream>
#include <thread>
#include <vector>

using namespace net;
using namespace std::chrono_literals;


void connect_send()
{
	try {
		Socket s(Domain::IPv4, Type::TCP);
		std::this_thread::sleep_for(2s);
		s.connect("127.0.0.1", 24001);
		s.send("123456789");
	} catch (std::exception &e) {
		std::cerr << e.what() << "\n";
	}
}

int main()
{
	std::vector<std::thread> threads;
	for (int i = 0; i < 100; ++i) {
		threads.push_back(std::thread(connect_send));
	}
	for (auto &t : threads) {
		t.join();
	}
}
