#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/time.h>
#define BUF_SIZE 80

struct node {
	int id;
	int weight;
	struct node *next;
};

struct node **list;
int *key, *prev, *visited;

void Insert(int a, int b, int weight) {
	struct node *p;
	/* Add b to list-a */
	if ((p = malloc(sizeof(struct node))) == NULL) {
		printf("Cannot allocate memory \n"); exit(1);
	}
	p->weight = weight;
	p->id = b;
	p->next = list[a];
	list[a] = p;
	/* Add a to list-b */
	if ((p = malloc(sizeof(struct node))) == NULL) {
		printf("Cannot allocate memory \n"); exit(1);
	}
	p->weight = weight;
	p->id = a;
	p->next = list[b];
	list[b] = p;
}

void Disp(int size) {
	int i;
	struct node *p;
	for (i = 0; i < size; i++) {
		printf("Node %d : ", i);
		for (p = list[i]; p != NULL; p = p->next)
			printf("%d ", p->id);
		printf("\n");
	}
}

void Clear(int size) {
	int i;
	struct node *p;
	for (i = 0; i < size; i++) {
		for (p = list[i]; p != NULL; ) {
			struct node *q = p;
			p = p->next;
			free(q);
		}
	}
	free(list);
}

/*** Heap functions ***/
void PQ_Init(int size, int *key);
void PQ_Clear();
void PQ_Update(int v);
int  PQ_RemoveMin();
int  PQ_Empty();
/**********************/

void PrimPQ_func(int p, int size) {
	int i;
	for (i = 0; i < size; i++) {
		key[i] = INT_MAX;
		prev[i] = -1;
		visited[i] = 0;
	}
	key[p] = 0;
	PQ_Init(size, key);
	while (!PQ_Empty()) {
		struct node *n;
		int u = PQ_RemoveMin();
		visited[u] = 1;
		for (n = list[u]; n != NULL; n = n->next) {
			int v = n->id;
			int w = n->weight;
			if (!visited[v] && w < key[v]) {
				key[v] = w;
				prev[v] = u;
				PQ_Update(v);
			}
		}
	}
	for (i = 0; i < size; i++) {
		int j;
		if (i != p) {
			printf("%d ", i);
			for (j = prev[i]; j != p; j = prev[j]) printf("%d ", j); 
		}
		printf("%d\n", p);
	}

	printf("\nMST edges (selected by Prim's algorithm):\n");
	for (i = 0; i < size; i++) {
		if (i != p && prev[i] != -1) {
			int from = prev[i];
			int to = i;
			int weight = -1;
			struct node *n;
			for (n = list[from]; n != NULL; n = n->next) {
				if (n->id == to) {
					weight = n->weight;
					break;
				}
			}
			if (weight != -1)
				printf("Edge: (%d - %d), Weight: %d\n", from, to, weight);
			else
				printf("Edge: (%d - %d), Weight: not found\n", from, to);
		}
	}
	int total_weight = 0;
	int edge_count = 0;
	for (i = 0; i < size; i++) {
		if (prev[i] != -1) {
			total_weight += key[i];
			edge_count++;
		}
	}
	printf("MST Total Weight: %d\n", total_weight);
	printf("MST Edge Count  : %d\n", edge_count);

	PQ_Clear();
}

void PrimPQ(int size) {
	int root = 0;
	struct timeval stime, etime;
	double sec;
	if ((key = malloc(sizeof(int) * size)) == NULL) {
		printf("Cannot allocate memory \n"); exit(1);
	}
	if ((prev = malloc(sizeof(int) * size)) == NULL) {
		printf("Cannot allocate memory \n"); exit(1);
	}
	if ((visited = malloc(sizeof(int) * size)) == NULL) {
		printf("Cannot allocate memory \n"); exit(1);
	}
	gettimeofday(&stime, NULL);
	PrimPQ_func(root, size);
	gettimeofday(&etime, NULL);
	sec = (etime.tv_sec - stime.tv_sec) +
		(etime.tv_usec - stime.tv_usec) / 1000000.0;
	printf("Elapsed Time %.8f [sec] \n", sec / (double)size);
	printf("Total Time %.8f [sec] \n", sec);
	free(key);
	free(prev);
	free(visited);
}

