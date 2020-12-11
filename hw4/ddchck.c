#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

typedef struct _graphnode {
	pthread_t tid;
	unsigned int m;
	int hi;
} GNODE;

GNODE nodes[11];
int edge[11][11];
int visit[11];
int vqueue[10];
int num = 0;
int pre = 0;
int visitcnt = 0;

void init_all()
{
	int i, j;
	GNODE empty;
	for(i = 0; i < 11; i++){
		visit[i] = 0;
		nodes[i] = empty;
		for(j = 0; j < 11; j++){
			edge[i][j] = 0;
		}
	}
	num = 0;
	pre = 0;
	visitcnt = 0;
}

void print_edge()
{
	int i, j;
	for(i = 0; i < num; i++){
		for(j = 0; j < num; j++)
			printf("%d ", edge[i][j]);
		printf("\n");
	}	printf("\n");

}

void print_cycleinfo(int a, int b)
{
	int i, k;
	printf("Deadlock warning / Cycle is detected\n");
	for(i = a; i <= b; i++){
		printf("[visit : %d, ", nodes[vqueue[i%10]].hi);
		printf("thread : %lu, ",nodes[vqueue[i%10]].tid);
		printf("mutex : %x]\n", nodes[vqueue[i%10]].m);
	}
}

void checker(int v)
{
	int i, j;
	visitcnt++;
	visit[v] = 1;
	nodes[v].hi = visitcnt;
	vqueue[visitcnt%10] = v;
	for(i = 0; i < num; i++){
		if(edge[v][i] == 1){
			if(visit[i] == 1){
				//print_edge();
				print_cycleinfo(nodes[i].hi, nodes[v].hi);
			}else{
				checker(i);
			}
		}
	}
	visit[v] = 0;
}

void graphIn(GNODE node)
{
	int i;
	for(i = 0; i < num; i++){
		if(node.m == nodes[i].m) 
			break;
	}

	if(pre != i)
		edge[pre][i] = 1;
	pre = i;

	if(i == num){
		nodes[i] = node;
		num++;
	}
}

void graphDel(GNODE node)
{
	int i, j, k;
	for(i = 0; i < num; i++){
		if(node.m == nodes[i].m) 
			break;
	}
	for(j = i; j < num; j++){
		nodes[j] = nodes[j+1];
		for(k = 0; k < num; k++)
			edge[j][k] = edge[j+1][k];
		for(k = 0; k < num; k++)
			edge[k][j] = edge[k][j+1];
	}
	num--;
}

int main(int argc, char **argv)
{
	if(argc != 2){
		printf("argument error\n");
		exit(1);
	}
	init_all();

	int fd = open(".ddtrace", O_RDONLY | O_SYNC);
	
	while(1) {
		char s[128];
		int len;
		if((len = read(fd, s, 128)) == -1) break;
		if(len > 0){
			char l[10];
			GNODE node;
			int c;
			sscanf(s, "%s %lu %x %d", l, &node.tid, &node.m, &c);
			//printf("re : [%s,%lu,%x]\n", l, node.tid, node.m);
			if(!strcmp("lock", l)){
				if(c == 0) init_all();
				graphIn(node);
				checker(0);
			}
			else if(!strcmp("unlock", l)){
				graphDel(node);
			}
		}
	}
	close(fd);
	return 0;
}
