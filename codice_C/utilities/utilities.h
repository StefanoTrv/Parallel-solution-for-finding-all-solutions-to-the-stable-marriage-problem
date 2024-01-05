#ifndef UTILITIES
#define UTILITIES
#include "serial_cleanup_functions.c"
#include "make_random_preferences.c"
#include "compare.c"
#include "input_output.c"

int* make_random_preferences(int);
void free_results(struct ResultsList*);
void free_rotations_list_struct(struct RotationsList*);

int read_dimension(FILE*);
void fill_matrices(FILE*, int, int*, int*);

void compare_solutions(int*, int*, int, struct ResultsList*);

#endif