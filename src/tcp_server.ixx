export module tcp_server;
#include "includes.h"
#include "../logger/log.h"

#include <ctype.h>

export int run() {
	Logger l{};
#ifdef _WIN32 
	WSADATA d{};
	if (WSAStartup(MAKEWORD(2, 2), &d)) {
		l.error("Failed to initialize WSA.\n");
		return -1;
	}

#endif
	l.warn("Configuring local address...\n");
	struct addrinfo hints {};
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo* bind_address{};
	if (getaddrinfo(0, "8080", &hints, &bind_address)) {
		l.error("Unable to get address information\n");
	}
	l.warn("Creating socket...\n");
	SOCKET socket_listen{};
	socket_listen = socket(bind_address->ai_family,
		bind_address->ai_socktype,
		bind_address->ai_protocol);
	if (!ISVALIDSOCKET(socket_listen)) {
		l.error("socket() failed\n");
		fprintf(stderr, "ERROR CODE: %d\n", GETSOCKETERRNO());
		return 1;

	}
	l.warn("Binding socket to local address\n");
	if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen)) {
		l.error("bind() failed\n");
		fprintf(stderr, "ERROR CODE : %d", GETSOCKETERRNO());
		return 1;
	}
	freeaddrinfo(bind_address);

	l.warn("Listening...\n");
	if (listen(socket_listen, 10) < 0) {
		l.error("listen() failed\n");
		fprintf(stderr, "ERROR CODE : %d", GETSOCKETERRNO());
		return 1;
	}
	fd_set master{};
	FD_ZERO(&master);
	FD_SET(socket_listen, &master);
	SOCKET max_socket = socket_listen;
	l.info("Waiting for connections...\n");
	while (1) {
		fd_set reads = master;
		if (select(max_socket + 1, &reads, 0, 0, 0) < 0) {
			l.error("select() failed.\n");
			return 1;

		}
		for (SOCKET i = 1; i < max_socket; i++) {
			if (!FD_ISSET(i, &reads)) continue;
			//handle socket

		}
	}



}