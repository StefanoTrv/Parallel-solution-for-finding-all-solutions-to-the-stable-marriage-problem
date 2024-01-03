#ifndef UTILITIES
#define UTILITIES
#include "serial_cleanup_functions.c"
#include "make_random_preferences.c"
#include "compare.c"

int* make_random_preferences(int);
void free_results(struct ResultsList*);
void free_rotations_list_struct(struct RotationsList*);

void compare_solutions(int*, int*, int, struct ResultsList*);

#endif