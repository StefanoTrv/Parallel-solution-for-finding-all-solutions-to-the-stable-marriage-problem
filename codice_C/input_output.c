#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int test(int**, int, int, int, int);
void fill_matrices(FILE*, int, int**, int**);
int* gale_shapley(int, int**, int**);
int** matrix_alloc(int);
void matrix_free(int**, int);

void fill_matrices(FILE* ptr, int n, int** men_preferences, int** women_preferences) {
    fgetc(ptr);
    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            fscanf(ptr, "%d", &men_preferences[i][j]);
        }
    }
    fgetc(ptr);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            fscanf(ptr, "%d", &women_preferences[i][j]);
        }
    }
}


int test(int** women_preferences, int n, int w, int m, int m1) {
    for (int i = 0; i < n; i++) {
        if (women_preferences[w][i] == m) {
            return 1;
        }
        if (women_preferences[w][i] == m1) {
            return 0;
        }
    }
    return 0;
}


int** matrix_alloc(int n) {
    int** matrix = malloc(n * sizeof(int *));
    for(int i = 0 ; i < n ; i++) {
        matrix[i] = malloc(n * sizeof(int));
    }
    return matrix;
}

void matrix_free(int** matrix, int n) {
    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

