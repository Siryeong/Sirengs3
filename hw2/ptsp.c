#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <error.h>
#include <string.h>
#include <time.h>

pid_t pids[12];
int pipe2[2];
int m[50][50];
int path[50];
int used[50];
int prefix[50];
int length = 0;
int min = -1;
int matsize = 0;
int chlds;
int ischld = 0;
unsigned long long count[2] = {0, };

void prefix_mixer(int p);
int wating_result();
void to_master(int i);

void sig_handler(int sig)
{
	if(ischld && (sig == SIGTERM)){ // child end
		to_master(13);
		exit(0);
	}
	if(ischld == 0){ // master end
		int i;
		for(i = 0; i < chlds; i++){
			if(pids[i] > 0){
				kill(pids[i], SIGTERM);
				while(wating_result() < 0);
			}
		}
		while(wait(0) != -1);
		printf("\ntotal checked routes : 0xEFFFFFFFFFFFFFFF*%llu + %llu\n", count[1], count[0]);
		printf("best solution upto now : %d [", min);
		for(i = 0; i < matsize; i++)
			printf("%d ", path[i]);
		printf("%d]\n", path[0]);
		exit(0);
	}
}

void _travel(int idx)
{
	int i;
	if(idx == matsize){
		count[0]++;
		length += m[prefix[matsize-1]][prefix[0]];
		if(min == -1 || min > length){
			min = length;
			memcpy(path, prefix, sizeof(prefix));
		}
		length -= m[prefix[matsize-1]][prefix[0]];
	}else{
		for(i = 0; i < matsize; i++){
			if(used[i] == 0){
				prefix[idx] = i;
				used[i] = 1;
				length += m[prefix[idx-1]][i];
				_travel(idx+1);
				length -= m[prefix[idx-1]][i];
				used[i] = 0;
			}
		}
	}
}

int wating_result()
{
	int r, i=-1, w, len;
	char subs[300];
	r = read(pipe2[0], subs, 299);
	if(r > 0){ // receive string from child
		w = wait(0);
		subs[r+1] = 0x0;
		//puts(subs);
		i = atoi(strtok(subs, " "));
		len = atoi(strtok(NULL, " "));
		count[0] += atoll(strtok(NULL, " "));
		if(count[0] > 0xEFFFFFFFFFFFFFFF){
			count[1]++;
			count[0] %= 0xEFFFFFFFFFFFFFFF;
		}
		pids[i] = -1;
		if(min == -1 || min > len){
			min = len;
			for(i = 0; i < matsize; i++){
				path[i] = atoi(strtok(NULL, " "));
			}
		}
	}

	return i;
}

void to_master(int i)
{
	int t;
	char buf[300];
	char ci[5];
	sprintf(buf,"%d %d %llu ", i, min, count[0]);
	for(t = 0; t < matsize; t++){
		sprintf(ci, "%d ", path[t]);
		strcat(buf, ci);
	}
	write(pipe2[1], buf, strlen(buf)+1);
	close(pipe2[1]);
}

void child_proc(int i)
{
	int t;
	ischld = 1;
	min = -1;
	count[0] = 0;
	close(pipe2[0]);

	for(t = 0; t < matsize-12; t++){
		used[prefix[t]] = 1;
		length += m[prefix[t]][prefix[t+1]];
	}
	length -= m[prefix[t-1]][prefix[t]];
	_travel(t);
	to_master(i);

	exit(0);
}

void prefix_mixer(int p)
{
	int i, a, b, t;
	srand(time(NULL)+p);
	for(i = 0; i < 50; i++){ // random prefix
		a = rand()%matsize;
		b = rand()%matsize;
		t = prefix[a];
		prefix[a] = prefix[b];
		prefix[b] = t;
	}
}


int main(int argc, char* argv[])
{
	int i, j, t;
	char N[3] = {0,};
	if(argc != 3){ // argument excep
		printf("argument invalid\n");
		exit(-1);
	}
	chlds = atoi(argv[2]); // how many use chlds
	if(chlds > 12 || chlds < 1){
		printf("# of childs invalid\n");
		exit(-1);
	}
	FILE * fp = fopen(argv[1], "r");
	if(fp == 0) { // file open excep
		printf("file open fail\n");
		exit(-1);
	}

	N[0] = argv[1][2]; // mat size from filename
	N[1] = argv[1][3];
	matsize = atoi(N);
	if(matsize < 13 || matsize > 50){
		printf("matrix size invalid");
	}
	signal(SIGINT, sig_handler); // sigint handler
	signal(SIGTERM, sig_handler);

	for(i = 0; i < chlds; i++) // pid set
		pids[i] = -1;

	for(i = 0; i < matsize; i++){ // matrix set
		prefix[i] = i;
		for(j = 0; j < matsize; j++){
			fscanf(fp, "%d", &t);
			m[i][j] = t;
		}
	}
	fclose(fp);
	if(pipe(pipe2) != 0){
		perror("Error");
		exit(1);
	}
	
	i = 0;
	while(1){ // start
		if(pids[i] == -1){
			prefix_mixer(i);
			pids[i] = fork();
			if(pids[i] == 0)
				child_proc(i);
		}else{	
			j = wating_result();
			if(j > -1) i = j-1;
		}
		i++;
		if(i >= chlds) i = 0;
	} //while

	exit(0);	 
}
