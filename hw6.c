#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _node{
    struct _node * next;
    int idx;
    char city;
}NODE;

NODE list[21];
NODE trans[21];
int graph[21][21] = {0,};
int matsize = 0;

void insert_list(NODE * head, char city, int idx)
{
    NODE * search = head;
    while(search->next != NULL)
        search = search->next;
    
    NODE * node = malloc(sizeof(NODE));
    node->city = city;
    node->idx = idx;
    node->next = NULL;
    search->next = node;
}

void printlist(NODE * arr)
{
    NODE * search;
    int i;
    
    for(i = 1; i <= matsize; i++){
        search = &arr[i];
        do{
            printf("%c -> ",search->city);
            search = search->next;
        }while(search != NULL);
        printf("nil \n");
    }
    
}

void graph2list()
{
    int i, j;
    for(i = 0; i <= matsize; i++){
        list[i].city = (char)graph[i][0];
        list[i].next = NULL;
        list[i].idx = i;
    }
    
    for(i = 1; i <= matsize; i++){
        for(j = 1; j <= matsize; j++){
            if(graph[i][j] == 1){
                insert_list(&list[i], (char)graph[0][j], j);
            }
        }
    }
    
    printlist(list);
}

void transpose()
{
    NODE * search;
    int i, idx;
    for(i = 0; i <= matsize; i++){
        trans[i].city = (char)graph[0][i];
        trans[i].next = NULL;
        trans[i].idx = i;
    }
    
    for(i = 1; i <= matsize; i++){
        search = &list[i];
        search = search->next;
        while(search != NULL){
            idx = search->idx;
            insert_list(&trans[idx], (char)graph[0][i], i);
            search = search->next;
        }
    }
    
    printlist(trans);
    
}

void printmatrix()
{
    int i, j;
    for(i = 0; i <= matsize; i++){
        for(j = 0; j <= matsize; j++){
            printf("%d ", graph[i][j]);
        }   printf("\n");
    }
}

void remove_space(char * str)
{
    int i = 0, j = 0;
    char tmp[100];
    strcpy(tmp, str);
    
    while(1){
        if(tmp[i] == '\0')
        { str[j] = 0; break; }
        else if(tmp[i] == '\n' || tmp[i] == ' ' || tmp[i] ==  '\t' || tmp[i] == '\r')
        { i++; }
        else
        { str[j++] = tmp[i++]; }
    } // while
    
}

void matrix_init()
{
    char buf[100];
    int x, y;
    FILE  * fp;
    
    fp = fopen("hw6_data.txt", "r");
    if (fp == NULL) {
        printf("file not exist error\n");
        exit(-1);
    }
    
    fgets(buf, 100, fp);
    remove_space(buf);
    matsize = strlen(buf);
    for(x = 1; x <= matsize; x++)
        graph[0][x] = (int)buf[x-1];
    for(y = 1; y <= matsize; y++){
        fgets(buf, 100, fp);
        remove_space(buf);
        graph[y][0] = (int)buf[0];
        for(x = 1; x <= matsize; x++)
            graph[y][x] = (int)buf[x] - '0';
    }
}

int main()
{
    matrix_init();
    //printmatrix();
	printf("<adjacency>\n");
    graph2list();
    printf("=======================\n");
	printf("<transpose>\n");
    transpose();

    
    return 0;
}
