#include "..\data_structures\data_structures.h"
#include <stdlib.h>
#include <sys/time.h>
#include "..\utilities\utilities.h"

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

struct ResultsList* all_stable_matchings(int n, int* men_preferences, int* women_preferences, int* time_gale_shapley, int* time_find_all_rotations, int* time_build_graph, int* time_recursive){
	uint64_t start_time;
    uint64_t end_time;
    
	struct ResultsList* results_list = (struct ResultsList*) malloc(sizeof (struct ResultsList));
	
	start_time = currentTimeMillis();
	int* top_matching = gale_shapley(n,men_preferences,women_preferences);
	end_time = currentTimeMillis();
	*time_gale_shapley = end_time - start_time;	

	int* inverted_bottom_matching = gale_shapley(n, women_preferences, men_preferences);
	int* bottom_matching = (int*)malloc(sizeof (int) * n);
	for(int i = 0; i < n; i++){
		bottom_matching[inverted_bottom_matching[i]] = i;
	}
	free(inverted_bottom_matching);

	//termina subito se non ci sono rotazioni
	int only_one_matching=1;
	for(int i=0;i<n;i++){
		if(top_matching[i]!=bottom_matching[i]){
			only_one_matching=0;
			break;
		}
	}
	if(only_one_matching){
		results_list->first = (struct ResultsListElement*) malloc(sizeof (struct ResultsListElement));
		results_list->first->value = top_matching;
		results_list->first->next = NULL;
		results_list->last = results_list->first;
		free(bottom_matching);
		return results_list;
	}
	
	//copia top_matching
	int* top_matching_copy = (int*) malloc(sizeof (int) * n);
	for(int i = 0; i < n; i++){
		top_matching_copy[i] = top_matching[i];
	}

	//crea la lista delle rotazioni
	start_time = currentTimeMillis();
	struct RotationsList* rotations_list = find_all_rotations(men_preferences, women_preferences, n, top_matching_copy,bottom_matching);
	free(bottom_matching);
	end_time = currentTimeMillis();
	*time_find_all_rotations = end_time - start_time;
	
	//crea il grafo delle rotazioni
	start_time = currentTimeMillis();
	build_graph(n, rotations_list, top_matching, men_preferences, women_preferences);
	end_time = currentTimeMillis();
	*time_build_graph = end_time - start_time;

	//calcolo la lista delle rotazioni libere
	struct RotationsList* free_rotations_list = (struct RotationsList*)malloc(sizeof (struct RotationsList));
	free_rotations_list->first=NULL;
	free_rotations_list->last=NULL;
	struct RotationsListElement* list_el = rotations_list->first;
	while(list_el!=NULL){
		if(list_el->value->missing_predecessors==0){
			appendRotationsList(free_rotations_list,list_el->value);
		}
		list_el=list_el->next;
	}
	
	//aggiungo top matching ai risultati
	results_list->first = (struct ResultsListElement*) malloc(sizeof (struct ResultsListElement));
	for(int i = 0; i < n; i++){ //per non lavorare sul matching salvato tra i risultati
		top_matching_copy[i] = top_matching[i];
	}
	results_list->first->value = top_matching_copy;
	results_list->first->next = NULL;
	results_list->last = results_list->first;

	if(rotations_list->first != NULL){
		start_time = currentTimeMillis();	
		recursive_search(top_matching, n, free_rotations_list->first, results_list);
		end_time = currentTimeMillis();
		*time_recursive = end_time - start_time;
	}
	
	free(top_matching);
	free_rotations_list_struct(rotations_list);
	list_el=free_rotations_list->first;
	struct RotationsListElement* temp;
	while(list_el!=NULL){
		temp=list_el;
		list_el=list_el->next;
		free(temp);
	}
	free(free_rotations_list);
	return results_list;
}