// A C++ program for Dijkstra's single source shortest path algorithm.
// The program is for adjacency matrix representation of the graph

#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>
// Number of vertices in the graph
#define V 9999

// A utility function to find the vertex with minimum distance value, from
// the set of vertices not yet included in shortest path tree
int minDistance(int dist[], bool sptSet[], int thread_count)
{
	// Initialize min value
	int min = INT_MAX, min_index;
	int v;
	int my_min[thread_count];
	int idx[thread_count];
	//init my_min, not need to use parallel for
	for(v = 0; v < thread_count; v++) {
		my_min[v] = INT_MAX;
	}

# pragma omp parallel for num_threads(thread_count) 
	for (v = 0; v < V; v++) {
		int id = omp_get_thread_num();
		if (sptSet[v] == false && dist[v] <= my_min[id]) {
			my_min[id] = dist[v];
			idx[id] = v;
		}
	}
	// find the minimum
	for(v = 0; v < thread_count; v++) {
		if(min>=my_min[v]) {
			min=my_min[v];
			min_index=idx[v];
		}
	}

	return min_index;
}

// A utility function to print the constructed distance array
void printSolution(int dist[], int n) {
	printf("Vertex Distance from Source\n");
	int i;
	for (i = 0; i < V; i++)
		printf("%d \t %d\n", i, dist[i]);
}

// Function that implements Dijkstra's single source shortest path algorithm
// for a graph represented using adjacency matrix representation
void dijkstra(int** graph, int src, int thread_count)
{
	int dist[V];	 // The output array. dist[i] will hold the shortest
					// distance from src to i

	bool sptSet[V]; // sptSet[i] will true if vertex i is included in shortest
					// path tree or shortest distance from src to i is finalized

	// Initialize all distances as INFINITE and stpSet[] as false
    int i;
# pragma omp parallel for num_threads(thread_count)
	for (i = 0; i < V; i++)
		dist[i] = INT_MAX, sptSet[i] = false;

	// Distance of source vertex from itself is always 0
	dist[src] = 0;

	// Find shortest path for all vertices
    int count;
//# pragma omp parallel for num_threads(thread_count)
	for (count = 0; count < V-1; count++) {
		// Pick the minimum distance vertex from the set of vertices not
		// yet processed. u is always equal to src in the first iteration.
		int u = minDistance(dist, sptSet, thread_count);

		// Mark the picked vertex as processed
		sptSet[u] = true;

		// Update dist value of the adjacent vertices of the picked vertex.
		int v;
# pragma omp parallel for num_threads(thread_count)
		for (v = 0; v < V; v++)
			// Update dist[v] only if is not in sptSet, there is an edge from 
			// u to v, and total weight of path from src to v through u is 
			// smaller than current value of dist[v]
			if (!sptSet[v] && graph[u][v] && dist[u] != INT_MAX 
										&& dist[u]+graph[u][v] < dist[v])
				dist[v] = dist[u] + graph[u][v];
	}

	// print the constructed distance array
//	printSolution(dist, V);
}

// driver program to test above function
int main(int argc, char* argv[]) {
	/* Let us create the example graph discussed above */
	int thread_count = strtol(argv[1], NULL, 10); 
	/*int graph[V][V] = {{0, 4, 0, 0, 0, 0, 0, 8, 0},
						{4, 0, 8, 0, 0, 0, 0, 11, 0},
						{0, 8, 0, 7, 0, 4, 0, 0, 2},
						{0, 0, 7, 0, 9, 14, 0, 0, 0},
						{0, 0, 0, 9, 0, 10, 0, 0, 0},
						{0, 0, 4, 14, 10, 0, 2, 0, 0},
						{0, 0, 0, 0, 0, 2, 0, 1, 6},
						{8, 11, 0, 0, 0, 0, 1, 0, 7},
						{0, 0, 2, 0, 0, 0, 6, 7, 0}
						};
	*/
	int **graph;
	int i, j;
	graph=(int**)malloc(sizeof(int*)*V);  
    for(i=0;i<V;i++)  
    graph[i]=(int*)malloc(sizeof(int)*V);
//	srand((unsigned)time(NULL));
	for (i = 0; i < V; i++)
		for (j = i + 1; j < V; j++)
			graph[i][j] = rand() % 100;
	for (i = V - 1; i >= 0; i--)
		for (j = 0; j < i; j++)
			graph[i][j] = graph[j][i];
	for (i = 0; i < V; i++)
		graph[i][i] = 0;		

	double start = omp_get_wtime();
	dijkstra(graph, 0, thread_count);
	double finish = omp_get_wtime();
	printf("time: %f\n", (finish - start));

	for (i = 0; i < V; i++)
		free(graph[i]);
	free(graph);
    return 0;
}