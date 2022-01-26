/*
 * Libgopher is a library to create and run a gopher server reducing
 * a gopher application to a few function calls.
 * 
 * All public interfaces to the server are contained in the header
 * file "gopher.h" and remaining routines in this file should be
 * avoided.
 * 
 */

#include	<assert.h>
#include	<fcntl.h>
#include	<libgen.h>
#include	<netdb.h>
#include	<stdbool.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<sys/mman.h>
#include	<sys/socket.h>
#include	<sys/stat.h>
#include	<sys/types.h>
#include	<syslog.h>
#include	<unistd.h>

#include	"gopher.h"
#include	"private.h"	
#include	"error.h"	// all errors and logging functions


#define		DEFAULT_PORT		"70"
#define		CONNECTION_QUEUE	10000
#define		BUFSIZE			4096



/* 

	TODO	split up into different functions
	TODO 	structure struct server
	TODO	make certain functions public
	TODO	make other functions private
	TODO 	add tests as we go 
	TODO	move functionality into sub-functions that we can test in test.c

*/

		/********************************/
		/* PUBLIC FUNCTIONS FOR LIBRARY */
		/********************************/


Server * create_server() {
	_open_syslog();

	Server *s = calloc(1,sizeof(Server));
	assert(s != NULL);

	if (s == NULL) {
		_create_server_malloc_failed();
	}

	strcpy(s->hostname, "localhost");
	strcpy(s->listening, "0.0.0.0");
	strcpy(s->port, DEFAULT_PORT);
	s->socket = -2;		// initialize to impossible RC code
	strcpy(s->docroot, "/var/gopher");
	return s;

}



void configure_server(Server *server, int argc, char ** argv) {
	int sw;

	while ((sw = getopt(argc, argv, "p:n:l:r:h") ) != -1) {
		switch (sw) {
		case 'p':
			_set_port(server, optarg);	
			break;
		case 'n':
			_set_hostname(server, optarg);
			break;
		case 'l':
			_set_listening(server, optarg);
			break;
		case 'r':
			_set_docroot(server, optarg);
			break;
		case 'h':
			_usage();
			exit(EXIT_SUCCESS);
		default:
			_usage();
			exit(EXIT_FAILURE);
		}
	}
}


int start_server(Server * server) {

	struct addrinfo hints, *res, *p;
	memset( &hints, 0, sizeof(hints) );
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	syslog(LOG_DEBUG, "GOPHERD: Starting gopherd on ip %s on port %s", server->listening, server->port);

	if ( getaddrinfo( NULL, server->port, &hints, &res) != 0) {
		_getaddrinfo_failed();
	}

	// TODO should break this function up to smaller components

	// socket and bind
	for ( p = res; p != NULL; p = p->ai_next ) {
		server->socket = socket( p->ai_family, p->ai_socktype, 0 );

		if ( server->socket == -1 ) 
			continue;

		if ( bind( server->socket, p->ai_addr, p->ai_addrlen) == 0 ) 
			break;
	}

	if ( p == NULL ) {
		_socket_or_bind_failed();
	}

	freeaddrinfo(res);

	// listen for incoming connections
	if ( listen( server->socket, CONNECTION_QUEUE ) != 0 ) {
		_listen_failed();
	}

	// process connections

	int client_socket;

	struct sockaddr address;
	socklen_t addrlen;
	// int BUFSIZE = 1024;
	char *buffer = malloc(BUFSIZE);


	while (1) {

		if ((client_socket = accept(server->socket, 
			(struct sockaddr *) &address, &addrlen)) <= 0) {
			_accept_failed();
		}

		if (client_socket > 0) {
			syslog(LOG_DEBUG, "GOPHERD: Client connected to socket %d", client_socket);
		}

		memset(buffer, 0, BUFSIZE);

		recv(client_socket, buffer, BUFSIZE, 0);

		syslog(LOG_DEBUG, "Gopher URL requested: %s", buffer);

		_process_request(client_socket, buffer);

		close(client_socket);

	}
}

int stop_server(Server * server) {
	
	return 0;

}


		/********************************/
		/* PRIVATE FUNCTIONS IN LIBRARY */
		/********************************/

void _server_status(Server * server) {

}

#define		STATFAIL 	-1

