#include "..\data_structures\data_structures.h"
#include <stdlib.h>
#include "..\utilities\utilities.h"

struct ResultsList* all_stable_matchings(int n, int* men_preferences, int* women_preferences){
	struct ResultsList* results_list = (struct ResultsList*) malloc(sizeof (struct ResultsList));
	printf("a\n");
	int* top_matching = gale_shapley(n,men_preferences,women_preferences);
	printf("b\n");
	int* inverted_bottom_matching = gale_shapley(n, women_preferences, men_preferences);
	int* bottom_matching = (int*)malloc(sizeof (int) * n);

	int only_one_matching=1;
	for(int i = 0; i < n; i++){
		bottom_matching[inverted_bottom_matching[i]] = i;
	}
	free(inverted_bottom_matching);
	for(int i=0;i<n;i++){
		if(top_matching[i]!=bottom_matching[i]){
			only_one_matching=0;
			break;
		}
	}
	if(only_one_matching){
		for(int i=0;i<n;i++){
			printf("%i\t",top_matching[i]);
		}
		printf("\n");
		for(int i=0;i<n;i++){
			printf("%i\t",bottom_matching[i]);
		}
		printf("\n");
		printf("sono uguali!\n");
		results_list->first = (struct ResultsListElement*) malloc(sizeof (struct ResultsListElement));
		results_list->first->value = top_matching;
		results_list->first->next = NULL;
		results_list->last = results_list->first;
		free(bottom_matching);
		return results_list;
	}
	
	int* top_matching_copy = (int*) malloc(sizeof (int) * n);
	for(int i = 0; i < n; i++){
		top_matching_copy[i] = top_matching[i];
	}
	printf("c\n");
	//crea la lista delle rotazioni
	struct RotationsList* rotations_list = find_all_rotations(men_preferences, women_preferences, n, top_matching_copy);
	printf("d\n");
	struct RotationsListElement* list_el = rotations_list->first;
	struct RotationList* t_el;
	printf("%p != %p (NULL)\n",rotations_list->first->value, NULL);
	while(list_el!=NULL){
		printf("Rotazione %i:\t",list_el->value->index);
		t_el=list_el->value->rotation;
		while(t_el!=NULL){
			printf("(%i, %i), ",t_el->man,t_el->woman);
			t_el=t_el->next;
		}
		printf("\n");
		list_el=list_el->next;
	}
	//crea il grafo delle rotazioni
	build_graph(n, rotations_list, top_matching, men_preferences, women_preferences);
	//calcolo la lista delle rotazioni libere
	struct RotationsList* free_rotations_list = (struct RotationsList*)malloc(sizeof (struct RotationsList));
	free_rotations_list->first=NULL;
	free_rotations_list->last=NULL;
	list_el = rotations_list->first;
	while(list_el!=NULL){
		printf("list_el->value->missing_predecessors: %i\n",list_el->value->missing_predecessors);
		if(list_el->value->missing_predecessors==0){
			appendRotationsList(free_rotations_list,list_el->value);
		}
		list_el=list_el->next;
	}
	///test
	printf("\n");
	list_el = rotations_list->first;
	struct RotationList* le;
	struct SuccessorsList* sl;
	while(list_el!=NULL){
		le=list_el->value->rotation;
		while(le!=NULL){
			printf("(%i,%i), ",le->man,le->woman);
			le=le->next;
		}
		printf("ha indice %i e ",list_el->value->index);
		if(list_el->value->missing_predecessors>0) printf("non ");
		printf("e' una rotazione libera");
		printf(" con %i predecessori e i seguenti successori: ",list_el->value->missing_predecessors);
		sl=list_el->value->successors;
		while(sl!=NULL) {
			printf("%i, ",sl->value->index);
			sl=sl->next;
		}
		printf("\n");
		list_el=list_el->next;
	}

	//test
	
	//aggiungo top matching ai risultati
	results_list->first = (struct ResultsListElement*) malloc(sizeof (struct ResultsListElement));
	for(int i = 0; i < n; i++){ //per non lavorare sul matching salvato tra i risultati
		top_matching_copy[i] = top_matching[i];
	}
	results_list->first->value = top_matching_copy;
	results_list->first->next = NULL;
	results_list->last = results_list->first;
	printf("e\n");

	//testing
	struct RotationsListElement* testing_list = free_rotations_list->first;
	int count = 0;
	while (testing_list!=NULL){
		printf(".");
		count++;
		testing_list=testing_list->next;
	}
	printf("\nABBIAMO TROVATO IN TUTTO %i ROTAZIONI LIBERE\n",count);
	//testing

	if(rotations_list->first != NULL){
		recursive_search(top_matching, n, free_rotations_list->first, results_list);
	}
	printf("f\n");
	
	free(top_matching);
	free_rotations_list_struct(rotations_list);
	free(bottom_matching);
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