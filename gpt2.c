#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define INF INT_MAX
#define NIL -1

// 구조체 정의
typedef struct {
    int vertex;
    int distance;
} MinHeapNode;

typedef struct {
    int size;
    int capacity;
    int *pos;
    MinHeapNode **array;
} MinHeap;

typedef struct {
    int dest;
    int weight;
    struct AdjListNode *next;
} AdjListNode;

typedef struct {
    AdjListNode *head;
} AdjList;

typedef struct {
    int num_vertices;
    AdjList *array;
} Graph;

// 함수 선언
MinHeapNode* newMinHeapNode(int v, int dist);
MinHeap* createMinHeap(int capacity);
void swapMinHeapNode(MinHeapNode** a, MinHeapNode** b);
void minHeapify(MinHeap* minHeap, int idx);
int isEmpty(MinHeap* minHeap);
MinHeapNode* extractMin(MinHeap* minHeap);
void decreaseKey(MinHeap* minHeap, int v, int dist);
int isInMinHeap(MinHeap *minHeap, int v);
Graph* createGraph(int num_vertices);
void addEdge(Graph* graph, int src, int dest, int weight);
void dijkstra(Graph* graph, int src);

// 함수 정의
MinHeapNode* newMinHeapNode(int v, int dist) {
    MinHeapNode* minHeapNode = (MinHeapNode*) malloc(sizeof(MinHeapNode));
    minHeapNode->vertex = v;
    minHeapNode->distance = dist;
    return minHeapNode;
}

MinHeap* createMinHeap(int capacity) {
    MinHeap* minHeap = (MinHeap*) malloc(sizeof(MinHeap));
    minHeap->pos = (int *)malloc(capacity * sizeof(int));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (MinHeapNode**) malloc(capacity * sizeof(MinHeapNode*));
    return minHeap;
}

void swapMinHeapNode(MinHeapNode** a, MinHeapNode** b) {
    MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

void minHeapify(MinHeap* minHeap, int idx) {
    int smallest, left, right;
    smallest = idx;
    left = 2 * idx + 1;
    right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->distance < minHeap->array[smallest]->distance)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right]->distance < minHeap->array[smallest]->distance)
        smallest = right;

    if (smallest != idx) {
        MinHeapNode *smallestNode = minHeap->array[smallest];
        MinHeapNode *idxNode = minHeap->array[idx];

        minHeap->pos[smallestNode->vertex] = idx;
        minHeap->pos[idxNode->vertex] = smallest;

        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);

        minHeapify(minHeap, smallest);
    }
}

int isEmpty(MinHeap* minHeap) {
    return minHeap->size == 0;
}

MinHeapNode* extractMin(MinHeap* minHeap) {
    if (isEmpty(minHeap))
        return NULL;

    MinHeapNode* root = minHeap->array[0];
    MinHeapNode* lastNode = minHeap->array[minHeap->size - 1];
    minHeap->array[0] = lastNode;

    minHeap->pos[root->vertex] = minHeap->size-1;
    minHeap->pos[lastNode->vertex] = 0;

    --minHeap->size;
    minHeapify(minHeap, 0);

    return root;
}

void decreaseKey(MinHeap* minHeap, int v, int dist) {
    int i = minHeap->pos[v];
    minHeap->array[i]->distance = dist;

    while (i && minHeap->array[i]->distance < minHeap->array[(i - 1) / 2]->distance) {
        minHeap->pos[minHeap->array[i]->vertex] = (i-1)/2;
        minHeap->pos[minHeap->array[(i-1)/2]->vertex] = i;
        swapMinHeapNode(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);

        i = (i - 1) / 2;
    }
}

int isInMinHeap(MinHeap *minHeap, int v) {
    if (minHeap->pos[v] < minHeap->size)
        return 1;
    return 0;
}

Graph* createGraph(int num_vertices) {
    Graph* graph = (Graph*) malloc(sizeof(Graph));
    graph->num_vertices = num_vertices;
    graph->array = (AdjList*) malloc(num_vertices * sizeof(AdjList));
    for (int i = 0; i < num_vertices; ++i)
        graph->array[i].head = NULL;
    return graph;
}

void addEdge(Graph* graph, int src, int dest, int weight) {
    AdjListNode* newNode = (AdjListNode*) malloc(sizeof(AdjListNode));
    newNode->dest = dest;
    newNode->weight = weight;
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;
}

void dijkstra(Graph* graph, int src) {
    int num_vertices = graph->num_vertices;
    int dist[num_vertices];
    int pred[num_vertices];
    MinHeap* minHeap = createMinHeap(num_vertices);

    for (int v = 0; v < num_vertices; ++v) {
        dist[v] = INF;
        pred[v] = NIL;
        minHeap->array[v] = newMinHeapNode(v, dist[v]);
        minHeap->pos[v] = v;
    }

    minHeap->array[src] = newMinHeapNode(src, dist[src]);
    minHeap->pos[src] = src;
    dist[src] = 0;
    decreaseKey(minHeap, src, dist[src]);

    minHeap->size = num_vertices;

    while (!isEmpty(minHeap)) {
        MinHeapNode* minHeapNode = extractMin(minHeap);
        int u = minHeapNode->vertex;

        AdjListNode* crawl = graph->array[u].head;
        while (crawl != NULL) {
            int v = crawl->dest;

            if (isInMinHeap(minHeap, v) && dist[u] != INF && crawl->weight + dist[u] < dist[v]) {
                dist[v] = dist[u] + crawl->weight;
                pred[v] = u;
                decreaseKey(minHeap, v, dist[v]);
            }
            crawl = crawl->next;
        }
    }

    for (int i = 0; i < num_vertices; ++i)
        printf("%d\t%d\t%d\n", i, dist[i], pred[i]);
}

int main() {
    int num_vertices, num_edges, source;
    scanf("%d %d %d", &num_vertices, &num_edges, &source);

    Graph* graph = createGraph(num_vertices);

    for (int i = 0; i < num_edges; ++i) {
        int src, dest, weight;
        scanf("%d %d %d", &src, &dest, &weight);
        addEdge(graph, src, dest, weight);
    }

    dijkstra(graph, source);
    scanf("%d",&num_edges);

    return 0;
}
