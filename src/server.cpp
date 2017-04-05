#include "server.hpp"

namespace tcp {

void server::processUsingPoll()
{
	// collection of all poll fd structures and manage them using poll.

	std::vector<pollfd> connectedSockets;  // this vector size should be
	// no maximum than
	// RLIMIT_NOFILE

	pollfd listenSockObj;
	listenSockObj.fd = Socket::getSocket();
	listenSockObj.events
	  = static_cast<int>(events::IN | events::ERR | events::INVAL);

	connectedSockets.push_back(listenSockObj);

	for (;;) {

		// TODO: handle the case where the listening socket is closed and then
		// we have to close all the remaining connected sockets.

		if (connectedSockets.size() == 0) break;

		auto res = poll(connectedSockets.data(), connectedSockets.size(), -1);

		if (res == -1) {
			auto currErrNo = errno;
			throw std::runtime_error(methods::getErrorMsg(currErrNo));
		}

		for (auto &eachConnectedSocket : connectedSockets) {
			if (!(eachConnectedSocket.revents & events::INVAL)) {
				if (eachConnectedSocket.fd == Socket::getSocket()) {
					// accept connections here and push_back them to
					// connectedSockets vector with their events set for
					// input, error, hangup.
					auto peer = Socket::accept();
					pollfd peerObject;
					peerObject.fd = peer.getSocket();
					peerObject.events
					  = static_cast<int>(events::IN | events::ERR
					                     | events::HANGUP | events::INVAL);

					connectedSockets.push_back(peerObject);

				} else {
					// check the current socket's revent for input,
					// error, hangup and call functions accordingly.
					// resetting the listening socket with the current fd and
					// other things as needed.
					sockaddr_storage s;
					sockaddr_storage *ptr = &s;
					socklen_t size        = sizeof(s);
					auto res = ::getpeername(eachConnectedSocket.fd,
					                         (sockaddr *) &s, &size);
					if (res == -1) {
						auto currErrNo = errno;
						throw std::runtime_error(
						  methods::getErrorMsg(currErrNo));
					}

					if (eachConnectedSocket.revents & events::IN) {
						OnRequest(this->reset(eachConnectedSocket.fd,
						                      Socket::getDomain(), Type::TCP,
						                      ptr));
					} else if (eachConnectedSocket.revents & events::ERR) {
						OnError(this->reset(eachConnectedSocket.fd,
						                    Socket::getDomain(), Type::TCP,
						                    ptr));
					} else if (eachConnectedSocket.revents & events::HANGUP) {
						OnClose();
					}
				}
			} else {
				// if socket has been closed then remove it from vector
				// outside this loop, after noting it's index here OR
				// instead of that, make the fd member -1 and when
				// connectedSockets
				// size(capacity) reaches RLIMIT_NOFILE then only traverse
				// connectedSockets
				// and remove all those objects where fd is -1.
				// (applied second approach here)
				eachConnectedSocket.fd = -1;
				// TODO: erase invalid sockets from connected sockets here only.
				// connectedSockets.erase();
			}
		}

		rlimit limits;
		auto anotherRes = getrlimit(RLIMIT_NOFILE, &limits);
		if (anotherRes != -1) {
			if (limits.rlim_cur == connectedSockets.size()) {
				connectedSockets.erase(std::remove_if(connectedSockets.begin(),
				                                      connectedSockets.end(),
				                                      [](const pollfd &conn) {
					                                      return conn.fd == -1;
					                                  }),
				                       connectedSockets.end());
			}
		} else {
			auto currErrNo = errno;
			throw std::runtime_error(methods::getErrorMsg(currErrNo));
		}
	}
}

void server::run(expo policy)
{
	try {

		Socket::start(listenIp.c_str(), listenPort, 5);
		if (policy == expo::EP) {
			processUsingPoll();
		}

	} catch (...) {
		throw;
	}
}
}