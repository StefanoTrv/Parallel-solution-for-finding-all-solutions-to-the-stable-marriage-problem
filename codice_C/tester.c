#include <stdio.h>
#include <sys/time.h>
#include <stdint.h>
#include "logic\serial_functions.h"
#include "data_structures\data_structures.h"
#include "utilities\utilities.h"
#include "compare.h"

/**
* @brief provide same output with the native function in java called
* currentTimeMillis().
*/
int64_t currentTimeMillis() {
  struct timeval time;
  gettimeofday(&time, NULL);
  int64_t s1 = (int64_t)(time.tv_sec) * 1000;
  int64_t s2 = (time.tv_usec / 1000);
  return s1 + s2;
}


int main(int argc, char* argv[]) {
    if (argc != 4 && argc!=5) {
        printf("Usage: .\\tester.exe instances_dimension instances_number output_file <seed>");
        return 1;
    }
    int seed, n, iterations;
    printf("%i\n\n\n",argc);
    if(argc == 5){
        seed = strtol(argv[4], NULL, 10);
    } else {
        seed = 42;
    }
    srand(seed);
    n = strtol(argv[1], NULL, 10);
    iterations = strtol(argv[2], NULL, 10);
    FILE* file = fopen(argv[3], "w");
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
    for(int i =0; i<iterations;i++){
        men_preferences = make_random_preferences(n);
        women_preferences = make_random_preferences(n);

        printf("\n1\n");

        start_time = currentTimeMillis();
        results = all_stable_matchings(n, men_preferences, women_preferences);
        printf("1.5\n");
        end_time = currentTimeMillis();

        printf("2\n");

        number_of_results = 0;
        list_el = results->first;
        while(list_el!=NULL){
            number_of_results++;
            list_el=list_el->next;
        }

        time = end_time-start_time;
        fprintf(file, "%i\t%i\t%i\n", time, number_of_results, n);

        /* CHECK PER CORRETTEZZA DELLA SOLUZIONE */
        compare_solutions(men_preferences, women_preferences, n, results);


        free(men_preferences);
        free(women_preferences);
        free_results(results);
        printf("\nCompleted test %i of %i",i+1,iterations);
    }

    fclose(file);
    return 0;
}