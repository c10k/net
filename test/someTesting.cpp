#include <iostream>
#include <sys/socket.h>
#include <cstring>

int main()
{
	for (int i = 0; i < 255; i++) {
		std::cout << "i:: " << i;
		auto res = socket(AF_UNIX, SOCK_DGRAM, 0);
		if (res < 0) {
			std::cout << "  Error in creating socket: " << strerror(errno);
		}
		std::cout << '\n';
	}
	return 0;
}