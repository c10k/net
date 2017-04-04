#include "socket.hpp"
#include <iostream>
#include <signal.h>

using namespace net;

int main()
{
	try {
		signal(SIGCHLD, SIG_IGN);
		Socket s(Domain::IPv4, Type::TCP);
		s.start("127.0.0.1", 24001);

		while (true) {
			const auto peer = s.accept();
			if (!fork()) {
				std::cout << peer.recv(10) << '\n';
				return 0;
			}
		}
	} catch (std::exception &e) {
		std::cerr << e.what() << '\n';
	}
}
