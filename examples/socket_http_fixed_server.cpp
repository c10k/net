#include "socket.hpp"
#include <iostream>

using namespace net;

int main()
{
    try {
        Socket s(Domain::IPv4, Type::TCP);
        s.start("127.0.0.1", 8000);

        std::string response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: text/plain\r\n";
        response += "Content-Length: 11\r\n";
        response += "Connection: close\r\n";
        response += "\r\nHello World";

        while (1) {
            const auto peer = s.accept();
            peer.send(response);
        }
    } catch (std::exception &e) {
        std::cerr << e.what() << '\n';
    }
}
