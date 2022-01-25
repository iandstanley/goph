#ifndef CLIENT_H
#define CLIENT_H

/*
 * data on the client We need to hold the URL requested somewhere and also
 * need the file descriptor for the socket
 */
#define REQUEST_URL_SIZE	80

typedef struct {
	char		request_url[REQUEST_URL_SIZE];
	int		fd;	/* socket file descriptor */
} clientdata;

#endif

