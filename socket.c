#include <stdio.h>
#include <errno.h>
#include <sys/types.h> 
#include <sys/socket.h>

#include "mtun.h"

int create_tcp_socket()
{
	int sock;
	struct sockaddr_in addr;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("socket:");
		return -1;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(LISTENPORT);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
		perror("bind:");
		return -1;
	}

	return sock;
}

int create_udp_socket()
{
	int sock;

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		perror("socket");
		return -1;
	}

	return sock;
}

