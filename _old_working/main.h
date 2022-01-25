#ifndef MAIN_H
#define MAIN_H

#include <sys/param.h>

#include "protocol.h"		// defines from the RFCs
#include "server.h"
#include "client.h"

/* App defaults */
#define	SUCCESS	0
#define FAIL 1

#define GOPHER_ROOT	"/var/gopher"	// server's documents are hosted here

#define INDEX		gophermap	// first choice if a directory is requested
#define CACHE		.cache		// next choice if 'gophermap' does not exist

#define SERVER_SOFTWARE goph

#define GITHUB_PAGE	"https://github.com/iandstanley/goph"
#define WEBPAGE		"https://github.com/iandstanley/goph"

#define DEFAULT_PAGE_HEADER	"%s"
#define DEFAULT_PAGE_FOOTER	"Powered by Goph server\t\tSee GITHUB_PAGE for details"


#define DEFAULT_IP	"0.0.0.0"

/*
 * Various TCP/IP settings
 */
#define DEFAULT_SESSION_TIMEOUT        1800
#define DEFAULT_SESSION_MAX_KBYTES    4194304
#define DEFAULT_SESSION_MAX_HITS    4096
#define MAXCLIENTS	1000

#endif
