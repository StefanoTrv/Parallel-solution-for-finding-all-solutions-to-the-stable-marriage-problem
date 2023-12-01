#include "serial_functions.h"
#include "..\data_structures\data_structures.h"

struct ResultsList* all_stable_matchings(int n, int* men_preferences, int* women_preferences){
	char* top_matching = gale_shapley(n,men_preferences,women_preferences);
	
	char* top_matching_copy = malloc(sizeof (char) * n);
	for(int i=0;i<n;i++){
		top_matching_copy[i]=top_matching[i];
	}
	//crea il grafo delle rotazioni
	struct RotationsList* free_rotations_list = find_all_rotations(men_preferences,women_preferences, n, top_matching_copy);
	
	//aggiungo top matching ai risultati
	struct ResultsList* results_list = malloc(sizeof (struct ResultsList));
	results_list->first = malloc(sizeof (struct ResultsListElement));
	for(int i=0;i<n;i++){ //per non lavorare sul matching salvato tra i risultati
		top_matching_copy[i]=top_matching[i];
	}
	results_list->first->value=top_matching_copy;
	results_list->first->next=NULL;
	results_list->last=results_list->first;
	
	recursive_search(top_matching, n, free_rotations_list, results_list);
	
	return results_list;
}