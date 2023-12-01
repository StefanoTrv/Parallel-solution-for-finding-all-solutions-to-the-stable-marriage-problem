#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include "input_output.h"
#include "logic\serial_functions.h"
#include "utilities\utilities.h"

/**
 * @return milliseconds
 */
uint64_t get_now_time() {
  struct timespec spec;
  if (clock_gettime(1, &spec) == -1) { /* 1 is CLOCK_MONOTONIC */
    abort();
  }

  return spec.tv_sec * 1000 + spec.tv_nsec / 1e6;
}


int main(int argc, char* argv[]) {
    if (argc != 3 && argc!=4) {
        printf("Usage: .\\tester.exe instances_dimension instances_number output_file <seed>");
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
    FILE* file = fopen(argv[3], "r");
    if (file == NULL) {
        printf("Input file cannot be opened.");
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

        start_time = get_now_time;
        //results = all_stable_matchings(n, men_preferences, women_prefereces);
        end_time = get_now_time;

        number_of_results = 0;
        list_el = results->first;
        while(list_el!=NULL){
            number_of_results++;
            list_el=list_el->next;
        }

        time = end_time-start_time;
        fprintf(file, "%i\t%i\t%i\n", time, number_of_results, n);

        free(men_preferences);
        free(women_preferences);
        //free_results(results);
        printf("Completed test %i of %i",i+1,iterations);
    }

    fclose(file);
    return 0;
}