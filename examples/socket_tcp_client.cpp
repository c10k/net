#include "socket.hpp"
#include <iostream>

using namespace net;

int main()
{
	try {
		Socket s(SF::domain::IPv4, SF::type::TCP);
		s.connect("0.0.0.0", 24000);
		s.write("Hello World!!");
	} catch (std::exception &e) {
		std::cerr << e.what() << "\n";
	}
	return 0;
}
