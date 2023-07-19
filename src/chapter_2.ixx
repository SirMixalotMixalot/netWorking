module;


#include <stdio.h>
#include <time.h>
#include <iostream>
#include <chrono>
#include <array>
#include "includes.h"

#include "../logger/log.h"




export module chapter_2;
export namespace Chapter2 {


	struct DateTime {
		std::time_t timer{};
		std::array<char, 26> buffer{};

		DateTime() {
			this->fill_time();
		}

		friend std::ostream& operator<<(std::ostream& out, DateTime const& dt);
		void fill_time() {
			time(&this->timer);

			ctime_s(this->buffer.data(), this->buffer.size(), &this->timer);

		}
		std::tm get_time() {
			if (!this->timer) {
				fill_time();
			}
			std::tm t{};
			gmtime_s(&t, &this->timer);
			return t;
		}
	};

	auto run() -> int {
		
		Logger l{};


#if defined(_WIN32)
		WSADATA d{};
		if (WSAStartup(MAKEWORD(2, 2), &d)) {
			l.error("Failed to initialize\n");
			return 1;
		}
#endif // _WIN32

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
		
		int option = 0;
		if (setsockopt(socket_listen, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&option, sizeof(option))) {
			l.error("setsockopt() failed\n");
			return 1;
		}
		l.warn("Binding socket to local address...\n");
		if (bind(socket_listen,
			bind_address->ai_addr, bind_address->ai_addrlen)) {
			l.error("bind() failed.\n");
			return 1;
		}

		
		
		freeaddrinfo(bind_address);

		
	
		if (listen(socket_listen, 10) < 0) {
			l.error("listen() failed.\n");
			return 1;
		}
		l.success("Listening...\n");

		l.warn("Waiting for a connection...\n");
		struct sockaddr_storage client_addr;
		socklen_t client_len = sizeof(client_addr);
		auto socket_client = accept(socket_listen, //blocking
			(struct sockaddr*)&client_addr, &client_len);
		if (!ISVALIDSOCKET(socket_client)) {
			l.error("accept() failed\n");
			fprintf(stderr, "ERROR CODE: %d\n", GETSOCKETERRNO());
			return 1;

		}
		l.success("Client is connected... ");
		char address_buffer[100]{};
		getnameinfo(
			(struct sockaddr*)&client_addr,
			client_len, address_buffer, sizeof(address_buffer), 0, 0,
			NI_NUMERICHOST
		);
		printf("%s\n", address_buffer);
		l.warn("Reading request...\n");
		char request[1024];
		int bytes_received = recv(socket_client, request, 1024, 0); //blocking
		printf("Received %d bytes.\n", bytes_received);

		printf("%.*s", bytes_received, request);

		l.warn("Sending response...\n");
		const char* response = "HTTP/1.1 200 OK\r\n"
			"Connection: close\r\n"
			"Content-Type: text/plain; charset=utf-8\r\n\r\n"
			"Local time is: ";
		int bytes_sent = send(socket_client, response, strlen(response), 0);
		printf("Sent %d of %d bytes\n", bytes_sent, (int)strlen(response));

		DateTime dt{};
		bytes_sent = send(socket_client, dt.buffer.data(), strlen(dt.buffer.data()), 0);
		printf("Sent %d of %d bytes\n", bytes_sent, (int)strlen(dt.buffer.data()));

		l.warn("Closing connection...\n");
		CLOSESOCKET(socket_client);

#if defined(_WIN32)
		WSACleanup();
#endif

		l.success("Finished successfully \n");
		std::cin.get();
		return 0;


	}
	std::ostream& operator<<(std::ostream& out, DateTime const& dt)
	{
		return out << dt.buffer.data();
	}
}
