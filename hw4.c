#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CLOCKS_PER_MS (CLOCKS_PER_SEC/1000.0)
#define MAX(a, b) ((a) > (b) ? (a) : (b))

typedef struct _Item{
	int weight;
	int value;
	float vperw;
} ITEM;

ITEM items[10001];

int compare(const void * a, const void * b) // for qsort
{return (int)( ((ITEM *)b)->vperw - ((ITEM *)a)->vperw );}

void random_gen(int num);
float greedy(int max_W, int num);
int dynamic_prog(int max_W, int num);
int branch_bnd(int num);

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
	printf("%-6s|%-22s|%-21s|%-20s|\n", "Items", "Greedy", "D. P.", "B. & B.");
	fprintf(fp, "%-6s|%-22s|%-21s|%-20s|\n", "Items", "Greedy", "D. P.", "B. & B.");
	for(i = 1; i < 10; i++){
		random_gen(num[i]);
		qsort(items, num[i]+1, sizeof(ITEM), compare);
		max_W = num[i]*40;

		m1 = clock();
		gdy = greedy(max_W, num[i]);
		m2 = clock();
		dp = dynamic_prog(max_W, num[i]);
		m3 = clock();
		bnb = branch_bnd(0);
		end = clock();
		
		fprintf(fp, "%-6d|", num[i]);
		fprintf(fp, "%6.3fms/%13.3f|", (float)(m2-m1)/CLOCKS_PER_MS, gdy);
		fprintf(fp, "%10.3fms/%8d|", (float)(m3-m2)/CLOCKS_PER_MS, dp);
		fprintf(fp, "%.3fms/%d|", (float)(end-m3)/CLOCKS_PER_MS, bnb);
		fprintf(fp, "\n");

		printf("%-6d|", num[i]);
		printf("%6.3fms/%13.3f|", (float)(m2-m1)/CLOCKS_PER_MS, gdy);
		printf("%10.3fms/%8d|", (float)(m3-m2)/CLOCKS_PER_MS, dp);
		printf("%.3fms/%d|", (float)(end-m3)/CLOCKS_PER_MS, bnb);
		printf("\n");	
	}
	fclose(fp);

	return 0;
}

void random_gen(int num)
{
	int i, s1 = 0, s2 = 0;
	items[0].weight = 0;
	items[0].value = 0;
	items[0].vperw = 5000000; 
	srand(clock());
	for(i = 1; i <= num; i++){
		items[i].weight = random()%100+1;
		items[i].value = random()%300+1;
		items[i].vperw = (float)((items[i].value*1000.0)/items[i].weight);	
		s1+= items[i].weight;
		s2+= items[i].value;
	}
}

float greedy(int max_W, int num)
{
	int i = 0, w = 0;
	float v = 0.0f;
	for(i = 1; i <= num; i++){
		if( items[i].weight <= (max_W-w) ){
			v += (float)items[i].value;
			w += items[i].weight;
		}else{
			v += (float)(items[i].vperw/1000.0) * (max_W-w); // rest value
			w += max_W - w;
			break;
		}
	}

	return v;
}

int dynamic_prog(int max_W, int num)
{
	//int B[num+1][max_W+1]; // too much memory
	int B[3][max_W+1]; // eco mem
	int i, w, v;

	for(w = 0; w < max_W; w++)
		B[0][w] = 0;
	for(i = 1; i <= num; i++){
		B[i%3][0] = 0;
		for(w = 1; w <= max_W; w++){
			if(items[i].weight <= w){
				B[i%3][w] = MAX(items[i].value + B[(i-1)%3][w-items[i].weight], B[(i-1)%3][w]);
			}else{B[i%3][w] = B[(i-1)%3][w];}
		}
	}
	return B[num%3][max_W];
}

int branch_bnd(int num)
{	
	return 0;
}

