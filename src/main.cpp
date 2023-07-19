#include "includes.h"
#include "../logger/log.h"

#include <string_view>
#include <string>
#define let const auto
auto main(int argc, char* argv[]) -> int {
	if (argc < 3) {
		fprintf(stderr, "Not enough parameters passed");
		fprintf(stderr, "USAGE : %s <hostname> <port>\n", argv[0]);
	}
	let tcpclient = TcpClient{ argv[1],argv[2] };
}


struct TcpClient {
	TcpClient(std::string_view hostname, std::string_view port) {
		Logger l{};
#if defined _WIN32
		WSADATA d{};
		if (WSAStartup(MAKEWORD(2, 2), &d)) {
			l.error("Failed to initialize.\n");
			return ;
		}
#endif

		l.warn("Configuring remote address...\n");
		struct addrinfo hints;
		memset(&hints, 0, sizeof(hints));
		hints.ai_socktype = SOCK_STREAM;
		struct addrinfo* peer_address;
		if (getaddrinfo(hostname.data(), port.data(), &hints, &peer_address)) {
			l.error("getaddrinf() failed!\n");
			printf("\tERROR CODE : %d\n", GETSOCKETERRNO());
			return;
		}
		l.warn("Remote address is: ");
		char address_buf[100];
		char service_buffer[100];
		getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen,
			address_buf, sizeof address_buf, service_buffer, sizeof service_buffer, NI_NUMERICHOST);
		l.warn(address_buf); l.warn(" "); l.warn(service_buffer); l.warn("\n");

		l.warn("Creating socket...\n");
		SOCKET socket_peer = socket(
			peer_address->ai_family,
			peer_address->ai_socktype,
			peer_address->ai_protocol
		);
		if (!ISVALIDSOCKET(socket_peer)) {
			l.error("socket() failed -> "); l.error(std::to_string(GETSOCKETERRNO()).c_str());
			l.error("\n");
			return ;
		}
		l.warn("Connecting...\n");
		if (connect(socket_peer,
			peer_address->ai_addr, peer_address->ai_addrlen)) {
			l.error("connect() failed -> "); l.error(std::to_string(GETSOCKETERRNO()).c_str());
			l.error("\n");
			return;
		}
		freeaddrinfo(peer_address);
		l.success("Connected!\n");
		printf("To send data, enter text followed by enter\n");
		while (true) {
			fd_set reads{};
			FD_ZERO(&reads);
			FD_SET(socket_peer, &reads);
#ifndef _WIN32
			FD_SET(0, &reads);
#endif
			struct timeval timeout {};
			timeout.tv_sec = 0;
			timeout.tv_usec = 100000;

			if (select(socket_peer + 1, &reads, 0, 0, &timeout) < 0) {
				l.error("select() failed -> "); l.error(std::to_string(GETSOCKETERRNO()).c_str());
				l.error("\n");
				return ;
			}
			if (FD_ISSET(socket_peer, &reads)) {
				char read[4096]{};
				int bytes_received = recv(socket_peer, read, 4096, 0);
				if (bytes_received < 1) {
					l.error("Connection closed by peer\n");
					break;
				}
				l.success("Received : "); printf("%.*s", bytes_received, read);
			}
#ifdef _WIN32
			if (_kbhit()) {
#else 
			if (FD_ISSET(0, &reads)) {


#endif
				char read[4096];
				if (!fgets(read, 4096, stdin)) break;
				printf("Sending : %s", read);
				int bytes_sent = send(socket_peer, read, strlen(read), 0);
				printf("Sent %d bytes.\n", bytes_sent);
				l.success("Sending : "); printf("%.*s", bytes_sent, read);

			}

			}


		l.success("Closing socket...\n");
		CLOSESOCKET(socket_peer);

#ifdef _WIN32
		WSACleanup();
#endif
		printf("Finished\n");
		return ;





	}

};
