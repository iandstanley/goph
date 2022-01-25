#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>

#include "protocol.h"
#include "server.h"
#include "main.h"


void usage()
{
	fprintf(stderr, "Goph gopher server\n");
	fprintf(stderr, "-h             help\n");
	fprintf(stderr, "-n host        name of server\n");
	fprintf(stderr, "-p port        alternate port to server on\n");
	fprintf(stderr, "-l ipaddr      IP address to listen on\n");
	fprintf(stderr, "-r dir         directory to use as root document store\n");
	fprintf(stderr, "\n");
}



/*
** GLOBAL VARIABLES
*/
int clients[MAXCLIENTS];


int main(int argc, char *argv[])
{
	int		sw;	/* used for command line options processing */

	Server	* server;
	clientdata	client;


	server = calloc(1, sizeof(Server));

	getServerDefaults(server);

	while ((sw = getopt(argc, argv, "p:n:l:r:") ) != -1) {
		switch (sw) {
		case 'p':
			strcpy(server->port, optarg);
			break;
		case 'n':
			strcpy(server->hostname, optarg);
			break;
		case 'l':
			strcpy(server->listening, optarg);
			break;
		case 'r':
			strcpy(server->root, optarg);
			break;
		default:
			usage();
			exit(FAIL);
		}
	}

	//server_status(&server);


	/* start server */
	notify_server_starting(server);

	start_server(server);

	
	return SUCCESS;
}

