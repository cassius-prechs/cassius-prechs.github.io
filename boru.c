#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/time.h>
#define BUF_SIZE 80

struct edge {
	int a;
	int b;
	int weight;
	int added;
};

int **matrix;
struct edge *edges;
struct edge *mst_edges;

int my_compare(const struct edge *a, const struct edge *b) {
	return (a->weight > b->weight) ? 1 : (a->weight < b->weight) ? -1 : 0;
}

void Disp(int size) {
	int i, j;
	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++)
			printf("%d ", matrix[i][j]);
		printf("\n");
	}
}

void Clear(int size) {
	int i;
	for (i = 0; i < size; i++) free(matrix[i]);
	free(matrix);
}

/*** Union-Find functions ***/
void UF_Init(int n);
void UF_Union(int a, int b);
int  UF_Find(int a);
void UF_Clear(int n);
/****************************/

void Boruvka_func(int size, int edge_num) {
	int i, j, k;
	int components = size;  // 初期状態では各ノードが独立した連結成分
	int mst_edge_count = 0;
	int total_weight = 0;
	
	/* Initialize the edge array */
	k = 0;
	for (i = 0; i < size; i++)
		for (j = 0; j < size; j++)
			if (matrix[i][j] > 0 && i < j) {  // 重複辺除去
				edges[k].weight = matrix[i][j];
				edges[k].added = 0;
				edges[k].a = i; 
				edges[k].b = j; 
				k++;
			}

	/* MST edges array allocation */
	if ((mst_edges = malloc(sizeof(struct edge) * (size - 1))) == NULL) {
		printf("Cannot allocate memory \n"); 
		exit(1);
	}

	UF_Init(size);  // Union-Findを初期化する
	
	/* Boruvka's algorithm main loop */
	while (components > 1) {
		int *cheapest = malloc(sizeof(int) * size);
		
		/* Initialize cheapest array to -1 */
		for (i = 0; i < size; i++) {
			cheapest[i] = -1;
		}
		
		/* Find cheapest edge for each component */
		for (i = 0; i < k; i++) {
			int comp_a = UF_Find(edges[i].a);
			int comp_b = UF_Find(edges[i].b);
			
			/* Skip if both vertices are in same component */
			if (comp_a == comp_b) continue;
			
			/* Update cheapest edge for component A */
			if (cheapest[comp_a] == -1 || edges[i].weight < edges[cheapest[comp_a]].weight) {
				cheapest[comp_a] = i;
			}
			
			/* Update cheapest edge for component B */
			if (cheapest[comp_b] == -1 || edges[i].weight < edges[cheapest[comp_b]].weight) {
				cheapest[comp_b] = i;
			}
		}
		
		/* Add cheapest edges to MST */
		for (i = 0; i < size; i++) {
			if (cheapest[i] != -1) {
				int edge_idx = cheapest[i];
				int comp_a = UF_Find(edges[edge_idx].a);
				int comp_b = UF_Find(edges[edge_idx].b);
				
				/* Add edge if it connects different components */
				if (comp_a != comp_b) {
					UF_Union(comp_a, comp_b);
					edges[edge_idx].added = 1;
					
					/* Store in MST edges array */
					mst_edges[mst_edge_count] = edges[edge_idx];
					mst_edge_count++;
					total_weight += edges[edge_idx].weight;
					components--;
				}
			}
		}
		
		free(cheapest);
	}
	
	/* Display MST edges */
	for (i = 0; i < mst_edge_count; i++) {
		printf("edge(%d,%d) is added\n", mst_edges[i].a, mst_edges[i].b);
	}
	
	printf("MST Total Weight: %d\n", total_weight);
	printf("MST Edge Count  : %d\n", mst_edge_count);
	
	free(mst_edges);
	UF_Clear(size);
}

void Boruvka(int size) {
	int i, j, edge_num = 0;
	struct timeval stime, etime;
	double sec;
	
	/* Count the number of edges */
	for (i = 0; i < size; i++)
		for (j = 0; j < size; j++)
			if (matrix[i][j] > 0) edge_num++;
	
	/* Allocate memory for the edge array */
	if ((edges = malloc(sizeof(struct edge) * edge_num)) == NULL) {
		printf("Cannot allocate memory \n"); 
		exit(1);
	}

	gettimeofday(&stime, NULL);
	Boruvka_func(size, edge_num);
	gettimeofday(&etime, NULL);
	
	sec = (etime.tv_sec - stime.tv_sec) +
		(etime.tv_usec - stime.tv_usec) / 1000000.0;
	printf("Elapsed Time %.8f [sec] \n", sec / (double)size);
	printf("Total Time %.8f [sec] \n", sec);

	free(edges);
}

int main(int argc, char *argv[]) {
	FILE *fp;
	int i, j, n;
	char buf[BUF_SIZE];
	char command;
	
	if (argc != 2) {
		printf("Usage: %s <filename>\n", argv[0]); 
		exit(1);
	}
	
	if ((fp = fopen(argv[1], "r")) == NULL) {
		printf("Cannot open file (%s) \n", argv[1]); 
		exit(1);
	}
	
	/* Read the file to check the graph size */
	fgets(buf, BUF_SIZE, fp);
	sscanf(buf, "%d", &n);

	/* Memory allocation for the input graph */
	if ((matrix = malloc(sizeof(int*) * n)) == NULL) {
		printf("Cannot allocate memory \n"); 
		exit(1);
	}
	for (i = 0; i < n; i++) {
		if ((matrix[i] = malloc(sizeof(int) * n)) == NULL) {
			printf("Cannot allocate memory \n"); 
			exit(1);
		}
	}
	
	/* Initialize the matrix */
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			matrix[i][j] = 0;
	
	/* Read the graph */
	while (fgets(buf, BUF_SIZE, fp) != NULL) {
		int a, b, c;
		sscanf(buf, "%d %d %d", &a, &b, &c);
		matrix[a][b] = c;
		matrix[b][a] = c;
	}
	fclose(fp);

	printf("[d]       Display Graph\n");
	printf("[b]       Boruvka Algorithm\n");
	printf("[e]       Exit\n");
	
	for (;;) {
		scanf(" %c", &command);
		switch (command) {
		case 'd':	Disp(n); break;
		case 'b':	Boruvka(n); break;
		case 'e':	Clear(n); return 0;
		}
	}
	Clear(n);
	return 0;
}

/*** Union-Find functions ***/
int *UF_Parent;
int *UF_Rank;

void UF_Init(int n) {
	int i;
	if ((UF_Parent = malloc(sizeof(int) * n)) == NULL) {
		printf("Cannot allocate memory \n"); 
		exit(1);
	}
	if ((UF_Rank = malloc(sizeof(int) * n)) == NULL) {
		printf("Cannot allocate memory \n"); 
		exit(1);
	}
	for (i = 0; i < n; i++) {
		UF_Parent[i] = i;
		UF_Rank[i] = 0;
	}
}

void UF_Union(int a, int b) {
	if (UF_Rank[a] < UF_Rank[b])	UF_Parent[a] = b;
	else				UF_Parent[b] = a;
	if (UF_Rank[a] == UF_Rank[b])	UF_Rank[a]++;
}

int UF_Find(int a) {
	if (UF_Parent[a] == a)	return a;
	else 			return UF_Find(UF_Parent[a]);
}

void UF_Clear(int n) {
	free(UF_Parent);
	free(UF_Rank);
}