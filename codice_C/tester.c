#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "logic\serial_functions.h"
#include "logic\serial_all_stable_matchings_elapsed.h"
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
    uint64_t start_time;
    uint64_t end_time;
    struct ResultsList* results;
    struct ResultsListElement* list_el;

    int time_gale_shapley = 0; 
    int time_find_all_rotations = 0; 
    int time_build_graph = 0; 
    int time_recursive_search = 0;

    if (strcmp(argv[3], "times") == 0) {
        fprintf(file, "Gale-Shapley\tFind All Rotations\tBuild Graph\tRecursive Search\n");
    }

    for(int i = 0; i < iterations; i++){
        men_preferences = make_random_preferences(n);
        women_preferences = make_random_preferences(n);

        if (strcmp(argv[3], "times") == 0) {
            results = all_stable_matchings_times(n, men_preferences, women_preferences, &time_gale_shapley, &time_find_all_rotations, &time_build_graph, &time_recursive_search);
        } else {
            start_time = currentTimeMillis();
            results = all_stable_matchings(n, men_preferences, women_preferences);
            end_time = currentTimeMillis();
            time = end_time-start_time;
        }

        number_of_results = 0;
        list_el = results->first;
        while(list_el != NULL){
            number_of_results++;
            list_el = list_el->next;
        }

        if (strcmp(argv[3], "times") == 0) {
            fprintf(file, "%i\t\t\t\t%i\t\t\t\t\t%i\t\t\t%i\n", time_gale_shapley, time_find_all_rotations, time_build_graph, time_recursive_search);
        } else {
            fprintf(file, "%i\t%i\t%i\n", time, number_of_results, n);
        }

        free(men_preferences);
        free(women_preferences);
        free_results(results);
        printf("\nCompleted test %i of %i", i + 1, iterations);
    }

    fclose(file);
    fclose(file);
    return 0;
}