#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <dlfcn.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex4 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex5 = PTHREAD_MUTEX_INITIALIZER;

void 
noise()
{
	usleep(rand() % 1000) ;
}


int 
main(int argc, char *argv[]) 
{
	srand(time(0x0)) ;

		
	pthread_mutex_lock(&mutex); noise() ; 
	pthread_mutex_lock(&mutex2);	noise() ; 
	pthread_mutex_lock(&mutex3); noise() ; 
	pthread_mutex_lock(&mutex4); noise() ; 
	pthread_mutex_lock(&mutex5); noise() ;
	printf("all lock\n");
	pthread_mutex_unlock(&mutex5); noise() ;
	pthread_mutex_unlock(&mutex4); noise() ;
	pthread_mutex_unlock(&mutex3); noise() ;
	pthread_mutex_unlock(&mutex2); noise() ;
	pthread_mutex_unlock(&mutex); noise() ;

	return 0;
}

