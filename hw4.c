#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CLOCKS_PER_MS (CLOCKS_PER_SEC/1000.0)
#define MAX(a, b) ((a) > (b) ? (a) : (b))

typedef struct _Item{
	int weight;
	int bnf;
	float bperw;
} ITEM;

typedef struct _Node{
	int bnf;
	int weight;
	float bnd;
	int level;
} Q_NODE;

ITEM items[10001];
Q_NODE heap[100001]; // max heap priority queue
int heap_size;

int compare(const void * a, const void * b)
{ // qsort b/w descending order
	float r = ((ITEM *)b)->bperw - ((ITEM *)a)->bperw;
	if(r > 0.0) return 1;
	if(r == 0.0) return 0;
	if(r < 0.0) return -1;
}

void random_gen(int num);
float greedy(int max_W, int num);
int dynamic_prog(int max_W, int num);
int branch_bnd(int max_W, int num);
float cal_bound(int max_W, int start, int num);

// priority queue for branch and bound
void heap_push(Q_NODE node);
Q_NODE heap_pop();
void max_heapIN(int i);
void max_heapDel(int i);
void heap_swap(int a, int b);

int main()
{
	FILE* fp = fopen("output.txt", "w");
	int num[10] = {0, 10, 100, 500, 1000, 3000, 5000, 7000, 9000, 10000};
	int i, max_W;
	clock_t m1, m2, m3, end;
	float gdy;
	int dp, bnb;

	printf("\tprocessing time / maximum benefit value\n");
	fprintf(fp, "\tprocessing time / maximum benefit value\n");
	printf("%-6s|%-21s|%-21s|%-20s|\n", "Items", "Greedy", "D. P.", "B. & B.");
	fprintf(fp, "%-6s|%-21s|%-21s|%-20s|\n", "Items", "Greedy", "D. P.", "B. & B.");
	for(i = 1; i < 10; i++){
		random_gen(num[i]);
		qsort(items, num[i]+1, sizeof(ITEM), compare);
		max_W = num[i]*40;
		//timer
		m1 = clock();
		gdy = greedy(max_W, num[i]);
		m2 = clock();
		dp = dynamic_prog(max_W, num[i]);
		m3 = clock();
		bnb = branch_bnd(max_W, num[i]);
		end = clock();
		//file output to output.txt
		fprintf(fp, "%-6d|", num[i]);
		fprintf(fp, "%6.3fms/%12.3f|", (float)(m2-m1)/CLOCKS_PER_MS, gdy);
		fprintf(fp, "%10.3fms/%8d|", (float)(m3-m2)/CLOCKS_PER_MS, dp);
		fprintf(fp, "%9.3fms/%8d|", (float)(end-m3)/CLOCKS_PER_MS, bnb);
		fprintf(fp, "\n");
		// std output
		printf("%-6d|", num[i]);
		printf("%6.3fms/%12.3f|", (float)(m2-m1)/CLOCKS_PER_MS, gdy);
		printf("%10.3fms/%8d|", (float)(m3-m2)/CLOCKS_PER_MS, dp);
		printf("%9.3fms/%8d|", (float)(end-m3)/CLOCKS_PER_MS, bnb);
		printf("\n");	
	}
	fclose(fp);
	return 0;
}

void random_gen(int num)
{
	int i, s1 = 0, s2 = 0;
	items[0].weight = 0;
	items[0].bnf = 0;
	items[0].bperw = 5000000;
	srand(clock());
	for(i = 1; i <= num; i++){
		items[i].weight = random()%100+1;
		items[i].bnf = random()%300+1;
		items[i].bperw = (float)((items[i].bnf*1000.0)/items[i].weight);	
		s1+= items[i].weight;
		s2+= items[i].bnf;
	}
}

float greedy(int max_W, int num)
{
	int i = 0, w = 0;
	float v = 0.0f;
	for(i = 1; i <= num; i++){
		if( items[i].weight <= (max_W-w) ){
			v += (float)items[i].bnf;
			w += items[i].weight;
		}else{
			v += (float)(items[i].bperw/1000.0) * (max_W-w); // rest
			w += max_W - w;
			break;
		}
	}
	return v;
}