int main(int argc, char *argv[]) {
	FILE *fp;
	int i, n;
	char buf[BUF_SIZE];
	char command;
	if (argc != 2) {
		printf("Usage: %s <filename>\n", argv[0]); exit(1);
	}
	if ((fp = fopen(argv[1], "r")) == NULL) {
		printf("Cannot open file (%s) \n", argv[1]); exit(1);
	}
	fgets(buf, BUF_SIZE, fp);
	sscanf(buf, "%d", &n);
	if ((list = malloc(sizeof(struct node*) * n)) == NULL) {
		printf("Cannot allocate memory \n"); exit(1);
	}
	for (i = 0; i < n; i++)
		list[i] = NULL;
	while (fgets(buf, BUF_SIZE, fp) != NULL) {
		int a, b, c;
		sscanf(buf, "%d %d %d", &a, &b, &c);
		Insert(a, b, c);
	}
	fclose(fp);
	printf("[d]       Display Graph\n");
	printf("[p]       Prim Algorithm (Priority Queue Version)\n");
	printf("[e]       Exit\n");
	for (;;) {
		scanf(" %c", &command);
		switch (command) {
		case 'd':	Disp(n); break;
		case 'p':	PrimPQ(n); break;
		case 'e':	Clear(n); return 0;
		}
	}
	Clear(n);
	return 0;
}

int *Heap;
int *Heap_Index;
int *Heap_Key;
int Heap_Size = 0;

void downheap(int k){
	int v = Heap[k];
	for (;;) {
		int c = 2 * k + 1;
		if (c >= Heap_Size) break;
		if (c + 1 < Heap_Size && Heap_Key[Heap[c]] > Heap_Key[Heap[c + 1]]) c++;
		if (Heap_Key[v] <= Heap_Key[Heap[c]]) break;
		Heap[k] = Heap[c]; Heap_Index[Heap[c]] = k; k = c;
	}
	Heap[k] = v; Heap_Index[v] = k;
}

void upheap(int k){
	int v = Heap[k];
	for (;;) {
		int p = (k - 1) / 2;
		if (k < 1 || Heap_Key[Heap[p]] <= Heap_Key[v]) break;
		Heap[k] = Heap[p]; Heap_Index[Heap[p]] = k; k = p;
	}
	Heap[k] = v; Heap_Index[v] = k;
}

void PQ_Init(int size, int *key) {
	int k;
	Heap_Size = size;
	Heap_Key = key;
	if ((Heap = malloc(sizeof(int) * Heap_Size)) == NULL) {
		printf("Cannot allocate memory \n"); exit(1);
	}
	if ((Heap_Index = malloc(sizeof(int) * Heap_Size)) == NULL) {
		printf("Cannot allocate memory \n"); exit(1);
	}
	for (k = 0; k < Heap_Size; k++) {
		Heap[k] = k; Heap_Index[k] = k;
	}
	for (k = Heap_Size / 2 - 1; k >= 0; k--) downheap(k);
}

void PQ_Clear() {
	free(Heap);
	free(Heap_Index);
}

int PQ_Empty() {
	return (Heap_Size == 0);
}

int PQ_RemoveMin() {
	int min = Heap[0];
	int last = Heap[Heap_Size - 1];
	Heap[0] = last; Heap_Index[last] = 0;
	Heap_Size--;
	downheap(0);
	return min;
}

void PQ_Update(int v) {
	upheap(Heap_Index[v]);
} 

void PQ_Disp(int size) {
	int i;
	for (i = 0; i < Heap_Size; i++) printf("%d ", Heap[i]);
	printf("\n");
}
