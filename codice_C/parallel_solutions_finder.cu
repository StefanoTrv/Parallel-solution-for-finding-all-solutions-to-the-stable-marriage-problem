#include <stdio.h>
#include "logic\parallel_functions.cuh"
#include "data_structures\data_structures.h"
#include "utilities\utilities.h"
#include "input_output.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: .\\parallel_solutions_finder.exe input_file output_file");
        return 1;
    }
    FILE* in_ptr = fopen(argv[1], "r");
    if (in_ptr == NULL) {
        printf("Input file cannot be opened.");
        return 1;
    }
    FILE* out_ptr = fopen(argv[2], "w");
    if (out_ptr == NULL) {
        printf("Output file cannot be opened.");
        return 1;
    }

    int n = read_dimension(in_ptr);
	int* men_preferences = (int*)malloc(sizeof (int) * n * n);
    int* women_preferences = (int*)malloc(sizeof (int) * n * n); 
    fill_matrices(in_ptr, n, men_preferences, women_preferences);
    fclose(in_ptr);

    struct ResultsList* results;
    struct ResultsListElement* list_el;
    
    results = all_stable_matchings_CUDA(n, men_preferences, women_preferences);

    list_el = results->first;
    while(list_el!=NULL){
        for (int i = 0; i < n; i++) {
            printf("%i, ", list_el->value[i]);
            fprintf(out_ptr, "%i ", list_el->value[i]);
        }
        printf("\n");
        fprintf(out_ptr, "\n");
        list_el=list_el->next;
    }

    free(men_preferences);
    free(women_preferences);
    free_results(results);

    fclose(out_ptr);
    return 0;
}