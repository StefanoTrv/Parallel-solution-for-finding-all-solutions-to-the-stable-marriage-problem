#include "..\data_structures\data_structures.h"
#include <stdlib.h>
#include "..\utilities\utilities.h"

struct ResultsList* all_stable_matchings(int n, int* men_preferences, int* women_preferences){
	printf("a\n");
	char* top_matching = gale_shapley(n,men_preferences,women_preferences);
	printf("b\n");
	
	char* top_matching_copy = (char*) malloc(sizeof (char) * n);
	for(int i=0;i<n;i++){
		top_matching_copy[i]=top_matching[i];
	}
	printf("c\n");
	//crea il grafo delle rotazioni
	struct RotationsList* rotations_list = find_all_rotations(men_preferences,women_preferences, n, top_matching_copy);
	printf("d\n");
	
	//aggiungo top matching ai risultati
	struct ResultsList* results_list = (struct ResultsList*) malloc(sizeof (struct ResultsList));
	results_list->first = (struct ResultsListElement*) malloc(sizeof (struct ResultsListElement));
	for(int i=0;i<n;i++){ //per non lavorare sul matching salvato tra i risultati
		top_matching_copy[i]=top_matching[i];
	}
	results_list->first->value=top_matching_copy;
	results_list->first->next=NULL;
	results_list->last=results_list->first;
	printf("e\n");

	//testing
	struct RotationsListElement* testing_list = rotations_list->first;
	int count = 0;
	while (testing_list!=NULL){
		printf(".");
		count++;
		testing_list=testing_list->next;
	}
	printf("\nABBIAMO TROVATO IN TUTTO %i ROTAZIONI LIBERE\n",count);
	//testing

	if(rotations_list->first!=NULL){
		recursive_search(top_matching, n, rotations_list->first, results_list);
	}
	printf("f\n");
	
	free(top_matching);
	free_rotations_list(rotations_list);
	return results_list;
}