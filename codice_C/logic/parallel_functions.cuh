#ifndef PARALLEL_FUNCTIONS
#define PARALLEL_FUNCTIONS
#include "serial_functions.c"
#include "CUDA_functions.cu"
#include "CUDA_all_stable_matchings.cu"

int* gale_shapley(int, int*, int*);
struct RotationsList* find_all_rotations(int*, int*, int, int*, int*);
void build_graph(int, struct RotationsList*, int*, int*, int*);
void recursive_search(int*, int, struct RotationsListElement*, struct ResultsList*);

__global__ void build_graph_CUDA(int, int, int*, int*, int*, int*, int*, int*);

struct ResultsList* all_stable_matchings_CUDA(int n, int*, int*);
//struct ResultsList* all_stable_matchings_times_CUDA(int n, int*, int*, int*, int*, int*, int*);

#endif