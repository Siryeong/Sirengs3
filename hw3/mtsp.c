#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct {
	sem_t filled ;
	sem_t empty ;
	pthread_mutex_t lock ;
	char ** elem ;
	int capacity ;
	int num ; 
	int front ;
	int rear ;
} bounded_buffer;

typedef struct {
	pthread_t tid;
	int fin;
	int cur;
	char subt[50];
} information;

bounded_buffer * buf = 0x0;
information info[8];
pthread_t ui;
pthread_t prod;
sem_t lock, lock2;
int m[50][50];
int path[50];
int min = -1;
int matsize = 0;
int run = 0;
int nth = 0;
unsigned long long count[2] = {0,};
char key[8][2] = {"0", "1", "2", "3", "4", "5", "6", "7"};

void print_all(int sel)
{
	int i;
	int sum = 0;
	sem_wait(&lock);
	if(sel == 2){
		for(i = 0; i < nth; i++)
			sum += info[i].cur;
	}

	if(sel == 0 || sel == 2){
		printf("\n[Status]\n");
		printf("The number of checked routes upto now\n -> 0xEFFF FFFF FFFF FFFF * %llu + %llu\n", count[1], count[0] + sum);
		printf("Best solution upto now\n -> %d [", min);
		for(i = 0; i < matsize; i++)
			printf("%d ", path[i]);
		printf("%d]\n\n", path[0]);

	}
	if(sel == 1){
		printf("\n\nThe number of consumer threads : %d\n", nth);
		printf("run : %d\n", run);
		for(i = 0; i < nth; i++){
			printf("[Thread %d]\n - ID : %ld\n - processed : %d\n - checked in current : %d\n", i, info[i].tid, info[i].fin, info[i].cur);
		}	printf("\n");

	}
	sem_post(&lock);
}

void sig_handler(int sig)
{
	print_all(2);
	exit(0);
}

void bounded_buffer_init(bounded_buffer * buf, int capacity) {
	sem_init(&(buf->filled), 0, 0) ;
	sem_init(&(buf->empty), 0, capacity) ;
	pthread_mutex_init(&(buf->lock), 0x0) ;
	buf->capacity = capacity ;
	buf->elem = (char **) calloc(sizeof(char *), capacity) ;
	buf->num = 0 ;
	buf->front = 0 ;
	buf->rear = 0 ;
}

void buffer_push(bounded_buffer * buf, char * msg) 
{
	sem_wait(&(buf->empty)) ;
	pthread_mutex_lock(&(buf->lock)) ;
	buf->elem[buf->rear] = msg ;
	buf->rear = (buf->rear + 1) % buf->capacity ;
	buf->num += 1 ;
	pthread_mutex_unlock(&(buf->lock)) ;
	sem_post(&(buf->filled)) ;
}

char * buffer_pop(bounded_buffer * buf) 
{
	char * r = 0x0 ;
	if(buf->num == 0) return r;
	sem_wait(&(buf->filled)) ;
	pthread_mutex_lock(&(buf->lock)) ;
	r = buf->elem[buf->front] ;
	buf->front = (buf->front + 1) % buf->capacity ;
	buf->num -= 1 ;
	pthread_mutex_unlock(&(buf->lock)) ;
	sem_post(&(buf->empty)) ;
	return r ;
}

void _travel(int idx, int * prefix, int * used, int * length, int inf)
{
	int i;
	if(idx == matsize){
		*length += m[prefix[matsize-1]][prefix[0]];
		info[inf].cur++;
		if(min == -1 || min > *length){
			sem_wait(&lock);
			min = *length;
			for(i = 0; i < matsize; i++)
				path[i] = prefix[i];
			sem_post(&lock);
		}
		*length -= m[prefix[matsize-1]][prefix[0]];
	}else{
		for(i = 0; i < matsize; i++){
			if(used[i] == 0){
				prefix[idx] = i;
				used[i] = 1;
				*length += m[prefix[idx-1]][i];
				_travel(idx+1, prefix, used, length, inf);
				*length -= m[prefix[idx-1]][i];
				used[i] = 0;
			}
		}
	}
}

void subtask(int r, int * prefix, int * used)
{
	int i;
	char msg[51];
	if(r == (matsize - 11)){
		for(i = 0; i < matsize-11; i++)
			msg[i] = (char)prefix[i] + 33;
		msg[i] = 0;
		while(buf->num > nth);
		buffer_push(buf, strdup(msg));
	}else{
		for(i = 0; i < matsize; i++){
			if(used[i] == 0){
				prefix[r] = i;
				used[i] = 1;
				subtask(r+1, prefix, used);
				used[i] = 0;
			}
		}
	}
}

