#include "socket.hpp"
#include <iostream>
#include <thread>
#include <functional>
#include <condition_variable>

using namespace net;

std::mutex m;
std::condition_variable cv;
bool accepted = false;


void worker_thread(Socket &s)
{
	try {
		const auto peer = s.accept();
		std::unique_lock<std::mutex> lock(m);
		accepted = true;
		lock.unlock();
		cv.notify_one();
		std::cout << peer.recv(10) << '\n';
	} catch (std::exception &e) {
		std::cerr << e.what() << '\n';
	}
}


int main()
{
	try {
		Socket s(Domain::IPv4, Type::TCP);
		s.start("127.0.0.1", 24001);

		while (true) {
			std::thread(worker_thread, std::ref(s)).detach();
			std::unique_lock<std::mutex> lock(m);
			cv.wait(lock, [] { return accepted; });
			accepted = false;
		}
	} catch (std::exception &e) {
		std::cerr << e.what() << '\n';
	}
}
