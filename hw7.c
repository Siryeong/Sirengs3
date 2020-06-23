#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INF 999999999
#define CLOCKS_PER_MS (CLOCKS_PER_SEC/1000.0)

typedef struct _node{
    struct _node * next;
    char city[32];
    int city_code;
    int edge;
} NODE;

typedef struct _matrixnode{
    char element[32];
} MATNODE;

MATNODE read_mat[31][31];
NODE city_list[31];
NODE minheap[31];
int heap_size;
int citynum;
int visit[31];
int output_table[2][31][31];

void clear_table()
{
    int i, j;
    for(i = 0; i < 31; i++){
        for(j = 0; j < 31; j++){
            output_table[0][i][j] = INF;
            output_table[1][i][j] = INF;
        }
    }
    for(i = 0; i < 31; i++){
        output_table[0][i][i] = 0;
        output_table[1][i][i] = 0;
        
    }
}

void heapswap(int a, int b)
{
    NODE tmp = minheap[a];
    minheap[a] = minheap[b];
    minheap[b] = tmp;
}

void min_heappush(int i)
{
    int p = i/2;
    if(p > 0 && minheap[i].edge < minheap[p].edge){
        heapswap(i, p);
        min_heappush(p);
    }
}

void min_heappop(int i)
{
    int l = i*2;
    int r = i*2 + 1;
    int s = i;
    
    if(l <= heap_size && minheap[l].edge < minheap[i].edge){ s = l; }
    if(r <= heap_size && minheap[r].edge < minheap[i].edge){ s = r; }
    if(s != i){
        heapswap(i, s);
        min_heappop(s);
    }
}

void push(NODE node)
{
    heap_size++;
    minheap[heap_size] = node;
    min_heappush(heap_size);
}

NODE pop()
{
    NODE n = minheap[1];
    minheap[1] = minheap[heap_size];
    heap_size--;
    min_heappop(1);
    
    return n;
}

void insert_node(NODE * head, char * city, int city_code, int edge)
{
    NODE * search = head;
    while(search->next != NULL)
        search = search->next;
    
    NODE * node = malloc(sizeof(NODE));
    strcpy(node->city, city);
    node->city_code = city_code;
    node->edge = edge;
    node->next = NULL;
    search->next = node;
}

void matrix_init()
{
    
    char buffer[128];
    FILE * fp;
    fp = fopen("hw7.data", "r");
    if(fp == NULL){
        printf("fill open error");
        exit(1);
    }
    
    int i = 0, j = 0;
    while(fscanf(fp, " %s", buffer) != EOF){ // store matrix
        if(!strcmp(buffer, read_mat[0][j].element)){
            i = 0;
            j++;
        }
        strcpy(read_mat[j][i++].element, buffer);
    } // while
    citynum = j;
    fclose(fp);
    
    for(i = 0; i < citynum; i++){ // node initialize
        city_list[i].next = NULL;
        strcpy(city_list[i].city, read_mat[0][i].element);
        city_list[i].city_code = i;
        city_list[i].edge = 0;
    } // for
    
    for(i = 0; i < citynum; i++){ // adjacency list init
        for(j = 0; j < citynum; j++){
            if(strcmp(read_mat[i+1][j+1].element, "INF") && i!=j)
                insert_node(&city_list[i], city_list[j].city, j, atoi(read_mat[i+1][j+1].element));
        }//for
    }//for
    
}

void RELAX(NODE u)
{
    NODE * search = u.next;
    while(search != NULL){
        if(!visit[search->city_code] && city_list[search->city_code].edge > (u.edge + search->edge)){
            city_list[search->city_code].edge = u.edge + search->edge;
            //printf("edge : %d, city : %d\n",search->edge,search->city_code);
            push(city_list[search->city_code]);
        }
        search = search->next;
    }// while
    //printf("--relax end \n");
}

void dijkstra(int city)
{
    int i, j;
    
    for(i = 0; i < citynum; i++){
        city_list[i].edge = INF;
        visit[i] = 0;
    }   city_list[city].edge = 0;
    push(city_list[city]);

    while(heap_size > 0){
        NODE u = pop();
        visit[u.city_code] = 1;
        RELAX(u);
    }
    
    for(i = 0; i < citynum; i++)
        output_table[(citynum-1)%2][city][i] = city_list[i].edge;
    
}

void floyd()
{
    int i, j, k;
    
    for(i = 0; i < citynum; i++)
        city_list[i].edge = 0;
    
    for(i = 0; i < citynum; i++){ // first table
        NODE * search = city_list[i].next;
        while(search != NULL){
            output_table[0][i][search->city_code] = search->edge;
            search = search->next;
        }// while
    }// for
    
    for(i = 1; i < citynum; i++){
        for(j = 0; j < citynum; j++){
            for(k = 0; k < citynum; k++){
                if(output_table[(i-1)%2][j][i] + output_table[(i-1)%2][i][k] < output_table[(i-1)%2][j][k]){
                    output_table[i%2][j][k] = output_table[(i-1)%2][j][i] + output_table[(i-1)%2][i][k];
                }else{
                    output_table[i%2][j][k] = output_table[(i-1)%2][j][k];
                }
            }
        }
    }
}

void print_table()
{
    int i, j;
    printf("%-10s", " ");
    for(i = 0; i < citynum; i++)
        printf("%-10s", city_list[i].city);
    printf("\n");
    for(i = 0; i < citynum; i++){
        printf("%-10s", city_list[i].city);
        for(j = 0; j < citynum; j++){
            printf("%-10d", output_table[(citynum-1)%2][i][j]);
        }   printf("\n");
    }
}

int main(int argc, char ** argv)
{
    clock_t m1, m2, m3;
    matrix_init();
    clear_table();
    int i;
    m1 = clock();
    for(i = 0; i < citynum; i++)
        dijkstra(i);
    m2 = clock();
    printf("it took %f seconds to compute shortest path between cities with Dijkstra's algoritnm as follows\n", (float)(m2-m1)/CLOCKS_PER_SEC/1.0);
    print_table();
    clear_table();
    m2 = clock();
    floyd();
    m3 = clock();
    printf("it took %f seconds to compute shortest path between cities with Floyd's algoritnm as follows\n", (float)(m3-m2)/CLOCKS_PER_SEC/1.0);
    print_table();
    
    return 0;
}
