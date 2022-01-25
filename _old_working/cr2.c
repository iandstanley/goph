#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>

#define BUFSIZE 4096

int main() {

	int bytes_read;
	char filename[] = "/var/gopher/gophermap";

        char * buffer = calloc(BUFSIZE, sizeof(char));
	char * string;

        FILE *document = fopen(filename, "r");

        if (document ==  NULL) {
                perror("Unable to open document\n");
                return -1;
        }



        while (( bytes_read = fread(buffer, 1, BUFSIZE, document)) > 0) {

                // printf("sending %zu bytes\n", bytes_read);
		while (( string = strsep(&buffer, "\n")) != NULL) {
			printf("%s\r\n", string);
		}

                // write(1, buffer, bytes_read);
        }


        fflush(document);

        fclose(document);

}
