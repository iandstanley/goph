#include	<assert.h> 
#include	<stdio.h>
#include	<stdlib.h>
#include 	<syslog.h>
#include	<unistd.h>

#include	"gopher.h"
#include	"error.h"
#include	"private.h"

/*
 * void _create_server_malloc_failed() {
 * 	syslog(LOG_CRIT, "CRITICAL: Failed to malloc memory for server. Aborting");
 * 	closelog();
 * 	abort();
 * }
 */

void _getaddrinfo_failed() {
	syslog(LOG_ERR, "GOPHERD getaddrinfo() failed. Aborting");
	closelog();
	abort();
}

void _socket_or_bind_failed() {
	syslog(LOG_ERR, "CRITICAL: socket() or bind() failed. Aborting");
	// TODO do i need to close socket here ... probably
	closelog();
	abort();
};

void _listen_failed() {
	syslog(LOG_ERR, "CRITICAL: listen(). Aborting");
	closelog();
	abort();
}

void _accept_failed() {
	syslog(LOG_ERR, "CRITICAL: accept(). Aborting");
	closelog();
	abort();
}

void _invalid_url(char * url) {
	syslog(LOG_DEBUG, "Invalid URL requested: %s", url);
}

void _file_open_error(char * url) {
	syslog(LOG_DEBUG, "Unable to open file");
}

// close_failed ??


		/********************************/
		/* PRIVATE FUNCTIONS IN LIBRARY */
		/********************************/


void _open_syslog() {
	#ifdef NDEBUG
		perror("running with NDEBUG defined");
		setlogmask(LOG_UPTO(LOG_NOTICE));
		openlog("gopherd", LOG_PID | LOG_NDELAY, LOG_LOCAL0);
	#else
		perror("running with DEBUG defined");
		setlogmask(LOG_UPTO(LOG_DEBUG));
		openlog("gopherd", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL0);
	#endif


	syslog(LOG_NOTICE, "Gopherd program started by user %d", getuid() );
}

void _close_syslog() {
	closelog();
}
