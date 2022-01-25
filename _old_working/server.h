#include <limits.h>

#ifndef SERVER_H
#define SERVER_H

#include "main.h"

/*
 * Data about the server's settings
 */
typedef struct {
	char		hostname[_POSIX_HOST_NAME_MAX];
	char		port[6];	/* port listening on */
	char		listening[16];	/* IP address */
	int		fd;	/* socket file descriptor */
	int		client_sock;	/* socket file descriptor */
	char		root[_POSIX_PATH_MAX];	/* document root */

	int		session_timeout;
	int		session_max_kbytes;
	int		session_max_hits;
	int		session_id;

	char		page_header[81];
	char		page_footer[81];
} Server;

//typedef struct serverdata 	Server;

#define CONNECTION_QUEUE	10000
#define STATFAIL		-1
#define REQUEST_URL_SIZE        80
#define BUFSIZE		4096


void getServerDefaults(Server *server);

void server_status(Server *server);

void notify_server_starting(Server * s);

void start_server(Server * s);

void process_request(Server * server, char *buf);

char *  parse_url(char * url);


#endif

