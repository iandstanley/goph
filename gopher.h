#pragma once

/*
 * 
 * Header file for libgopher.
 * 
 * Libgopher is a C library and application for running a gopherd
 * server (see RFCs rfc1436.txt, rfc1738.txt, back rfc4266.txt). 
 * 
 * All public interfaces to the library are described in this file.
 * 
 */

#include	"limits.h" 

/* 
	hostname	hostname (currently irrelevant)
			unless we decide on virtual hosts
	port		Port server listening on (default=70)
	listening	IP4 addr server listening on
	socket		socket used for serving
	docroot		document root (default=/var/gopher)
 */
typedef struct {
	char	hostname[_POSIX_HOST_NAME_MAX];
	char	port[6];
	char	listening[16];
	int	socket;
	char	docroot[_POSIX_PATH_MAX];
	int	client_socket; 		// TODO do I need this in the struct??
} Server;

#define		GOPHER_ROOT		"/var/gopher"
#define		REQUEST_URL_SIZE	80
#define		CONFIG_FILE		"/home/ian/goph/gopherd.conf"


/*
 *	PUBLIC function prototypes
 *
 */

// Server * create_server();
// void configure_server(Server *server, int argc, char ** argv);
int start_server();
// int stop_server(Server * server);
