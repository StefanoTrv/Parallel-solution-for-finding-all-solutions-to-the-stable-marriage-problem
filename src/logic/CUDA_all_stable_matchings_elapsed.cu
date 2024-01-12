#include <stdlib.h>
#include <chrono>
#include <stdint.h>
#include "..\utilities\utilities.h"
#include "..\utilities\handle_error.cuh"
#include "..\data_structures\data_structures.h"


#define min(i, j) (((i) < (j)) ? (i) : (j))
#define max(i, j) (((i) > (j)) ? (i) : (j))


struct ResultsList* all_stable_matchings_times_CUDA(int n, int* men_preferences, int* women_preferences, int* time_gale_shapley, int* time_find_all_rotations, int* time_overhead, int* time_kernel, int* time_overhead_kernel, int* time_recursive, int* time_total){
	// Time measure
    std::chrono::steady_clock::time_point start_time;
    std::chrono::steady_clock::time_point end_time;
	std::chrono::steady_clock::time_point start_time_kernel;
    std::chrono::steady_clock::time_point end_time_kernel;

	struct ResultsList* results_list = (struct ResultsList*) malloc(sizeof (struct ResultsList));
	
	start_time = std::chrono::steady_clock::now();
	int* top_matching = gale_shapley(n,men_preferences,women_preferences);
	int* inverted_bottom_matching = gale_shapley(n, women_preferences, men_preferences);
	end_time = std::chrono::steady_clock::now();
	*time_gale_shapley = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
	*time_total += *time_gale_shapley;

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
	end_time = std::chrono::steady_clock::now();
	*time_find_all_rotations = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
	*time_total += *time_find_all_rotations;
	free(bottom_matching);

	//crea il grafo delle rotazioni
	start_time = std::chrono::steady_clock::now();

	//SEZIONE PARALLELIZZATA
	//creazione delle strutture dati di input
	start_time = std::chrono::steady_clock::now();
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

	int* rotations_vector;
	HANDLE_ERROR(cudaHostAlloc((void**)&rotations_vector, sizeof (int) * total_number_of_pairs * 2, cudaHostAllocMapped));
	int* end_displacement_vector;
	HANDLE_ERROR(cudaHostAlloc((void**)&end_displacement_vector, sizeof (int) * number_of_rotations, cudaHostAllocMapped));
	struct RotationNode** rotation_vector = (struct RotationNode**)malloc(sizeof (struct RotationNode*) * number_of_rotations); //per velocizzare il salvataggio dei risultati

	list_el = rotations_list->first;
	int c1 = 0;
	int c2 = 0;
	while(list_el!=NULL){
		rotation_vector[list_el->value->index]=list_el->value;//riempio rotation_vector
		rotation_el=list_el->value->rotation;
		while(rotation_el!=NULL){//salva tutte le coppie
			rotations_vector[c1]=rotation_el->man;
			rotations_vector[total_number_of_pairs+c1]=rotation_el->woman;
			c1++;
			rotation_el=rotation_el->next;
		}
		c2=c1-1;
		end_displacement_vector[list_el->value->index]=c2; //il displacement di questa rotazione
		list_el=list_el->next;
	}

	//preparazione per il lancio del kernel
	int* triangular_matrix, *dev_triangular_matrix, *dev_rotations_vector, *dev_end_displacement_vector, *dev_top_matching, *dev_men_preferences, *dev_women_preferences;

	HANDLE_ERROR(cudaHostAlloc((void**)&triangular_matrix, sizeof (int) * ((number_of_rotations-1)*number_of_rotations)/2, cudaHostAllocMapped));
	
	if(number_of_rotations>1){
		(cudaHostGetDevicePointer(&dev_triangular_matrix, triangular_matrix, 0));
	} else{
		dev_triangular_matrix=NULL;
	}
	HANDLE_ERROR(cudaHostGetDevicePointer(&dev_rotations_vector, rotations_vector, 0));
	HANDLE_ERROR(cudaHostGetDevicePointer(&dev_end_displacement_vector, end_displacement_vector, 0));

	HANDLE_ERROR(cudaMalloc((void**)&dev_top_matching, sizeof(int) * n));
	HANDLE_ERROR(cudaMalloc((void**)&dev_men_preferences, sizeof(int) * n * n));
	HANDLE_ERROR(cudaMalloc((void**)&dev_women_preferences, sizeof(int) * n * n));

	HANDLE_ERROR(cudaMemcpy(dev_top_matching, top_matching, sizeof(int) * n, cudaMemcpyHostToDevice));
	HANDLE_ERROR(cudaMemcpy(dev_men_preferences, men_preferences, sizeof(int) * n * n, cudaMemcpyHostToDevice));
	HANDLE_ERROR(cudaMemcpy(dev_women_preferences, women_preferences, sizeof(int) * n * n, cudaMemcpyHostToDevice));

	//lancio del kernel
	int NumThPerBlock = min(max(number_of_rotations, n), 1024);
	size_t free_t,total_t;
	cudaMemGetInfo(&free_t,&total_t);
	cudaDeviceSetLimit(cudaLimitMallocHeapSize, free_t * 0.9);
	start_time_kernel = std::chrono::steady_clock::now();
	build_graph_CUDA<<<1, NumThPerBlock>>>(n, number_of_rotations, total_number_of_pairs, dev_rotations_vector, dev_end_displacement_vector,  dev_top_matching, dev_women_preferences, dev_men_preferences, dev_triangular_matrix);

	//libero memoria
	cudaDeviceSynchronize();
	end_time_kernel = std::chrono::steady_clock::now();
	*time_kernel = std::chrono::duration_cast<std::chrono::milliseconds>(end_time_kernel - start_time_kernel).count();

	HANDLE_ERROR(cudaFree(dev_top_matching));
	HANDLE_ERROR(cudaFree(dev_men_preferences));
	HANDLE_ERROR(cudaFree(dev_women_preferences));

	//applico i risultati alle strutture dati dell'host
	list_el = rotations_list->first;
	int y;
	struct SuccessorsList* sl_el;
	while(list_el!=NULL){
		y=list_el->value->index;
		for(int x = 0; x<y; x++){
			if(triangular_matrix[(y-1)*number_of_rotations+x]){//se y dipende da x
				list_el->value->missing_predecessors++;//incremento il numero di predecessori di y
				//e aggiungo y tra i successori di x
				sl_el=(struct SuccessorsList*)malloc(sizeof (struct SuccessorsList));
				sl_el->value=list_el->value;
				sl_el->next=rotation_vector[x]->successors;
				rotation_vector[x]->successors=sl_el;
			}
		}
		list_el=list_el->next;
	}

	//libero memoria
	HANDLE_ERROR(cudaFreeHost(triangular_matrix));
	HANDLE_ERROR(cudaFreeHost(rotations_vector));
	HANDLE_ERROR(cudaFreeHost(end_displacement_vector));
	free(rotation_vector);

	end_time = std::chrono::steady_clock::now();
	*time_overhead_kernel = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
	*time_overhead = *time_overhead_kernel - *time_kernel;
	*time_total += *time_overhead_kernel;
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
		start_time = std::chrono::steady_clock::now();
		recursive_search(top_matching, n, free_rotations_list->first, results_list);
		end_time = std::chrono::steady_clock::now();
		*time_recursive = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
		*time_total += *time_recursive;
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