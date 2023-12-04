#ifndef INPUT_OUTPUT
#define INPUT_OUTPUT

int read_dimension(FILE*);
void fill_matrices(FILE*, int, int**, int**);
int** matrix_alloc(int);
void matrix_free(int**, int);
void print_matrix(int **, int);
 #endif