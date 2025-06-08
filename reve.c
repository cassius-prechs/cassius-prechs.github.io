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
	int deleted;
};

int **matrix;
struct edge *edges;

int my_compare_desc(const struct edge *a, const struct edge *b) {
	return (a->weight < b->weight) ? 1 : (a->weight > b->weight) ? -1 : 0;
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

/*** Graph connectivity check functions ***/
int is_connected(int size, int edge_count) {
	int i;
	int *visited;
	int *queue;
	int front = 0, rear = 0;
	int visited_count = 0;
	
	/* Allocate memory for visited array and queue */
	if ((visited = malloc(sizeof(int) * size)) == NULL) {
		printf("Cannot allocate memory \n"); 
		exit(1);
	}
	if ((queue = malloc(sizeof(int) * size)) == NULL) {
		printf("Cannot allocate memory \n"); 
		exit(1);
	}
	
	/* Initialize visited array */
	for (i = 0; i < size; i++) {
		visited[i] = 0;
	}
	
	/* Start BFS from node 0 */
	queue[rear++] = 0;
	visited[0] = 1;
	visited_count = 1;
	
	while (front < rear) {
		int current = queue[front++];
		
		/* Check all remaining (non-deleted) edges */
		for (i = 0; i < edge_count; i++) {
			if (edges[i].deleted) continue;  // 削除された辺はスキップ
			
			int neighbor = -1;
			if (edges[i].a == current && !visited[edges[i].b]) {
				neighbor = edges[i].b;
			} else if (edges[i].b == current && !visited[edges[i].a]) {
				neighbor = edges[i].a;
			}
			
			if (neighbor != -1) {
				visited[neighbor] = 1;
				queue[rear++] = neighbor;
				visited_count++;
			}
		}
	}
	
	free(visited);
	free(queue);
	
	return (visited_count == size);
}

void ReverseDelete_func(int size, int edge_num) {
	int i, j, k;
	int total_weight = 0;
	int mst_edge_count = 0;
	
	/* Initialize the edge array */
	k = 0;
	for (i = 0; i < size; i++)
		for (j = 0; j < size; j++)
			if (matrix[i][j] > 0 && i < j) {  // 重複辺除去（無向グラフ）
				edges[k].weight = matrix[i][j];
				edges[k].added = 0;
				edges[k].deleted = 0;
				edges[k].a = i; 
				edges[k].b = j; 
				k++;
			}
	
	printf("Total edges: %d\n", k);
	
	/* Sort all edges in descending order of weight : O(E log E) */
	qsort(edges, k, sizeof(struct edge),
			(int (*)(const void *, const void *))my_compare_desc);
	
	/* Reverse-Delete Algorithm */
	for (i = 0; i < k; i++) {
		printf("Checking edge (%d,%d) with weight %d: ", 
			   edges[i].a, edges[i].b, edges[i].weight);
		
		/* Temporarily delete the edge */
		edges[i].deleted = 1;
		
		if (!is_connected(size, k)) {
			/* If disconnected, restore the edge (keep it in MST) */
			edges[i].deleted = 0;
			edges[i].added = 1;
			printf("kept\n");
		} else {
			printf("deleted\n");
		}
		/* If still connected, keep the edge deleted */
	}
	
	/* Display MST edges and calculate total weight */
	printf("\nMST edges:\n");
	for (i = 0; i < k; i++) {
		if (edges[i].added) {
			printf("edge(%d,%d) is added\n", edges[i].a, edges[i].b);
			total_weight += edges[i].weight;
			mst_edge_count++;
		}
	}
	
	printf("MST Total Weight: %d\n", total_weight);
	printf("MST Edge Count  : %d\n", mst_edge_count);
}

void ReverseDelete(int size) {
	int i, j, edge_num = 0;
	struct timeval stime, etime;
	double sec;
	
	/* Count the number of edges */
	for (i = 0; i < size; i++)
		for (j = i + 1; j < size; j++)  // 修正: 重複カウントを避ける
			if (matrix[i][j] > 0) edge_num++;
	
	/* Allocate memory for the edge array */
	if ((edges = malloc(sizeof(struct edge) * edge_num)) == NULL) {
		printf("Cannot allocate memory \n"); 
		exit(1);
	}

	gettimeofday(&stime, NULL);
	ReverseDelete_func(size, edge_num);
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
	printf("[r]       Reverse-Delete Algorithm\n");
	printf("[e]       Exit\n");
	
	for (;;) {
		scanf(" %c", &command);
		switch (command) {
		case 'd':	Disp(n); break;
		case 'r':	ReverseDelete(n); break;
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
	int root_a = UF_Find(a);
	int root_b = UF_Find(b);
	
	if (root_a != root_b) {
		if (UF_Rank[root_a] < UF_Rank[root_b])
			UF_Parent[root_a] = root_b;
		else if (UF_Rank[root_a] > UF_Rank[root_b])
			UF_Parent[root_b] = root_a;
		else {
			UF_Parent[root_b] = root_a;
			UF_Rank[root_a]++;
		}
	}
}

int UF_Find(int a) {
	if (UF_Parent[a] != a)
		UF_Parent[a] = UF_Find(UF_Parent[a]);  // Path compression
	return UF_Parent[a];
}

void UF_Clear(int n) {
	free(UF_Parent);
	free(UF_Rank);
}