int dynamic_prog(int max_W, int num)
{
	//int B[num+1][max_W+1]; // too much memory
	int B[3][max_W+1]; // eco memory
	int i, w, v;

	for(w = 0; w < max_W; w++)
		B[0][w] = 0;
	for(i = 1; i <= num; i++){
		B[i%3][0] = 0;
		for(w = 1; w <= max_W; w++){
			if(items[i].weight <= w){
				B[i%3][w] = MAX(items[i].bnf + B[(i-1)%3][w-items[i].weight], B[(i-1)%3][w]);
			}else{B[i%3][w] = B[(i-1)%3][w];}
		}
	}
	return B[num%3][max_W];
}

int branch_bnd(int max_W, int num)
{	
	int i, max_benefit = 0;
	Q_NODE parent, childs[2];

	//initial and root
	heap_size = 0;
	heap[0].bnf = 0;
	heap[0].weight = 0;
	heap[0].bnd = greedy(max_W, num);
	heap[0].level = 0;
	heap_push(heap[0]);

	while(heap_size > 0){
		parent = heap_pop();
		//left child
		childs[0].level = parent.level + 1;
		childs[0].bnf = parent.bnf + items[childs[0].level].bnf;
		childs[0].weight = parent.weight + items[childs[0].level].weight;
		childs[0].bnd = childs[0].bnf + cal_bound(max_W - childs[0].weight,childs[0].level, num);
		//right child
		childs[1].level = parent.level + 1;
		childs[1].bnf = parent.bnf;
		childs[1].weight = parent.weight;
		childs[1].bnd = childs[1].bnf + cal_bound(max_W - childs[1].weight,childs[1].level + 1, num);
		// promising case push
		if((childs[0].bnd > max_benefit) && (childs[0].weight <= max_W)){
			heap_push(childs[0]);
			max_benefit = MAX(max_benefit, childs[0].bnf);
		}
		if((childs[1].bnd > max_benefit) && (childs[1].weight <= max_W)){
			heap_push(childs[1]);
			max_benefit = MAX(max_benefit, childs[1].bnf);
		}
	}
	return max_benefit;
}

float cal_bound(int max_W, int start, int num)
{
	int i = 0, w = 0;
	float v = 0.0f;
	for(i = start; i <= num; i++){
		if( items[i].weight <= (max_W-w) ){
			v += (float)items[i].bnf;
			w += items[i].weight;
		}else{
			v += (float)(items[i].bperw/1000.0) * (max_W-w); // rest value
			w += max_W - w;
			break;
		}
	}
	return v;
}

/*********priority queue**********/
void heap_push(Q_NODE node)
{
	heap_size++;
	if(heap_size > 100000){
		printf("heap overflow\n");
		heap_size = 0;
		return;
	}
	heap[heap_size] = node;
	max_heapIN(heap_size);
}

Q_NODE heap_pop()
{
	Q_NODE r = heap[1];
	heap[1] = heap[heap_size];
	heap_size--;
	max_heapDel(1);

	return r;
}

void max_heapIN(int i)
{
	int parent = i/2;
	if((parent > 0) && (heap[i].bnd > heap[parent].bnd)){
		heap_swap(i, parent);
		max_heapIN(parent);
	}
}

void max_heapDel(int i)
{
	int left = i*2;
	int right = i*2 + 1;
	int largest = i;
	
	if((left <= heap_size) && (heap[left].bnd > heap[i].bnd)){largest = left;}
	if((right <= heap_size) && (heap[right].bnd > heap[largest].bnd)){largest = right;}
	if(largest != i){
		heap_swap(i, largest);
		max_heapDel(largest);
	}
}

void heap_swap(int a, int b)
{
	Q_NODE tmp = heap[a];
	heap[a] = heap[b];
	heap[b] = tmp;
}
