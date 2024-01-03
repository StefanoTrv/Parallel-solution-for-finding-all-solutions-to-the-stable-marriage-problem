#include <stdlib.h>
#include "..\utilities\utilities.h"
#include "..\data_structures\data_structures.h"


struct ResultsList* all_stable_matchings(int n, int* men_preferences, int* women_preferences){
	struct ResultsList* results_list = (struct ResultsList*) malloc(sizeof (struct ResultsList));
	int* top_matching = gale_shapley(n,men_preferences,women_preferences);
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
	struct RotationsList* rotations_list = find_all_rotations(men_preferences, women_preferences, n, top_matching_copy, bottom_matching);
	free(bottom_matching);
	
	//crea il grafo delle rotazioni
	build_graph(n, rotations_list, top_matching, men_preferences, women_preferences);

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

	//TEST
	/*struct RotationsListElement* lel = rotations_list->first;
	struct RotationList* le;
	struct SuccessorsList* sl;
	printf("\nGRAFO SERIALE:\n");
	while(lel!=NULL){
		le=lel->value->rotation;
		while(le!=NULL){
			printf("(%i,%i), ",le->man,le->woman);
			le=le->next;
		}
		printf("ha indice %i e ",lel->value->index);
		if(lel->value->missing_predecessors>0) printf("non ");
		printf("e' una rotazione libera");
		if(lel->value->missing_predecessors>0){
			printf(" con %i predecessori: ",lel->value->missing_predecessors);
			sl=lel->value->successors;
			printf(" con %i predecessori e i seguenti successori: ",lel->value->missing_predecessors);
			sl=lel->value->successors;
			while(sl!=NULL) {
				printf("%i, ",sl->value->index);
				sl=sl->next;
			}
		}
		printf("\n");
		lel = lel->next;
	}
	printf("\n\n");*/
	//TEST
	
	//aggiungo top matching ai risultati
	results_list->first = (struct ResultsListElement*) malloc(sizeof (struct ResultsListElement));
	for(int i = 0; i < n; i++){ //per non lavorare sul matching salvato tra i risultati
		top_matching_copy[i] = top_matching[i];
	}
	results_list->first->value = top_matching_copy;
	results_list->first->next = NULL;
	results_list->last = results_list->first;

	if(rotations_list->first != NULL){
		recursive_search(top_matching, n, free_rotations_list->first, results_list);
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