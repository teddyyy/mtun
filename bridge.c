#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/types.h>         
#include <sys/socket.h>

#include <linux/sockios.h>
#include <linux/if_bridge.h>
#include <net/if.h>

int br_add_interface(const char *bridge, const char *dev)
{
    struct ifreq ifr;
    int err, br_socket_fd;
    int ifindex = if_nametoindex(dev);

    if (ifindex == 0)
        return ENODEV;

	if ((br_socket_fd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "can't create socket\n");
        return errno;
    }

    strncpy(ifr.ifr_name, bridge, IFNAMSIZ);
#ifdef SIOCBRADDIF
    ifr.ifr_ifindex = ifindex;
    err = ioctl(br_socket_fd, SIOCBRADDIF, &ifr);
    if (err < 0)
#endif
    {
        unsigned long args[4] = { BRCTL_ADD_IF, ifindex, 0, 0 };

        ifr.ifr_data = (char *) args;
        err = ioctl(br_socket_fd, SIOCDEVPRIVATE, &ifr);
    }

    return err < 0 ? errno : 0;
}

int add_bridge(const char *brname)
{
	int br_sock_fd;
    int ret = 0;

	if ((br_sock_fd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "can't create socket\n");
        return errno;
    }

#ifdef SIOCBRADDBR
    ret = ioctl(br_sock_fd, SIOCBRADDBR, brname);
    if (ret < 0) {
#endif
        char _br[IFNAMSIZ];
        unsigned long arg[3] = { BRCTL_ADD_BRIDGE, (unsigned long) _br };

        strncpy(_br, brname, IFNAMSIZ);
        ret = ioctl(br_sock_fd, SIOCSIFBR, arg);
    }

    return ret < 0 ? errno : 0;
}
