#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "..\data_structures\data_structures.h"

int is_matching_stable(int*, int*, int*, int);
void swap(int*, int*);
void generate_permutations(int*, int*, struct ResultsList*, int*, int, int); 
void all_permutations(int*, int*, int, struct ResultsList*);
void compare_solutions(int*, int*, int, struct ResultsList*);


int is_matching_stable(int* men_preferences, int* women_preferences, int *matching, int n) {
    for (int i = 0; i < n; i++) {
        int man = i;
        int actualWoman = matching[man];

        if (actualWoman != -1) {
            //Finds women that man prefers that actual woman
            int *preferredWomen = &men_preferences[man * n];
            int indexActualWoman = -1;
            for (int j = 0; j < n; j++) {
                if (preferredWomen[j] == actualWoman) {
                    indexActualWoman = j;
                    break;
                }
            }

            //Checks if woman prefers man that actual man        
            for (int j = 0; j < indexActualWoman; j++) {
                int woman = preferredWomen[j];
                int actualManOfWoman = -1;

                //Finds actual man of woman
                for (int k = 0; k < n; k++) {
                    if (matching[k] == woman) {
                        actualManOfWoman = k;
                        break;
                    }
                }
                for (int k = 0; k < n; k++) {
                    if (women_preferences[woman*n + k] == man) {
                        return 0;
                    }
                    if (women_preferences[woman*n + k] == actualManOfWoman) {
                        break;
                    }
                }
            }
        }
    }
    return 1;
}


void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}


void generate_permutations(int* men_preferences, int* women_preferences, struct ResultsList* perms, int* array, int start, int end) {
    if (start == end-1) {
        if (is_matching_stable(men_preferences, women_preferences, array, end)) {
            int* matching = (int*) malloc(sizeof(int) * end);
            for (int i = 0; i < end; i++) {
                matching[i] = array[i];
            }
            appendResultsList(perms, matching);
        }
    } else {
        for (int i = start; i < end; i++) {
            //Swap elements to generate all possible permutations
            swap(&array[start], &array[i]);
            generate_permutations(men_preferences, women_preferences, perms, array, start + 1, end);
            //Restore original state to find other permutations
            swap(&array[start], &array[i]);
        }
    }
}


void all_permutations(int* men_preferences, int* women_preferences, int n, struct ResultsList* stable_perms) {
    int* array = (int*) malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++) {
        array[i] = i;
    }
    generate_permutations(men_preferences, women_preferences, stable_perms, array, 0, n);
    free(array);
}


void compare_solutions(int* men_preferences, int* women_preferences, int n, struct ResultsList* results) {
    struct ResultsList* stable_perms = (struct ResultsList*) malloc(sizeof(struct ResultsList));
    stable_perms->first = NULL;
    stable_perms->last = NULL;

    all_permutations(men_preferences, women_preferences, n, stable_perms);

    struct ResultsListElement* res_list_el = results->first;
    struct ResultsListElement* stable_perms_list_el = stable_perms->first;

    while(stable_perms_list_el != NULL) {
        int* matching_res = res_list_el->value;
        int* matching_perm = stable_perms_list_el->value;
        for (int i = 0; i < n; i++) {
            if (matching_res[i] != matching_perm[i]) {
                res_list_el = res_list_el->next;
                break;
            } else if (matching_res[i] == matching_perm[i] && i == n-1) {
                stable_perms_list_el = stable_perms_list_el->next;
                res_list_el = results->first;
            } 
        }
        if (res_list_el == NULL) {
            printf("men_preferences:\n");
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    printf("%i ", men_preferences[i * n + j]);
                }
                printf("\n");
            }
            printf("\nwomen_preferences:\n");
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    printf("%i ", women_preferences[i * n + j]);
                }
                printf("\n");
            }
            printf("\nOur solutions:\n");
            struct ResultsListElement* r = results->first;
            while (r != NULL) {
                for (int i = 0; i < n; i++) {
                    printf("%i ", r->value[i]);
                }
                printf("\n");
                r = r->next;
            }
            printf("\nNaive solutions:\n");
            struct ResultsListElement* p = stable_perms->first;
            while (p != NULL) {
                for (int i = 0; i < n; i++) {
                    printf("%i ", p->value[i]);
                }
                printf("\n");
                p = p->next;
            }
            printf("\nError: Matching ");
            for (int i = 0; i < n; i++) {
                printf("%i ", matching_perm[i]);
            }
            printf("not found in our solutions list.");
            printf("\n\n------------------------------------------------\n\n");
            return;
        }
    }
}