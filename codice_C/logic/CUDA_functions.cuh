#ifndef PARALLEL_FUNCTIONS
#define PARALLEL_FUNCTIONS
#include "CUDA_functions.cu"
#include "CUDA_all_stable_matchings.cu"
#include "CUDA_all_stable_matchings_elapsed.cu"

__global__ void build_graph_CUDA(int, int, int*, int*, int*, int*, int*, int*);

struct ResultsList* all_stable_matchings_CUDA(int n, int*, int*);
struct ResultsList* all_stable_matchings_times_CUDA(int n, int*, int*, int*, int*, int*, int*, int*, int*, int*);

#endif