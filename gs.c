#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int test(int**, int, int, int, int);
void fill_matrices(FILE*, int, int**, int**);
int* gale_shapley(int, int**, int**);
int** matrix_alloc(int);
void matrix_free(int**, int);


int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: .\\gs.exe filename");
        return 1;
    }
    FILE* ptr = fopen(argv[1], "r");
    if (ptr == NULL) {
        printf("Input file cannot be opened.");
        return 1;
    }

    int n;
    fscanf(ptr, "%d", &n);
    int** men_preferences = matrix_alloc(n);
    int** women_preferences = matrix_alloc(n);

    fill_matrices(ptr, n, men_preferences, women_preferences);
    fclose(ptr);

    int* top_matching = gale_shapley(n, men_preferences, women_preferences);
    for (int i = 0; i < n; i++) {
        printf("%d %d\n", i, top_matching[i]);
    }

    matrix_free(men_preferences, n);
    matrix_free(women_preferences, n);
    return 0;
}


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


int* gale_shapley(int n, int** men_preferences, int** women_preferences) {
    int women_partners[n], men_free[n];
    for (int i = 0; i < n; i++) {
        women_partners[i] = -1;
        men_free[i] = 1;
    }

    while (1) {
        int m = -1;
        for (int i = 0; i < n; i++) {
            if (men_free[i]){
                m = i;
                break;
            }
        }
        if (m == -1) {
            break;
        }

        for (int i = 0; i < n && men_free[m]; i++) {
            int w = men_preferences[m][i];
            if (women_partners[w] == -1) {
                women_partners[w] = m;
                men_free[m] = 0;
            } else {
                int m1 = women_partners[w];
                if (test(women_preferences, n, w, m, m1)) {
                    women_partners[w] = m;
                    men_free[m] = 0;
                    men_free[m1] = 1;
                }
            }
        }
    }
    int* matching = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        matching[women_partners[i]] = i;
    }
    return matching;
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

