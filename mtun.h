#include <netinet/in.h>
#include <net/if.h>
#include <pthread.h>

#define TUNNEL_SETUP_REQ		10
#define TUNNEL_SETUP_ACCEPT		20
#define TUNNEL_SETUP_DENY		30

#define LISTENPORT			55500
#define TUNNEL_BASE_PORT		55550

#define BUFSIZE			2048
#define PEERNUMMAX			16
#define ADDRLEN			16

void do_debug(char *msg, ...);

struct mtun {
	int peer_num;
	int daemon;
	char br_name[IFNAMSIZ];
};

struct peer_data {
	int tap_fd;
	int sock;

	char if_name[IFNAMSIZ];

	int remote_net_fd;
	int local_net_fd;

	struct sockaddr_in remote;
	struct sockaddr_in local;

	pthread_t send;
	pthread_t recv;
};

struct response_body {
	int message_id;
	int tunnel_port;
};