void * producer(void * ptr) 
{
	int prefix[50] = {0,};
	int used[50] = {0,};
	used[0] = 1;
	subtask(1, prefix, used);

	while(run > 0);
	usleep(1000);
	pthread_cancel(ui);
	print_all(0);

	return 0x0;
}

void * consumer(void * ptr) 
{
	char * msg;
	int used[50]={0,};
	int prefix[50]={0,};
	int i, idx, length;
	idx = atoi((char*)ptr);

	sem_wait(&lock);
	info[idx].fin = 0;
	run++; 
	sem_post(&lock);
	//working start
	while(1){
		usleep(1000);
		msg = buffer_pop(buf);
		if(msg != NULL){
			//initial
			strcpy(info[idx].subt, msg);
			info[idx].cur = 0;
			length = 0;
			for(i = 0; i < matsize; i++)
				used[i] = 0;
			for(i = 0; i < matsize-11; i++)
				prefix[i] = (int)msg[i] - 33;
			free(msg);
			//travel start
			for(i = 0; i < matsize-11; i++){
				used[prefix[i]] = 1;
				length += m[prefix[i]][prefix[i+1]];
			}	length -= m[prefix[i-1]][prefix[i]];
			_travel(i, prefix, used, &length, idx);
			info[idx].fin++;

			sem_wait(&lock);
			count[0] += info[idx].cur;
			if(count[0] > 0xEFFFFFFFFFFFFFFF){
				count[1]++;
				count[0] %= 0xEFFFFFFFFFFFFFFF;
			}
			sem_post(&lock);
		}else{ break; }
	}
	sem_wait(&lock); run--; sem_post(&lock);

	return 0x0;
}

void * interface(void * ptr)
{
	char command[10] = {0,};
	int i, num;
	printf("thread started!\n");

	while(1){
		printf("command [stat, threads, num N]: ");
		fgets(command, 10, stdin);
		command[strlen(command)-1] = 0;

		if(strcmp(command, "stat") == 0){
			print_all(2);

		}else if(strcmp(command, "threads") == 0){
			print_all(1);

		}else if(strncmp(command, "num", 3) == 0){
			num = (int)command[4] - 48;
			if(num > 8 || num < 1){
				printf("N should be 1~8\n");
				continue;
			}

			sem_wait(&lock);
			if(num > nth){
				printf("wait...\n");
				for(i = nth; i < num; i++)
					pthread_create(&(info[i].tid), NULL, consumer, (void *)key[i]);
				usleep(1000);
				printf("%d threads created\n", num - nth);
				nth = num;	
			}
			if(num < nth){
				printf("wait...\n");
				for(i = nth-1; i >= num; i--){
					buffer_push(buf, strdup(info[i].subt));
					pthread_cancel(info[i].tid);
					info[i].tid = 0;
					info[i].cur = 0;
					info[i].fin = 0;
					info[i].subt[0] = 0;
					run--;
				}
				usleep(1000);
				printf("%d threads removed\n", nth - num);
				nth = num;
			}
			sem_post(&lock);

		}else{
			printf("command invalid\n");
		}
	}
	return 0x0;
}

int main(int argc, char * argv[])
{
	int i, j, t;
	char N[3] = {0,};

	//check argument valid
	if(argc != 3){
		printf("argument invalid\n");
		exit(-1);
	}
	//check how many thread
	nth = atoi(argv[2]);
	if(nth > 8 || nth < 1){
		printf("# of threads invalid\n");
		exit(-1);
	}
	//check file open
	FILE * fp = fopen(argv[1], "r");
	if(fp == 0) {
		printf("file open fail\n");
		exit(-1);
	}
	// mat size from file name
	N[0] = argv[1][2]; 
	N[1] = argv[1][3];
	matsize = atoi(N);
	if(matsize < 13 || matsize > 50){
		printf("matrix size invalid");
		exit(-1);
	}
	//matrix init
	for(i = 0; i < matsize; i++){
		for(j = 0; j < matsize; j++){
			fscanf(fp, "%d", &t);
			m[i][j] = t;
		}
	}
	fclose(fp);	
	// signal handler routine
	signal(SIGINT, sig_handler);

	// sem, queue init
	sem_init(&lock, 0, 1);
	sem_init(&lock2, 0, 1);
	buf = malloc(sizeof(bounded_buffer));
	bounded_buffer_init(buf, 32);

	//thread start
	pthread_create(&ui, NULL, interface, NULL);
	pthread_create(&prod, NULL, producer, NULL);
	for(i = 0; i < nth; i++)
		pthread_create(&(info[i].tid), NULL, consumer, (void *)key[i]);

	pthread_join(prod, NULL);

	//sem_destroy(&lock);
	exit(0);	 
}
