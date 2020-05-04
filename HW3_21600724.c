#include <stdio.h>

int comb_recur(int n, int k);
int comb_dynmProg(int n, int k);

int main()
{
	int n, k;
	scanf(" %d %d", &n, &k);
	printf("dynamic programming comb(%d, %d) : %d\n", n, k, comb_dynmProg(n, k));
	printf("recursive comb(%d, %d) : %d\n", n, k, comb_recur(n, k));

	return 0;
}

//combination with recursive
int comb_recur(int n, int k)
{
	if(n < 0 || k < 0 || n < k)
	{	printf("invalid input value!\n");
		return -1;
	}

	if(n == 0) return 0;
	if(k == 0 || n == k) return 1;
	return comb_recur(n-1, k-1) + comb_recur(n-1, k);
	
}

//combination with dynamic programing
int comb_dynmProg(int n, int k)
{
	if(n < 0 || k < 0 || n < k)
	{	printf("invalid input value!\n");
		return -1;
	}

	int table[n+1][n+1];
	table[0][0] = 1;
	for(int i = 1; i <= n; i++)
	{	table[0][i] = 0;
		table[i][0] = 1;
		table[i][i] = 1;
	}

	for(int i = 2; i <= n; i++)
	{	for(int j = 1; j <= k; j++)
			if(i > j) table[i][j] = table[i-1][j-1] + table[i-1][j];		
	}

	return table[n][k];
}