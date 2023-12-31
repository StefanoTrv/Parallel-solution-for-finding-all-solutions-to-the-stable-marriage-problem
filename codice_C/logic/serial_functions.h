#ifndef SERIAL_FUNCTIONS
#define SERIAL_FUNCTIONS
#include "serial_functions.c"
#include "serial_all_stable_matchings.c"

int* gale_shapley(int, int*, int*);
struct RotationsList* find_all_rotations(int*, int*, int, int*, int*);
void build_graph(int, struct RotationsList*, int*, int*, int*);
void recursive_search(int*, int, struct RotationsListElement*, struct ResultsList*);

struct ResultsList* all_stable_matchings(int n, int*, int*);

#endif