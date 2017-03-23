#include "socket.hpp"
#include <iostream>

using namespace net;

int main()
{
	try {
		Socket unixServer(Domain::UNIX, Type::UDP);
		std::string serverPath("/tmp/unixServer");

		unixServer.bind([&](AddrUnix &s) {
			return methods::construct(s, serverPath.c_str());
		});

		const auto res = unixServer.read(12);

		std::cout << "Some client sent: " << res << '\n';

	} catch (std::exception &e) {
		std::cout << "  Something unexpected happened: " << e.what() << '\n';
	}

	return 0;
}