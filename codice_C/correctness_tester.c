#include <stdio.h>
#include <stdint.h>
#include "logic\serial_functions.h"
#include "data_structures\data_structures.h"
#include "utilities\utilities.h"
#include "compare.h"

int main(int argc, char* argv[]) {
    if (argc != 3 && argc != 4) {
        printf("Usage: .\\tester.exe instances_dimension instances_number <seed>");
        return 1;
    }
    int seed, n, iterations;
    if(argc == 4){
        seed = strtol(argv[4], NULL, 10);
    } else {
        seed = 42;
    }
    
    srand(seed);
    n = strtol(argv[1], NULL, 10);
    iterations = strtol(argv[2], NULL, 10);
    
    int* men_preferences;
    int* women_preferences;
    struct ResultsList* results;
    struct ResultsListElement* list_el;
    for(int i = 0; i < iterations; i++) {
        men_preferences = make_random_preferences(n);
        women_preferences = make_random_preferences(n);

        results = all_stable_matchings(n, men_preferences, women_preferences);
        compare_solutions(men_preferences, women_preferences, n, results);

        free(men_preferences);
        free(women_preferences);
        free_results(results);
        printf("\nCompleted test %i of %i", i+1, iterations);
    }

    return 0;
}