void _process_request(int client_socket, char * buf) {

	char * url = calloc(REQUEST_URL_SIZE+1, sizeof(char));
	//char *fullpath = calloc(REQUEST_URL_SIZE+1, sizeof(char));
	char * documentname;	// full path of document OR 'fullpath'/gophermap if DIR

	syslog(LOG_DEBUG, "_process_request() received %s", buf);

	_parse_url(url, buf);

	syslog(LOG_DEBUG, "Parsed URL = %s", url);

	struct stat sb;

	if (stat( url, &sb) == STATFAIL) {
		_invalid_url(url);
		return;
	}

	if (S_ISDIR(sb.st_mode)) {
		// requested directory

		syslog(LOG_DEBUG, "Requested %s is a directory, appending /gophermap", url);
		printf("Requested %s is a directory\n", url);

		documentname = strcat(url, "gophermap");
	} else {
		documentname = url;
	}

	syslog(LOG_DEBUG, "Fullpath of requested URL = %s", url);

	_send_document(client_socket, documentname);
}

/*
//char * _full_url(char * url) {
void _full_url(char * fullpath, char * url) {

	//char fullpath[REQUEST_URL_SIZE+1];

	sprintf(fullpath, "%s%s", GOPHER_ROOT, url);

	//return fullpath;
}
*/

char * _propend_gophermap(char * url) {

	sprintf(url, "%sgophermap", url);
	return url;
}

char * _validate_document(char * doc) {

	char * validated_doc;
	char * fullpath = NULL;


	struct stat sb;

	if (stat( fullpath, &sb) == STATFAIL) {
		_invalid_url(fullpath);
		return "";
	}

	if (S_ISDIR(sb.st_mode)) {
		// requested directory

		syslog(LOG_DEBUG, "Requested %s is a directory, appending /gophermap", fullpath);
		printf("Requested %s is a directory\n", fullpath);

		validated_doc = strcat(fullpath, "gophermap");
	} else {
		validated_doc = fullpath;
	}




	return validated_doc;
}

void _send_document(int client_socket, char * doc) {

	syslog(LOG_DEBUG, "Sending %s to client", doc);

	int fd = open(doc, O_RDONLY, S_IRUSR | S_IWUSR);

	if (fd == -1) {
		_file_open_error(doc);
		return;
	}

	struct stat sb;

	fstat(fd, &sb);

	char * filestr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

	for (int i=0; i < sb.st_size; i++) {
	
		if (filestr[i] == '\n') {
			// shoudlreally only do this with text documents
			// TODO fix for other types??

			write(client_socket, "\r", 1);
			// TODO what to do with client sockets??
		} 

		write(client_socket, &filestr[i], 1);
	}

	write(client_socket, ".", 1);

	munmap(filestr, sb.st_size);

	close(fd);
}

char * _strip_rn(char * url) {

	int len = strlen(url) -1;	// should point to '\n'

	if (url[len] == '\n') { url[len--] = 0; }	// remove '\n'
	if (url[len] == '\r') { url[len] = 0; }		// remove '\r'

	return url;
}

char _last_char_of_url(char * url) {
	return url[strlen(url)-1];
}

void  _parse_url(char * parsed_url, char * url) {

	sprintf(parsed_url, "%s%s", GOPHER_ROOT, _strip_rn(url));
	//sprintf(url, "%s%s", GOPHER_ROOT, _strip_rn(url));

	//if (_last_char_of_url(url) == '/') {
	if (_last_char_of_url(parsed_url) == '/') {
		//_propend_gophermap(url);
		_propend_gophermap(parsed_url);
	}
	
	//return url;
	//return parsed_url;
}

void _usage() {
	fprintf(stderr, "Goph gopher server\n");
	fprintf(stderr, "-h             help\n");
	fprintf(stderr, "-n host        name of server\n");
	fprintf(stderr, "-p port        alternate port to server on\n");
	fprintf(stderr, "-l ipaddr      IP address to listen on\n");
	fprintf(stderr, "-r dir         directory to use as root document store\n");
	fprintf(stderr, "\n");
}

void _set_docroot(Server * s, char *root ) {
	assert(strcmp(root,"") != 0);

	bzero(s->docroot, sizeof(s->docroot));
	strcpy(s->docroot, root);
}
void _set_listening(Server * s, char *ip) {
	assert(strcmp(ip,"") != 0);

	bzero(s->listening, sizeof(s->listening));
	strcpy(s->listening, ip);
}
void _set_port(Server * s, char * p) {
	assert(p != 0);

	strcpy(s->port, p);
}
void _set_hostname(Server * s, char *newhost ) {
	assert(strcmp(newhost,"") != 0);

	bzero(s->hostname, sizeof(s->hostname));
	strcpy(s->hostname, newhost);
}

