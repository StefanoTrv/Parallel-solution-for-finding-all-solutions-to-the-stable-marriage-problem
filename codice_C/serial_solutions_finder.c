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