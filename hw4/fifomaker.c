#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

int main()
{
	if(mkfifo(".ddtrace", 0666)){
		if(errno != EEXIST){
			perror("fail to open fifo : ");
			exit(1);
		}
	}
}
