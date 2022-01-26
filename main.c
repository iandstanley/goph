
#include	<stdio.h>
#include	<stdlib.h>
#include	<assert.h>

#include 	"gopher.h"


int main(int argc, char **argv) {

/*
	Server *s = create_server();

	configure_server(s, argc, argv);

	start_server(s);
*/

	start_server();

	exit(EXIT_SUCCESS);
}
