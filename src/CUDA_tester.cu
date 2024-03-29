#include <stdio.h>
#include <string.h>
#include <chrono>
#include "logic\CUDA_functions.cuh"
#include "data_structures\data_structures.h"
#include "utilities\utilities.h"

int main(int argc, char* argv[]) {
    if (argc != 5 && argc != 6) {
        printf("Usage: .\\tester.exe instances_dimension instances_number mode output_file <seed>");
        return 1;
    }
    if (strcmp(argv[3], "classic") != 0 && strcmp(argv[3], "times") != 0) {
        printf("Mode must be \"classic\" or \"times\"");
        return 1;
    }
    int seed, n, iterations;
    if(argc == 6){
        seed = strtol(argv[5], NULL, 10);
    } else {
        seed = 42;
    }
    srand(seed);
    n = strtol(argv[1], NULL, 10);
    iterations = strtol(argv[2], NULL, 10);
    FILE* file = fopen(argv[4], "w");
    if (file == NULL) {
        printf("Output file cannot be opened.");
        return 1;
    }

    int* men_preferences;
    int* women_preferences;
    int number_of_results, time;
    
    struct ResultsList* results;
    struct ResultsListElement* list_el;

    int time_gale_shapley = 0; 
    int time_find_all_rotations = 0; 
    int time_overhead = 0;
    int time_kernel = 0;
    int time_overhead_kernel = 0; 
    int time_recursive_search = 0;
    int time_total;

    if (strcmp(argv[3], "times") == 0) {
        fprintf(file, "GaleShapley FindAllRotations Overhead Kernel KernelOverhead RecursiveSearch Total\n");
    }

    for(int i = 0; i < iterations; i++){
        men_preferences = make_random_preferences(n);
        women_preferences = make_random_preferences(n);

        time_total = 0;

        if (strcmp(argv[3], "times") == 0) {
            results = all_stable_matchings_times_CUDA(n, men_preferences, women_preferences, &time_gale_shapley, &time_find_all_rotations, &time_overhead, &time_kernel, &time_overhead_kernel, &time_recursive_search, &time_total);
        } else {
            // Time measure
            std::chrono::steady_clock::time_point start_time;
            std::chrono::steady_clock::time_point end_time;
	        start_time = std::chrono::steady_clock::now();
            results = all_stable_matchings_CUDA(n, men_preferences, women_preferences);
	        end_time = std::chrono::steady_clock::now();
            time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();;
        }

        number_of_results = 0;
        list_el = results->first;
        while(list_el != NULL){
            number_of_results++;
            list_el = list_el->next;
        }

        if (strcmp(argv[3], "times") == 0) {
            fprintf(file, "%i %i %i %i %i %i %i\n", time_gale_shapley, time_find_all_rotations, time_overhead, time_kernel, time_overhead_kernel, time_recursive_search, time_total);
        } else {
            fprintf(file, "%i %i %i\n", time, number_of_results, n);
        }

        free(men_preferences);
        free(women_preferences);
        free_results(results);
        printf("\nCompleted test %i of %i", i + 1, iterations);
    }

    fclose(file);
    fclose(file);
    printf("\n");
    return 0;
}