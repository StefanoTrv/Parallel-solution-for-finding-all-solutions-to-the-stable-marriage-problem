#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void read_input(FILE* ptr, int* n, int** men_preferences, int** women_preferences) {
    fscanf(ptr, "%d", &n);
    matrix_alloc(n);
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

