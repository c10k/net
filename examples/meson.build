progs = [['socket_tcp_server', ['socket_tcp_server.cpp']],
		['socket_tcp_client', ['socket_tcp_client.cpp']],
		['socket_unix_udp_server',['socket_unix_udp_server.cpp']],
		['socket_unix_udp_client',['socket_unix_udp_client.cpp']],
		['socket_tcp_mt_server', ['socket_tcp_mt_server.cpp']],
		['socket_tcp_mt_client', ['socket_tcp_mt_client.cpp']],
		['socket_tcp_fork_server', ['socket_tcp_fork_server.cpp']],
		['socket_http_fixed_server', ['socket_http_fixed_server.cpp']]]

foreach p : progs
  executable(p[0], p[1], include_directories : inc,
  			link_with : netlib, dependencies: [thread_dep])
endforeach
