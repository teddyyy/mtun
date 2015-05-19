#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "mtun.h"

void* monitor_tap2net (void* fd)
{
	int n = 0;
    char buffer[BUFSIZE];
    unsigned long int tap2net = 0;
    struct peer_data *fds = fd;
	
	do_debug("monitor_tap2net: process generated\n");

    while (1) {
		n = read(fds->tap_fd, buffer, BUFSIZE);
        tap2net++;

		do_debug("TAP2NET %lu: Read %d bytes from the tap interface\n", tap2net, n);

        n = sendto(fds->remote_net_fd, buffer, n, 0,
				(struct sockaddr *)&fds->remote, sizeof(fds->remote));
		do_debug("TAP2NET %lu: Written %d bytes to the network\n", tap2net, n);
    }
}

void* monitor_net2tap (void* fd)
{
	int n = 0;
    char buffer[BUFSIZE];
    unsigned long int net2tap = 0;
    struct peer_data *fds = fd;
    socklen_t sin_size;
	
	do_debug("monitor_net2tap: process generated\n");

    while (1) {
		n = recvfrom(fds->local_net_fd, buffer, sizeof(buffer), 0,
					 (struct sockaddr*)&fds->local, &sin_size);
        net2tap++;
		do_debug("NET2TAP %lu: Read %d bytes from the network\n", net2tap, n);

        n = write(fds->tap_fd, buffer, n);
		do_debug("NET2TAP %lu: Written %d bytes to the tap interface\n", net2tap, n);
	}
}
