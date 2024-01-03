#include <stdio.h>
#include "data_structures\data_structures.h"
#include "utilities\utilities.h"
#include "logic\serial_functions.h"
#include "logic\CUDA_functions.cuh"

int main(int argc, char* argv[]) {
    if (argc != 3 && argc != 4) {
        printf("Usage: .\\serial_parallel_solutions_comparator.exe instances_dimension instances_number <seed>");
        return 1;
    }
    int seed, n, iterations;
    if(argc == 4){
        seed = strtol(argv[3], NULL, 10);
    } else {
        seed = 42;
    }

    srand(seed);
    n = strtol(argv[1], NULL, 10);
    iterations = strtol(argv[2], NULL, 10);
    
    int* men_preferences;
    int* women_preferences;
    struct ResultsList* results_serial;
    struct ResultsList* results_parallel;
    struct ResultsListElement* list_el_serial;
    struct ResultsListElement* list_el_parallel;
    FILE* f = fopen("error.txt", "w");

    for(int i = 0; i < iterations; i++) {
        men_preferences = make_random_preferences(n);
        women_preferences = make_random_preferences(n);

        results_serial = all_stable_matchings(n, men_preferences, women_preferences);
        results_parallel = all_stable_matchings_CUDA(n, men_preferences, women_preferences);

        list_el_serial = results_serial->first;
        list_el_parallel = results_parallel->first;

        while(list_el_parallel != NULL) {
            int* matching_serial = list_el_serial->value;
            int* matching_parallel = list_el_parallel->value;
            for (int i = 0; i < n; i++) {
                if (matching_serial[i] != matching_parallel[i]) {
                    list_el_serial = list_el_serial->next;
                    break;
                } else if (matching_serial[i] == matching_parallel[i] && i == n-1) {
                    list_el_parallel = list_el_parallel->next;
                    list_el_serial = results_serial->first;
                    break;
                } 
            }
            if (list_el_serial == NULL) {
                fprintf(f, "men_preferences:\n");
                for (int i = 0; i < n; i++) {
                    for (int j = 0; j < n; j++) {
                        fprintf(f, "%i ", men_preferences[i * n + j]);
                    }
                    fprintf(f, "\n");
                }
                fprintf(f, "\nwomen_preferences:\n");
                for (int i = 0; i < n; i++) {
                    for (int j = 0; j < n; j++) {
                        fprintf(f, "%i ", women_preferences[i * n + j]);
                    }
                    fprintf(f, "\n");
                }
                printf("\nSerial solutions:\n");
                struct ResultsListElement* r = results_serial->first;
                while (r != NULL) {
                    for (int i = 0; i < n; i++) {
                        printf("%i ", r->value[i]);
                    }
                    printf("\n");
                    r = r->next;
                }
                printf("\nParallel solutions:\n");
                struct ResultsListElement* p = results_parallel->first;
                while (p != NULL) {
                    for (int i = 0; i < n; i++) {
                        printf("%i ", p->value[i]);
                    }
                    printf("\n");
                    p = p->next;
                }
                printf("\nError: Matching ");
                for (int i = 0; i < n; i++) {
                    printf("%i ", matching_parallel[i]);
                }
                printf("not found in serial solutions list.");
                printf("\n\n------------------------------------------------\n\n");
                return;
            }
        }
        free(men_preferences);
        free(women_preferences);
        free_results(results_serial);
        free_results(results_parallel);
        printf("\nCompleted test %i of %i", i+1, iterations);
    }
    return 0;
}