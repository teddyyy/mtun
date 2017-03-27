#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <pthread.h>

#include "mtun.h"
#include "tap.h"
#include "bridge.h"
#include "socket.h"
#include "handle.h"

int debug = 0;

void do_debug(char *msg, ...) {
    va_list argp;

    if (debug) {
        va_start(argp, msg);
        vfprintf(stderr, msg, argp);
        va_end(argp);
    }
}

static void init_mtun_data(struct mtun *p) {
	p->peer_num = 0;
	p->daemon = 0;
	memset(p->br_name, '\0', IFNAMSIZ - 1);
}

static void usage(char *progname) {
	fprintf(stderr, "Usage:\n");
  	fprintf(stderr, "%s -i <bridge name> [-d]\n", progname);
  	fprintf(stderr, "%s -h\n", progname);
  	fprintf(stderr, "\n");
  	fprintf(stderr, "-i <bridge name>: Name of interface to use (mandatory)\n");
  	fprintf(stderr, "-v: outputs debug information while running\n");
  	fprintf(stderr, "-d: running process as daemon\n");
  	fprintf(stderr, "-h: prints this help text\n");
 	exit(1);
}

int main(int argc, char *argv[])
{
	char *progname;
	int option;
	struct mtun mt;
	int sock_fd;

	progname = argv[0];

	// initialize mtun data
	init_mtun_data(&mt);

	while ((option = getopt(argc, argv, "i:hvd")) > 0) {
		switch(option) {
			case 'v':
				debug = 1;
				break;
			case 'h':
				usage(progname);
				break;
			case 'i':
				strncpy(mt.br_name, optarg, IFNAMSIZ - 1);
				break;
			case 'd':
				mt.daemon = 1;
				break;
			default:
				fprintf(stderr, "Unknown option %c\n", option);
				usage(progname);
			}
	}

	argv += optind;
	argc -= optind;

	if (argc > 0) {
		fprintf(stderr, "Too many options!\n");
		printf("\n");
		usage(progname);
    }

	if (mt.br_name[0] == '\0') {
		fprintf(stderr, "Must specify bridge interface name\n");
		printf("\n");
		usage(progname);
    }

	// create socket for listening request message
	sock_fd = create_tcp_socket();
	if (sock_fd < 0) {
		fprintf(stderr, "Can't create tcp socket\n");
		return -1;
	}

	do_debug("create socket for listen\n");

	// deamonize if daemon flag
	if (mt.daemon) {
		if ((daemon(1,1)) != 0) {
			perror("daaemon: ");
			exit(1);
		}
    }

	// create bridge interface and interface up
	add_bridge(mt.br_name);
	tap_ifup(mt.br_name);

	// main loop
	handle_tunnel_setup_request(&mt, sock_fd);

	return 0;
}
