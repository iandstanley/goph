#ifndef PROTOCOL_H
#define PROTOCOL_H

// Definitions in RFC1436.txt


#define	GOPHER_PORT	"70"	// default port for server
#define SECURE_PORT	"7070"	// TODO find out what other servers use for gophers://

#define DEFAULT_HOST	"localhost"
#define DEFAULT_HOST_URL	"gopher://DEFAULT_HOST:GOPHER_PORT/"

#define MAPNAME		"gophermap"
#define CACHENAME	".cache"

/*
 * Link Types
 */
#define	DOCUMENT	0
#define	DIRECTORY	1
#define	CSOPHONE	2
#define	ERROR		3
#define	BINHEX		4
#define	DOS		5
#define	UUENCODE	6
#define	UNIX		6
#define	SEARCH		7
#define	TELNET		8
#define	BINARY		9
#define	REDUNDANTSERVER	+
#define	TN3270		T
#define	GIF		g
#define	IMAGE		I

#define	CRLF		"\r\n" 	// use CRLF at end of each line returned


/*
 * Error Text
 */
#define	ERROR_BAD_REQUEST	3
#define	ERROR_TEXT	"3\tERROR file or server not found\r\n"


#endif
