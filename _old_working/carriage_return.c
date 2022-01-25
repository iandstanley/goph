#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>

int main() {

	char filename[] = "/var/gopher/gophermap";

	int fd = open(filename,  O_RDONLY, S_IRUSR | S_IWUSR);

	struct stat sb;

	fstat(fd, &sb);	

	char * filestr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

	for (int i = 0 ; i < sb.st_size ; i++) {

		if  ( filestr[i] == '\n' ) {
			printf("\r");
		}
                printf("%c", filestr[i]);
        }

	munmap(filestr, sb.st_size);
	close(fd);
}
