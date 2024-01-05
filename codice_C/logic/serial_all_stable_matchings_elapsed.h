#ifndef SERIAL_ALL_STABLE_MATCHINGS_ELAPSED
#define SERIAL_ALL_STABLE_MATCHINGS_ELAPSED
#include "serial_all_stable_matchings_elapsed.c"

struct ResultsList* all_stable_matchings_times(int n, int*, int*, int*, int*, int*, int*);
struct ResultsList* all_stable_matchings_times_CUDA(int, int*, int*, int*, int*, int*, int*, int*);

#endif