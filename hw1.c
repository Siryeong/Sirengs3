#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QSIZE 30
#define MAX_NAME 20

typedef struct _node{
	char name[MAX_NAME];
	int key;
} Q_NODE;

Q_NODE heap[MAX_QSIZE+1];

void print_menu();
void insertQ();
Q_NODE deleteQ();
void increaseKey(int i, int k);
void increaseInput();
void max_heapify(int i);
void print_elements();
Q_NODE get_max();
void swap(int x, int y);

int main(){
	heap[0].key = 0; // initialize # of elements

	while(1){ // repeat command input
		print_menu();
		printf("\nChoose menu : ");
		char command = 0;
		scanf(" %c", &command);
		if(command == 'Q') break;

		switch(command){
			case 'I' :
				insertQ();
				break;
			case 'D' :
				deleteQ();
				break;
			case 'R' :
				get_max();
				break;
			case 'K':
				increaseInput();
				break;
			case 'P' :
				print_elements();
				break;
			default:
				printf("%c menu is not exist\n", command); // not allowed menu
				break;
  	}
	}

	return 0;
}

void print_menu(){ // print menu
	printf("\n*************  MENU  *************\n");
	printf("I : Insert new element into queue.\n");
	printf("D : Delete element with largest key from queue.\n");
	printf("R : Retrieve element with largest key from queue.\n");
	printf("K : Increase key of element in queue.\n");
	printf("P : Print out all elements in queue.\n");
	printf("Q : Quit\n");
}

void insertQ(){ // insert in queue
	if(heap[0].key >= MAX_QSIZE){printf("Queue is full\n"); return;}
	char * str;
	int key = 0;

	heap[0].key++; // # of elements increase
	printf("Enter name of element : ");
	scanf(" %s", str);
	if(strlen(str) >= 20 ) {printf("Too long name! Should be under 20 characters.\n"); heap[0].key--; return;}
	printf("Enter key value of element : ");
	scanf(" %d", &key);
	if((key < 1) || (key > 10)){printf("Not allowed value. Please enter between 1~10\n"); heap[0].key--; return;}
	
	//element values store
	strcpy(heap[heap[0].key].name, str);
	heap[heap[0].key].key = 0;
	increaseKey(heap[0].key, key);
	printf("New element [%s, %d] is inserted\n", str, key);
}

void increaseKey(int i, int k){ // heap increase key
	if(k < heap[i].key){printf("New key is smaller than current key!\n"); return;}
	heap[i].key = k;

	while((i > 1) && (heap[i/2].key < heap[i].key)){
		swap(i/2, i);
		i = i/2;
	}
}

void increaseInput(){ // k mode input routine
	int i, k;
	printf("Enter index of element : ");
	scanf(" %d", &i);
	if((i > heap[0].key) || i == 0){printf("Not allowd. Max index is %d\n", heap[0].key); return;}
	printf("Enter key value of element : ");
	scanf(" %d", &k);
	if((k > 10) || (k < 1)){printf("Now allowed value. Please enter between %d~10\n", heap[i].key+1); return;}
	increaseKey(i, k);
}

void swap(int x, int y){
	Q_NODE tmp = heap[x];
	heap[x] = heap[y];
	heap[y] = tmp;
}

Q_NODE deleteQ(){
	if(heap[0].key < 1){printf("Queue is empty\n"); return heap[0];}

	Q_NODE max = heap[1];
	heap[1] = heap[heap[0].key];
	heap[0].key--;
	max_heapify(1);
	printf("[%s, %d] is deleted\n", max.name, max.key);
	return max;
}

void max_heapify(int i){
	int l = i*2;
	int r = i*2+1;
	int largest = 0;

	if((l <= heap[0].key) && (heap[l].key > heap[i].key)){ largest = l; } else { largest = i; }
	if((r <= heap[0].key) && (heap[r].key > heap[largest].key)){largest = r;}
	if(largest != i){
		swap(i, largest);
		max_heapify(largest);
	}
}

void print_elements(){
	if(heap[0].key == 0){printf("Queue is empty!\n"); return;}
	int i = 0;
	for(i = 1; i <= heap[0].key; i++){
		printf("[%s, %d] ", heap[i].name, heap[i].key);
	}printf("\n");
}

Q_NODE get_max(){
	if(heap[0].key == 0){printf("Queue is empty!\n"); return heap[0];}
	printf("[%s, %d]\n", heap[1].name, heap[1].key);
	return heap[1];
}
