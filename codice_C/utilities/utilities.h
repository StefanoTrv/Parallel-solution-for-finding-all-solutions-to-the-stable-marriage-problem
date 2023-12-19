#ifndef UTILITIES
#define UTILITIES
#include "serial_cleanup_functions.c"
#include "make_random_preferences.c"

int* make_random_preferences(int);
void free_results(struct ResultsList*);
void free_rotations_list_struct(struct RotationsList*);

#endif