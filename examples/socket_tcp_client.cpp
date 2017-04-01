#include "socket.hpp"
#include <iostream>

using namespace net;

int main()
{
	try {

		Socket s(Domain::IPv4, Type::TCP);
		s.connect("0.0.0.0", 24000);
		s.send("Hello World!");

	} catch (std::exception &e) {
		std::cerr << e.what() << '\n';
	}
}
