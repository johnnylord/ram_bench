#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <math.h>
#include <string.h>

#define BLOCK_SIZE 4096

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
double read_bench(Int N, Int iters) {
	// Allocate all the memory continuously so we aren't affected by the particulars of the system allocator:
	Node* memory = (Node*)malloc(N * sizeof(Node));

	// Initialize the node pointers:
	Node** nodes = (Node**)malloc(N * sizeof(Node*));
	for (Int i=0; i<N; ++i) {
		nodes[i] = &memory[i];
	}

	// Shuffle the node pointers to mimic random access in the test
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

	for (Int it=0; it<iters; ++it) {
		// Run through all the nodes:
		Node* node = start_node;
		while (node) {
			node = node->next;
		}
	}

	Int dur = clock() - start;
	double ns = 1e9 * dur / CLOCKS_PER_SEC;

	free(memory);

	return ns / (N * iters);
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
	printf("Usage: ./benchmark [-m | --mode <mode> ]\n");
	printf("-m, --mode:\n\t1 stands for read benchmark, 2 stands for read_write benchmark\n");
	return;
}

int main(int argc, char * argv[])
{
	int opt = 0;
	int index = 0;
	int verbose_flag = 0;
	int size = 2; // unit GB
	int mode = 1; // 1: read_bench, 2: read_write_bench

	struct option long_options[] = {
		{ "verbose", no_argument, &verbose_flag, 1 },
		{ "mode", required_argument, NULL, 'm' },
		{ "size", required_argument, NULL, 's' },
		{ "help", no_argument, NULL, 'h' },
		{ NULL, 0, NULL, 0 }
	};

	// Parse user input
	while ((opt = getopt_long(argc, argv, "s:m:h", long_options, NULL)) != -1) {
		switch (opt) {
			case 'm':
				mode = atoi(optarg);
				if (mode != 1 && mode != 2) {
					printHelp();
					exit(EXIT_FAILURE);
				}
				break;
			case 's':
				size = atoi(optarg);
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
	if (mode == 1) {
		printf("# Start memory read benchmark\n");
	} else {
		printf("# Start memory write benchmark\n");
	}

	// Outputs data in gnuplot friendly .data format
	printf("#bytes    ns/elem\n");

	// Size of memory foot print for each bench test (4GB by default)
	Int memSize = (Int) size * (1 << 30);

	// Each bench test will traverse to 2GB memory footprint in total
	// Each traversal step will sizeof(Node)
	Int elemsPerMeasure = memSize / sizeof(Node);

	// Related to the number of Nodes for each round of the benchmark
	Int stepPerFactor = 4;
	Int minElemsFactor = 1;
	Int maxElemsFactor = (Int)ceil(log2((double)memSize / sizeof(Node)));

	// Run the benchmark with different number of elements Node
	Int min = stepPerFactor * minElemsFactor;
	Int max = stepPerFactor * maxElemsFactor;

	double avgReadLatency = 0;
	for (Int ei=min; ei<=max; ++ei) {
		// memSize = N * sizeof(Node) * reps
		Int N = (Int)floor(pow(2.0, (double)ei / stepPerFactor) + 0.5);
		Int reps = (Int)floor((double) elemsPerMeasure / N);
		if (reps<1) reps = 1;

		// Run the benchmark
		if (mode == 1) {
			avgReadLatency = read_bench(N, reps);
		} else if (mode == 2) {
			avgReadLatency = write_bench(N, reps);
		}

		// Print the benchmark result
		printf("%lu   %f   # (N=%lu, reps=%lu) %lu/%lu\n",
				N*sizeof(Node), avgReadLatency, N, reps, ei-min+1, max-min+1);
	}
}
