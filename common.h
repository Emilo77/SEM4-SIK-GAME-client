#ifndef MIMUW_SIK_TCP_SOCKETS_COMMON_H
#define MIMUW_SIK_TCP_SOCKETS_COMMON_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <cstdint>
#include <arpa/inet.h>
#include <csignal>
#include "err.h"

#define NO_FLAGS 0

inline static void
install_signal_handler(int signal, void (*handler)(int), int flags) {
	struct sigaction action{};
	sigset_t block_mask;

	sigemptyset(&block_mask);
	action.sa_handler = handler;
	action.sa_mask = block_mask;
	action.sa_flags = flags;

	CHECK_ERRNO(sigaction(signal, &action, nullptr));
}

enum ConnectionType {
	TCP,
	UDP,
};


inline static struct sockaddr_in
get_address(std::string &host, uint16_t port, ConnectionType connection) {
	struct addrinfo hints{};
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET; // IPv4
	switch (connection) {
		case TCP:
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;
			break;
		case UDP:
			hints.ai_socktype = SOCK_DGRAM;
			hints.ai_protocol = IPPROTO_UDP;
			break;
	}

	struct addrinfo *address_result;
	CHECK(getaddrinfo(host.c_str(), nullptr, &hints, &address_result));

	struct sockaddr_in address{};
	address.sin_family = AF_INET; // IPv4, zmienić
	address.sin_addr.s_addr =
			((struct sockaddr_in *) (address_result->ai_addr))->sin_addr.s_addr; // IP address
	address.sin_port = htons(port);

	freeaddrinfo(address_result);

	return address;
}

inline static int open_tcp_socket() {
	int socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socket_fd < 0) {
		PRINT_ERRNO();
	}

	return socket_fd;
}

inline static int open_udp_socket() {
	int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket_fd < 0) {
		PRINT_ERRNO();
	}

	return socket_fd;
}

inline static void
connect_socket(int socket_fd, const struct sockaddr_in *address) {
	int result = connect(socket_fd, (struct sockaddr *) address,
	                     sizeof(*address));
	if (result == -1) {
		std::cerr << "Could not connect to server." << std::endl;
		std::cerr
				<< "Consider checking server status or changing server hostname."
				<< std::endl;
	}
}

static void
send_message_to(int socket_fd, const struct sockaddr_in *client_address,
                const char *message, size_t length) {
	auto address_length = (socklen_t) sizeof(*client_address);
	int flags = 0;
	ssize_t sent_length =
			sendto(socket_fd, message, length, flags,
			       (struct sockaddr *) client_address, address_length);
	ENSURE(sent_length == (ssize_t) length);
}


#endif //MIMUW_SIK_TCP_SOCKETS_COMMON_H
