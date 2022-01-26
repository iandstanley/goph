#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<assert.h>
#include	<unistd.h>
#include	<fcntl.h>

#include	"gopher.h"

#define		CONFIG_FILE	"/home/ian/goph/gopherd.conf"
#define		MAX_LINE	80

Server server;

void load_config(char * configfile) {

	char line[MAX_LINE+1];
	char *sep="=\n";
	char *token, *value;

	FILE * fd = fopen(CONFIG_FILE, "r");

	while ( fgets(line, 80, fd) != NULL ) {

		if (line[0] == '#') {
			continue;
		}

		token = strtok(line, sep);
		value = strtok(NULL, sep);

		if (strcmp(token, "listening") == 0) {
			strcpy(server.listening, value);
			//memcpy(server.listening, value, sizeof(value+1));
			printf("listening on %s\n", server.listening);
			continue;
		}

		if (strcmp(token, "port") == 0) {
			strcpy(server.port, value);
			printf("port on %s\n", server.port);
			continue;
		}

		if (strcmp(token, "root") == 0) {
			strcpy(server.docroot, value);
			printf("root on %s\n", server.docroot);
			continue;
		}

	}


	fclose(fd);
}

void create_config() {

	FILE * fd = fopen(CONFIG_FILE, "w");

	fprintf(fd, "# /etc/gopherd.conf\n");
	fprintf(fd, "# This is the configuration file for the gopherd server.\n");
	fprintf(fd, "# \n");
	fprintf(fd, "# listening=0.0.0.0\n");
	fprintf(fd, "# port=70\n");
	fprintf(fd, "# root=/var/gopherd/\n");

	fclose(fd);
}

int main() {

	load_config(CONFIG_FILE);

	assert(strncmp(server.listening, "127.0.0.1", strlen("127.0.0.1")) == 0);
	assert(strncmp(server.port, "7070", strlen("7070")) == 0);
	assert(strncmp(server.docroot, "/home/ian/goph/", strlen("/home/ian/goph")) == 0);

}


