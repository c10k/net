progs = [['socket_tcp_server', ['socket_tcp_server.cpp']],
		['socket_tcp_client', ['socket_tcp_client.cpp']]]

foreach p : progs
  executable(p[0], p[1], include_directories : inc, link_with : netlib)
endforeach
