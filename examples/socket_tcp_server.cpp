#include "socket.hpp"
#include <iostream>

using namespace net;

int main()
{
	try {
		Socket s(SF::domain::IPv4, SF::type::TCP);
		s.start("0.0.0.0", 24000);

		while (1) {
			auto peer = s.accept();
			auto msg  = peer.read(100);
			std::cout << msg << "\n";
		}
	} catch (std::exception &e) {
		std::cerr << e.what() << "\n";
	}
}
