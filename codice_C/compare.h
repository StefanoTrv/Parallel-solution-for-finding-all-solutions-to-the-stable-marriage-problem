#ifndef COMPARE
#define COMPARE
#include "compare.c"


int is_matching_stable(int*, int*, int*, int);
void swap(int*, int*);
void generate_permutations(int*, int*, struct StablePermutations*, int*, int, int); 
void all_permutations(int*, int*, int, struct StablePermutations*);
void compare_solutions(int*, int*, int, struct ResultsList*);


#endif