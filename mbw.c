#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <math.h>
#include <string.h>

#ifndef BLOCK_SIZE

#define BLOCK_SIZE 4096

#endif

#ifndef RANDOM_MODE

#define RANDOM_MODE 1

#endif

typedef uint64_t Int;

typedef struct Node Node;

struct Node {
	char block[BLOCK_SIZE];
	Node* next;
};

void random_shuffle(Node** list, Int N)
{
	for (Int i=0; i<N-1; ++i) {
		Int swap_ix = i + rand() % (N-i);
		Node* tmp = list[swap_ix];
		list[swap_ix] = list[i];
		list[i] = tmp;
	}
}

// Returns nanoseconds per element.
double write_bench(Int N, Int iters) {

	// Allocate all the memory continuously so we aren't affected by the particulars of the system allocator:
	Node* memory = (Node*)malloc(N * sizeof(Node));
	memset(memory, 0, N * sizeof(Node));

	// Initialize the node pointers:
	Node** nodes = (Node**)malloc(N * sizeof(Node*));
	for (Int i=0; i<N; ++i) {
		nodes[i] = &memory[i];
	}

	// Shuffle the node pointers to mimic random access in the test
	if (RANDOM_MODE)
		random_shuffle(nodes, N);

	// Link up the nodes so that when traversing the nodes, we are traversing in random order.
	for (Int i=0; i<N-1; ++i) {
		nodes[i]->next = nodes[i+1];
	}
	nodes[N-1]->next = NULL;

	// The starting point for list traversal
	Node* start_node = nodes[0];

	// Free up unused memory before meassuring:
	free(nodes);

	// Do the actual measurements:
	Int start = clock();

	char pattern = 97;
	for (Int it=0; it<iters; ++it) {
		// Run through all the nodes:
		Node* node = start_node;
		while (node) {
			memset(node->block, pattern, BLOCK_SIZE);
			node = node->next;
		}
		pattern = 97 + (it % 26);
	}

	Int dur = clock() - start;
	double ns = 1e9 * dur / CLOCKS_PER_SEC;

	free(memory);

	return ns / (N * iters);
}

void printHelp() {
	printf("Usage: ./mbw [-s | --size <memSize> ] [-w | --work <workSize> ]\n");
	printf("-s, --size:\n\tSize of totoal memory foot print in GB\n");
	printf("-w, --work:\n\tWorking set size in MB\n");
	return;
}

int main(int argc, char * argv[])
{
	int opt = 0;
	int index = 0;
	int verbose_flag = 0;
	int size = 2; // unit MB
	int work = 1024; // unit MB

	struct option long_options[] = {
		{ "verbose", no_argument, &verbose_flag, 1 },
		{ "size", required_argument, NULL, 's' },
		{ "work", required_argument, NULL, 'w' },
		{ "help", no_argument, NULL, 'h' },
		{ NULL, 0, NULL, 0 }
	};

	// Parse user input
	while ((opt = getopt_long(argc, argv, "s:w:h", long_options, NULL)) != -1) {
		switch (opt) {
			case 's':
				size = atoi(optarg);
				if (size <= 0) {
					printHelp();
					exit(EXIT_FAILURE);
				}
				break;
			case 'w':
				work = atoi(optarg);
				if (size <= 0) {
					printHelp();
					exit(EXIT_FAILURE);
				}
				break;
			case 'h':
				printHelp();
				break;
			default:
				printHelp();
				exit(EXIT_FAILURE);
		}

	}

	printf("MemSize,BlockSize,Blocks,Latency(nsec),BW(MBps)\n");

	// Size of memory foot print for each bench test
	Int memSize = (Int) size * (1 << 30);
	Int workSize = (Int) work * (1 << 20);

	// Each bench test will traverse to 'memSize' memory footprint in total
	// Each traversal step will sizeof(Node)
	Int elemsPerMeasure = memSize / sizeof(Node);

	Int workElemsFactor = (Int)ceil(log2((double)workSize / sizeof(Node)));

	// Start the stress test infinitely
	double bw = 0;
	double avgLatency = 0;
	while (1) {
		// memSize = N * sizeof(Node) * reps
		Int N = (Int)floor(pow(2.0, (double)workElemsFactor) + 0.5);
		Int reps = (Int)floor((double) elemsPerMeasure / N);
		if (reps<1) reps = 1;

		// Run the benchmark
		avgLatency = write_bench(N, reps);

		// Print the benchmark result
		bw = (double) BLOCK_SIZE * 1e3 / avgLatency / (1.024 * 1.024);
		printf("%lu,%u,%lu,%f,%f\n",
				N*sizeof(Node), BLOCK_SIZE, N, avgLatency, bw);
	}

	return 0;
}
