/*
 * Libgopher is a library to create and run a gopher server reducing
 * a gopher application to a few function calls.
 * 
 * All public interfaces to the server are contained in the header
 * file "gopher.h" and remaining _functions in this file should be
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

#define		CONNECTION_QUEUE	10000
#define		BUFSIZE			4096



/* TODO 	special handling for image files?? */

Server server;

		/********************************/
		/* PUBLIC FUNCTIONS FOR LIBRARY */
		/********************************/

int start_server() {

	_open_syslog();

	_load_config(CONFIG_FILE);

	struct addrinfo hints, *res, *p;
	memset( &hints, 0, sizeof(hints) );
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	syslog(LOG_DEBUG, "GOPHERD: Starting gopherd on ip %s on port %s", server.listening, server.port);

	if ( getaddrinfo( NULL, server.port, &hints, &res) != 0) {
		_getaddrinfo_failed();
	}

	// TODO should break this function up to smaller components

	// socket and bind
	for ( p = res; p != NULL; p = p->ai_next ) {
		server.socket = socket( p->ai_family, p->ai_socktype, 0 );

		if ( server.socket == -1 ) 
			continue;

		if ( bind( server.socket, p->ai_addr, p->ai_addrlen) == 0 ) 
			break;
	}

	if ( p == NULL ) {
		_socket_or_bind_failed();
	}

	freeaddrinfo(res);

	// listen for incoming connections
	if ( listen( server.socket, CONNECTION_QUEUE ) != 0 ) {
		_listen_failed();
	}

	// process connections

	int client_socket;

	struct sockaddr address;
	socklen_t addrlen;

	char *buffer = malloc(BUFSIZE);

	while (1) {

		if ((client_socket = accept(server.socket, 
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


		/********************************/
		/* PRIVATE FUNCTIONS IN LIBRARY */
		/********************************/


#define		STATFAIL 	-1

void _process_request(int client_socket, char * buf) {
	assert(client_socket != 0);
	assert(buf != NULL);
	assert(strlen(buf) != 0 );

	char * url = calloc(REQUEST_URL_SIZE+1, sizeof(char));

	syslog(LOG_DEBUG, "_process_request() received %s", buf);

	_parse_url(url, buf);

	syslog(LOG_DEBUG, "Parsed URL = %s", url);

	struct stat sb;
	if (stat( url, &sb) == STATFAIL) {
		_invalid_url(url);
		return;
	}

	syslog(LOG_DEBUG, "Fullpath of requested URL = %s", url);

	_send_document(client_socket, url);
}

char * _propend_gophermap(char * url) {
	assert(url != NULL);
	assert(strlen(url) > 0);

	sprintf(url, "%sgophermap", url);
	return url;
}

void _send_document(int client_socket, char * doc) {
	assert(client_socket != 0);
	assert(doc != NULL);
	assert(strlen(doc) != 0 );

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
		} 

		write(client_socket, &filestr[i], 1);
	}

	write(client_socket, ".", 1);

	munmap(filestr, sb.st_size);

	close(fd);
}

char * _strip_rn(char * url) {
	assert(url != NULL);
	assert(strlen(url)>0);

	int len = strlen(url) -1;	// should point to '\n'

	if (url[len] == '\n') { url[len--] = 0; }	// remove '\n'
	if (url[len] == '\r') { url[len] = 0; }		// remove '\r'

	return url;
}

char _last_char(char * url) {
	assert(url != NULL);
	assert(strlen(url)>0);

	return url[strlen(url)-1];
}

void  _parse_url(char * parsed_url, char * url) {
	assert(strlen(url)>0);
	assert(parsed_url != NULL);

	sprintf(parsed_url, "%s%s", GOPHER_ROOT, _strip_rn(url));

	if (_last_char(parsed_url) == '/') { _propend_gophermap(parsed_url); }
}

void _set_docroot(char *root ) {
	assert(root != NULL);
	assert(strcmp(root,"") != 0);

	bzero(server.docroot, sizeof(server.docroot));
	strcpy(server.docroot, root);
}

void _set_listening(char *ip) { 	// can be "" as this is an override 
	assert(ip != NULL);
	assert(strlen(ip) < 16);

	bzero(server.listening, sizeof(server.listening));
	strcpy(server.listening, ip);
}

void _set_port(char * p) {
	assert(p != NULL);
	assert(atoi(p) > 0);
	assert(atoi(p) <= 65535);

	bzero(server.port, sizeof(server.port));
	strcpy(server.port, p);
}

void _set_hostname(char *newhost ) {
	assert(newhost != NULL);
	assert(strcmp(newhost,"") != 0);

	bzero(server.hostname, sizeof(server.hostname));
	strcpy(server.hostname, newhost);
}


void _write_default_config() {
	FILE * newfp = fopen(CONFIG_FILE, "w");

	fprintf(newfp, "# /etc/gopherd.conf\n");
	fprintf(newfp, "# This is the configuration file for the gopherd server.\n");
	fprintf(newfp, "# \n");
	fprintf(newfp, "# listening=0.0.0.0\n");
	fprintf(newfp, "# port=70\n");
	fprintf(newfp, "# root=/var/gopher\n");

	fclose(newfp);
}


#define		MAX_LINE	80

void _load_config(char * configfile) {
	assert(configfile != NULL);
	assert(strcmp(configfile,"") != 0 );

	char line[MAX_LINE+1];
	char *sep="=\n";
	char *token, *value;

	FILE * fp = fopen(CONFIG_FILE, "r");

	if ( fp == NULL ) {
		_write_default_config();

		_set_listening("0.0.0.0");
		_set_port(DEFAULT_PORT);
		_set_docroot(GOPHER_ROOT);
		return;
	}

	while ( fgets(line, 80, fp) != NULL ) {

		if (line[0] == '#') {
			continue;
		}

		token = strtok(line, sep);
		value = strtok(NULL, sep);

		if (strcmp(token, "listening") == 0) {
			_set_listening(value); continue;
		}

		if (strcmp(token, "port") == 0) {
			_set_port(value); continue;
		}

		if (strcmp(token, "root") == 0) {
			_set_docroot(value); continue;
		}
	}

	fclose(fp);
}




