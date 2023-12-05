#include <stdio.h>
#include <sys/time.h>
#include <stdint.h>
#include "logic\serial_functions.h"
#include "data_structures\data_structures.h"
#include "utilities\utilities.h"
#include "input_output.h"

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
    if (argc != 3) {
        printf("Usage: .\\serial_solutions_finder.exe input_file output_file");
        return 1;
    }
    FILE* in_ptr = fopen(argv[1], "r");
    if (in_ptr == NULL) {
        printf("Input file cannot be opened.");
        return 1;
    }

    int n = read_dimension(in_ptr);
	int* men_preferences = (int*)malloc(sizeof (int) * n * n);
    int* women_preferences = (int*)malloc(sizeof (int) * n * n); 
    fill_matrices(in_ptr, n, men_preferences, women_preferences);
    fclose(in_ptr);
    
    FILE* out_ptr = fopen(argv[2], "w");
    if (out_ptr == NULL) {
        printf("Output file cannot be opened.");
        return 1;
    }
    int number_of_results, time;
    uint64_t start_time;
    uint64_t end_time;
    struct ResultsList* results;
    struct ResultsListElement* list_el;
    
    start_time = currentTimeMillis();
    results = all_stable_matchings(n, men_preferences, women_preferences);
    end_time = currentTimeMillis();

    number_of_results = 0;
    list_el = results->first;
    while(list_el!=NULL){
        number_of_results++;
        list_el=list_el->next;
    }

    time = end_time-start_time;
    fprintf(out_ptr, "%i\t%i\t%i\n", time, number_of_results, n);

    free(men_preferences);
    free(women_preferences);
    free_results(results);

    fclose(out_ptr);
    return 0;
}