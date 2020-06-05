#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* TODO */
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>



char * 
read_exec (char * exe)
{
	/* TODO*/
	execl(exe, "");
	char * ret = malloc(sizeof(char) * 2048);
	int fd = open("hello.txt", O_WRONLY | O_CREAT, 0644);
	dup2(fd, 1);
	close(fd);
	return ret;
}

int 
main (int argc, char ** argv)
{
	if (argc != 2) {
		fprintf(stderr, "Wrong number of arguments\n") ;
		exit(1) ;
	}

	char * s ;
	s = read_exec(argv[1]) ;
	printf("\"%s\"\n", s) ;
	free(s) ;
	exit(0) ;
}
