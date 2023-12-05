#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int read_dimension(FILE* ptr) {
    int n;
    fscanf(ptr, "%d", &n);
    return n;
}


void fill_matrices(FILE* ptr, int n, int* men_preferences, int* women_preferences) {
    fgetc(ptr);
    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++) {
            fscanf(ptr, "%d", &men_preferences[i * n + j]);
        }
    }
    fgetc(ptr);
    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++) {
            fscanf(ptr, "%d", &women_preferences[i * n + j]);
        }
    }
}