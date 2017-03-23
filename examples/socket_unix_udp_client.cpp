#include "socket.hpp"
#include <iostream>

using namespace net;

int main()
{
	try {
		Socket unixClient(Domain::UNIX, Type::UDP);
		std::string clientPath("/tmp/unixClient");
		std::string serverPath("/tmp/unixServer");

		unixClient.bind([&](AddrUnix &s) {
			return methods::construct(s, &clientPath.front());
		});

		unixClient.connect(&serverPath.front());

		unixClient.write("hello server");
	} catch (std::exception &e) {
		std::cout << "  Something unexpected happened: " << e.what() << '\n';
	}

	return 0;
}