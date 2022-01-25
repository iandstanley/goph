#include <fcntl.h>
#include <libgen.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


#include "server.h"


void getServerDefaults(Server *server)
{

	strcpy(server->hostname, "localhost");
	strcpy(server->listening, DEFAULT_IP);
	strcpy(server->root, GOPHER_ROOT);
	strcpy(server->port, GOPHER_PORT);

	server->session_timeout = DEFAULT_SESSION_TIMEOUT;
	server->session_max_kbytes = DEFAULT_SESSION_MAX_KBYTES;
	server->session_max_hits = DEFAULT_SESSION_MAX_HITS;
	server->session_id = 0;

	strcpy(server->page_header, DEFAULT_PAGE_HEADER);
	strcpy(server->page_footer, DEFAULT_PAGE_FOOTER);
}


void server_status(Server *server)
{
	printf("hostname is %s\n", server->hostname);
	printf("Listening on %s on port %s\n", server->listening, server->port);
	printf("Documents stored in %s\n", server->root);
	printf("session timeout = %d\n", server->session_timeout);
	printf("max kbytes = %d\n", server->session_max_kbytes);
	printf("max hits = %d\n", server->session_max_hits);
	printf("session id = %d\n", server->session_id);
	printf("page_header =  %s\n", server->page_header);
	printf("page footer  = %s\n", server->page_footer);
}



void notify_server_starting(Server * s)
{
	fprintf(stderr, "Gopherd server '%s' starting on %s:%s serving %s\n", s->hostname, s->listening, s->port, s->root);
}


void start_server(Server * server)
{

	struct addrinfo hints, *res, *p;
	struct sockaddr address;    
	socklen_t addrlen;    
	int sock;
	int bufsize = 1024;    
	char *buffer = malloc(bufsize);

	// getaddrinfo for host
	memset (&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if (getaddrinfo( NULL, server->port, &hints, &res) != 0)
	{
		perror ("getaddrinfo() error");
		exit(1);
	}

	// socket and bind
	for (p = res; p!=NULL; p=p->ai_next)
	{
		server->fd = socket(p->ai_family, p->ai_socktype, 0);
		if (server->fd == -1) continue;
		if (bind(server->fd, p->ai_addr, p->ai_addrlen) == 0) break;
	}
	if (p==NULL)
	{
		perror ("socket() or bind()");
		exit(1);
	}

	freeaddrinfo(res);

	// listen for incoming connections
	if ( listen (server->fd, CONNECTION_QUEUE) != 0 )
	{
		perror("listen() error");
		exit(1);
	}

	if ((server->client_sock = accept(server->fd, (struct sockaddr *) &address, &addrlen)) < 0) {    
	 perror("server: accept");    
	 exit(1);    
	}    
    
	if (server->client_sock > 0){    
		printf("The Client is connected...\n");
	}
        
	recv(server->client_sock, buffer, bufsize, 0);    
	printf("Gopher url requested = _%s_\n", buffer);    

	process_request(server, buffer);

	//write(server->client_sock, "hello world\n", 12);    
	close(server->client_sock); 
}


char *  parse_url(char *url) {

        char * path;

	url[strlen(url)-2] = 0; 	// remove \r\n from url



	// fixup for gopher client
	return url;

        //printf("tokenize(%s)\n", url);

        strtok(url, "/");       // get rid of 'gopher:'

        strtok(NULL , "/");     // get rid of '<hostname>/'

        path = strtok(NULL , "/");

	/* need to drop \r\n from path */
	//path[strlen(path)-2] = 0;

	if (strlen(path) == 0) {
		path = "gophermap";
	}

	return path;
}


/*
 * 
 * void send_document(Server * server, char * documentname) {
 * 
 * 	int fd = open(documentname,  O_RDONLY, S_IRUSR | S_IWUSR);
 * 
 * 	if (fd ==  -1) {
 * 		perror("Unable to open document\n");
 * 		return;
 * 	}
 * 
 * 
 *         struct stat sb;
 * 
 *         fstat(fd, &sb);
 * 
 *         char * filestr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
 * 
 *         for (int i = 0 ; i < sb.st_size ; i++) {
 * 
 *                 if  ( filestr[i] == '\n' ) {
 *                         write(server->client_sock, "\r", 1);
 *                 }
 * 		write(server->client_sock, filestr[i], 1);
 *         }
 * 
 *         munmap(filestr, sb.st_size);
 *         close(fd);
 * 
 * }
 * 
 */


void send_document(Server * server, char * documentname) {

	int bytes_read;
	char * buffer = calloc(BUFSIZE, sizeof(char));

	FILE *document = fopen(documentname, "r");

	if (document ==  NULL) {
		perror("Unable to open document\n");
		return;
	} 

	printf("document %s exists", documentname);


        while (( bytes_read = fread(buffer, 1, BUFSIZE, document)) > 0) {

                // printf("sending %zu bytes\n", bytes_read);


		for (int i = 0 ; i < bytes_read ; i++) {
		
			if (buffer[i] == '\n') {
				write(server->client_sock, "\r", 1);
			}

			write(server->client_sock, buffer[i], 1);

		}		

        }


	fflush(document);

	fclose(document);

}


void process_request(Server * server, char *buf) 
{
	char * url;
	char *fullpath = calloc(REQUEST_URL_SIZE +1 , sizeof(char));
	char * documentname;	// fullpath of document or 'fullpath'/gophermap if a DIR

	printf("DEBUG: buf= %s\n", buf);

	url = parse_url(buf);
	printf("DEBUG: url= _%s_ \t buf= _%s_\n", url, buf);

/* 	char * filename = malloc(_POSIX_PATH_MAX * sizeof(char)); */
/*  */
/* 	memset(filename, 0, _POSIX_PATH_MAX); */
/*  */
/* 	filename = strncat(server->root, url, _POSIX_PATH_MAX); */
/*  */
/* 	printf("DEBUG: filename= %s\n", filename); */

        sprintf(fullpath, "%s%s", GOPHER_ROOT, url);


	struct stat sb;

	if (stat(fullpath, &sb) == STATFAIL) {
		perror("File doesn't exist");
		return;
	}
	
	if (S_ISDIR(sb.st_mode) ) {
		// requested directory

		printf("Requested _%s_ is a directory\n", fullpath);

		documentname = strcat(fullpath, "gophermap");
		printf("document now _%s_\n", documentname);
	} else {
		documentname = fullpath;
	}

	send_document(server, documentname);

}


