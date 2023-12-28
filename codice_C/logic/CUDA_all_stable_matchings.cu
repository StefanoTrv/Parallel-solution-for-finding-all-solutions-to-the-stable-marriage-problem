#include <stdlib.h>
#include <chrono>
#include <stdint.h>
#include "..\utilities\utilities.h"
#include "..\data_structures\data_structures.h"


#define min(i, j) (((i) < (j)) ? (i) : (j))
#define max(i, j) (((i) > (j)) ? (i) : (j))


struct ResultsList* all_stable_matchings_CUDA(int n, int* men_preferences, int* women_preferences){
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

	//SEZIONE PARALLELIZZATA
	//creazione delle strutture dati di input
	int number_of_rotations = 0;
	int total_number_of_pairs = 0;
	struct RotationsListElement* list_el = rotations_list->first;
	struct RotationList* rotation_el;
	while(list_el!=NULL){
		number_of_rotations++;
		rotation_el=list_el->value->rotation;
		while(rotation_el!=NULL){
			total_number_of_pairs++;
			rotation_el=rotation_el->next;
		}
		list_el=list_el->next;
	}

	int* rotations_vector = (int*)malloc(sizeof (int) * total_number_of_pairs * 2);
	int* end_displacement_vector = (int*)malloc(sizeof (int) * number_of_rotations);
	struct RotationNode** rotation_vector = (struct RotationNode**)malloc(sizeof (struct RotationNode*) * number_of_rotations); //per velocizzare il salvataggio dei risultati

	list_el = rotations_list->first;
	int c1,c2;
	while(list_el!=NULL){
		c1 = 0;
		rotation_vector[list_el->value->index]=list_el->value;//riempio rotation_vector
		rotation_el=list_el->value->rotation;
		while(rotation_el!=NULL){//salva tutte le coppie
			rotations_vector[c1]=rotation_el->man;
			rotations_vector[total_number_of_pairs+c1]=rotation_el->woman;
			c1++;
			rotation_el=rotation_el->next;
		}
		c2+=c1-1;
		end_displacement_vector[list_el->value->index]=c2; //il displacement di questa rotazione
		list_el=list_el->next;
	}

	//Alloco memoria page-locked sull'host
	int* triangular_matrix;
	cudaHostAlloc((void**)&triangular_matrix, sizeof (int) * ((n-1)*n)/2, cudaHostAllocDefault); //da vedere se passare a mapped
	
	//Sposto i dati su device
	void* dev_ptr = triangular_matrix; 
	cudaMemcpy(dev_ptr, triangular_matrix, sizeof (int) * ((n-1)*n)/2, cudaMemcpyHostToDevice);

	//lancio del kernel
	int NumThPerBlock = min(max(number_of_rotations, n), 1024);
	build_graph_CUDA<<<1, NumThPerBlock>>>(n, number_of_rotations, rotations_vector, end_displacement_vector,  top_matching, women_preferences, men_preferences, dev_ptr);
	
	//Sposto i risultati sull'host
	cudaMemcpy(triangular_matrix, dev_ptr, sizeof (int) * ((n-1)*n)/2, cudaMemcpyDeviceToHost);

	//libero la memoria sul device
	cudaFree(dev_ptr);

	//applico i risultati alle strutture dati dell'host
	list_el = rotations_list->first;
	int y;
	struct SuccessorsList* sl_el;
	while(list_el!=NULL){
		y=list_el->value->index;
		for(int x = 0; x<y; x++){
			if(triangular_matrix[((y-1)*y)/2+x]){//se y dipende da x
				list_el->value->missing_predecessors++;//incremento il numero di predecessori di y
				//e aggiungo y tra i successori di x
				sl_el=(struct SuccessorsList*)malloc(sizeof (struct SuccessorsList));
				sl_el->value=list_el->value;
				sl_el=rotation_vector[x]->successors;
				rotation_vector[x]->successors=sl_el;
			}
		}
		list_el=list_el->next;
	}

	//libero memoria
	cudaFreeHost(triangular_matrix);
	free(rotations_vector);
	free(end_displacement_vector);
	free(rotation_vector);

	//FINE SEZIONE PARALLELIZZATA

	
	//calcolo la lista delle rotazioni libere
	struct RotationsList* free_rotations_list = (struct RotationsList*)malloc(sizeof (struct RotationsList));
	free_rotations_list->first=NULL;
	free_rotations_list->last=NULL;
	list_el = rotations_list->first;
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



/*struct ResultsList* all_stable_matchings_times_CUDA(int n, int* men_preferences, int* women_preferences, int* time_gale_shapley, int* time_find_all_rotations, int* time_build_graph, int* time_recursive){
	// Time measure
    std::chrono::steady_clock::time_point start_time;
    std::chrono::steady_clock::time_point end_time;

	struct ResultsList* results_list = (struct ResultsList*) malloc(sizeof (struct ResultsList));
	
	start_time = std::chrono::steady_clock::now();
	int* top_matching = gale_shapley(n,men_preferences,women_preferences);
	end_time = std::chrono::steady_clock::now();
	*time_gale_shapley = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

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
	start_time = std::chrono::steady_clock::now();
	struct RotationsList* rotations_list = find_all_rotations(men_preferences, women_preferences, n, top_matching_copy,bottom_matching);
	free(bottom_matching);
	end_time = std::chrono::steady_clock::now();
	*time_find_all_rotations = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

	
	//crea il grafo delle rotazioni
	start_time = std::chrono::steady_clock::now();
	build_graph(n, rotations_list, top_matching, men_preferences, women_preferences);
	end_time = std::chrono::steady_clock::now();
	*time_build_graph = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

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
		start_time = std::chrono::steady_clock::now();
		recursive_search(top_matching, n, free_rotations_list->first, results_list);
		end_time = std::chrono::steady_clock::now();
		*time_recursive = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
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
}*/