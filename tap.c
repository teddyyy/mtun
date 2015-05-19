#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/if_tun.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "mtun.h"
#include "socket.h"

static int tap_alloc(char *dev)
{
    struct ifreq ifr;
    int fd, err;
    char *clonedev = "/dev/net/tun";

    if ((fd = open(clonedev, O_RDWR)) < 0) {
        perror("open");
        return fd;
    }

    memset(&ifr, 0, sizeof(ifr));

    ifr.ifr_flags = IFF_TAP|IFF_NO_PI;

    if (*dev)
        strncpy(ifr.ifr_name, dev, IFNAMSIZ);

    if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0) {
        perror("ioctl");
        close (fd);
        return err;
    }

    strcpy(dev, ifr.ifr_name);

    return fd;
}

static int tap_ifup(char *dev)
{
    int sock;
    struct ifreq ifr;

	sock = create_udp_socket();

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, dev, IFNAMSIZ);
    ifr.ifr_flags |= IFF_UP | IFF_RUNNING;

    if(ioctl(sock, SIOCSIFFLAGS, &ifr) < 0) {
        fprintf(stderr, "failed to set device flags\n");
        return -1;
    }

    close(sock);

    return 0;
}

void create_tap_device(struct peer_data *pd, char *addr, int port)
{
	do_debug("addr:%s, port%d\n", addr, port);
	strncpy(pd->if_name, "tap%d", IFNAMSIZ -1);

	pd->tap_fd = tap_alloc(pd->if_name);
	tap_ifup(pd->if_name);

	// for remote
	pd->sock = create_udp_socket();

	pd->remote.sin_family = AF_INET;
    pd->remote.sin_addr.s_addr = inet_addr(addr);
    pd->remote.sin_port = htons(port);

    pd->remote_net_fd = pd->sock;

	// for local
	pd->sock = create_udp_socket();

	pd->local.sin_family = AF_INET;
    pd->local.sin_addr.s_addr = htonl(INADDR_ANY);
    pd->local.sin_port = htons(port);
    if (bind(pd->sock, (struct sockaddr*) &pd->local, sizeof(pd->local)) < 0) {
		perror("bind()");
        exit(1);
    }

    pd->local_net_fd = pd->sock;
}
