#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <pthread.h>

static int count = 0;

//interpositioning 
int pthread_mutex_lock(pthread_mutex_t * mutex)
{
	int ret = 0;
	int a, b;
	int (*pthread_mutex_lockp)(int *) = NULL;
	char * error;

	pthread_mutex_lockp = dlsym(RTLD_NEXT, "pthread_mutex_lock");
	if((error = dlerror()) != 0x0) exit(1);

	char buf[128];
	snprintf(buf, 128, "lock %lu %p %d\n", pthread_self(), mutex, count);
	int fd = open(".ddtrace", O_WRONLY | O_SYNC);
	write(fd, buf, 128);
	close(fd);
	
	ret = pthread_mutex_lockp((int *)mutex);
	count++;
	//fputs(buf, stderr);
	
	return ret ; 
}	

int pthread_mutex_unlock(pthread_mutex_t * mutex)
{
	int ret = 0;
	int (*pthread_mutex_unlockp)(int *) = NULL;
	char * error;

	pthread_mutex_unlockp = dlsym(RTLD_NEXT, "pthread_mutex_unlock");
	if((error = dlerror()) != 0x0) exit(1);
	
	char buf[128];
	snprintf(buf, 128, "unlock %lu %p %d\n", pthread_self(), mutex, count);
	int fd = open(".ddtrace", O_WRONLY | O_SYNC);
	write(fd, buf, 128);
	close(fd);
	
	ret = pthread_mutex_unlockp((int *)mutex);
	count--;
	//fputs(buf, stderr);

	return ret ; 
}
