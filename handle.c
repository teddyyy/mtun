#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "mtun.h"
#include "tap.h"
#include "monitor.h"
#include "bridge.h"

void handle_tunnel_setup_request(struct mtun *mt, int sock)
{
	struct sockaddr_in client;
	struct response_body rb;
	struct peer_data *p_pd;
	socklen_t len = sizeof(client);
	int accept_fd;
	char *client_addr;

	if (listen(sock, 5) != 0) {
		perror("listen:");
		exit(1);
	}

	while ((accept_fd = accept(sock, (struct sockaddr*)&client, &len))) {
		if (accept_fd < 0) {
			perror("accept:");
			exit(1);
		}

		memset(&rb, 0, sizeof(rb));

		if (mt->peer_num <= PEERNUMMAX) {
			// reply accept message
			rb.message_id = TUNNEL_SETUP_ACCEPT;	
			rb.tunnel_port = TUNNEL_BASE_PORT + mt->peer_num;
			write(accept_fd, &rb, sizeof(rb));
			do_debug("Reply accept message\n");
		
			client_addr = inet_ntoa(client.sin_addr);

			// initialize peer_data
			if ((p_pd = (struct peer_data*)malloc
						(sizeof(struct peer_data))) == NULL) {
				perror("malloc");
				exit(1);
			}

			memset(p_pd, 0, sizeof(struct peer_data));
			create_tap_device(p_pd, client_addr, 
						TUNNEL_BASE_PORT + mt->peer_num);

			// add bridge interface
			br_add_interface(mt->br_name, p_pd->if_name);

			// create network io process
			pthread_create(&p_pd->send, NULL, monitor_tap2net, (void*)p_pd);
			pthread_create(&p_pd->recv, NULL, monitor_net2tap, (void*)p_pd);

			mt->peer_num++;
		} else {
			// reply deny message
			rb.message_id = TUNNEL_SETUP_DENY;
			rb.tunnel_port = -1;
			write(accept_fd, &rb, sizeof(rb));
			do_debug("Reply deny message\n");
		}
	}

}
