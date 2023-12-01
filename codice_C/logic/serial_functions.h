#ifndef SERIAL_FUNCTIONS
#define SERIAL_FUNCTIONS
#include "serial_functions.c"
#include "serial_all_stable_matchings.c"

char* gale_shapley(int, int*, int*);
struct RotationsList* find_all_rotations(int*, int*, int, char*);
void recursive_search(char*, int, struct RotationsListElement*, struct ResultsList*);

struct ResultsList* all_stable_matchings(int n, int*, int*);
#endif