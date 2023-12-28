#include <stdlib.h>
#include "..\data_structures\data_structures.h"

#define false 0
#define true 1

int* gale_shapley(int, int*, int*);
int accept_proposal(int*, int, int, int, int);
struct RotationsList* find_all_rotations(int*, int*, int, int*, int*);
void breakmarriage(int*, int, int, int*, int*, int*, int*, struct RotationsList*, int*, int);
void pause_breakmarriage(int*, int*, int*, int*, struct RotationsList*, int, int, int*);
void build_graph(int, struct RotationsList*, int*, int*, int*);
void recursive_search(int*, int, struct RotationsListElement*, struct ResultsList*);


int* gale_shapley(int n, int* men_preferences, int* women_preferences) {
	int* women_partners = (int*)malloc(n * sizeof(int));
    int* men_free = (int*)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        women_partners[i] = -1;
        men_free[i] = true;
    }

    while (true) {
		//cerco il primo uomo libero m
        int m = -1;
        for (int i = 0; i < n; i++) {
            if (men_free[i]){
                m = i;
                break;
            }
        }
        if (m == -1) {
            break;
        }

		//itero sulle preferenze di m
        for (int i = 0; i < n && men_free[m]; i++) {
            int w = men_preferences[m * n + i];
			if (women_partners[w] == -1) {
                women_partners[w] = m;
                men_free[m] = false;
            } else {
				//w non libera, prendo m1 (attuale) e lo confronto con m
                int m1 = women_partners[w];
				//se w preferisce m a m1, sciolgo (w, m1) e creo (w, m)
                if (accept_proposal(women_preferences, n, w, m, m1)) {
                    women_partners[w] = m;
                    men_free[m] = false;
                    men_free[m1] = true;
                }
            }
        }
    }
    int* matching = (int*)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        matching[women_partners[i]] = i;
    }
	free(women_partners);
	free(men_free);
    return matching;
}


//True se w accetta la proposta di m, False se preferisce rimanere con m1 
int accept_proposal(int* women_preferences, int n, int w, int m, int m1) {
    for (int i = 0; i < n; i++) {
        if (women_preferences[w * n + i] == m) {
            return true;
        }
        if (women_preferences[w * n + i] == m1) {
            return false;
        }
    }
    return 0;
}


struct RotationsList* find_all_rotations(int* men_preferences, int* women_preferences, int n, int* top_matching, int* bottom_matching) {
	struct RotationsList* rotations_list = (struct RotationsList*) malloc(sizeof (struct RotationsList));
	rotations_list->first = NULL;
	rotations_list->last = NULL;
	int* m_i = (int*) malloc(sizeof (int) * n);
	
	int* marking = (int*) malloc(sizeof (int) * n); //-1 unmarked, n marked ma non associata, altri sono la donna precedente
	int rotation_index = 0; //per indicizzare le rotationi

	for (int j = 0; j < n; j++) {
		m_i[j] = top_matching[j];
		marking[j] = -1;
	}
	int* men_preferences_indexes = (int*) malloc(sizeof (int) * n);

	//escludiamo le preferenze sicuramente non presenti in un matrimonio stabile
	for (int j = 0; j < n; j++) {
		int k = 0; 
		while (men_preferences[j * n + k] != m_i[j]) {
			k++;
		}
		men_preferences_indexes[j] = k;//quelle attuali verranno scartate successivamente
	}

	int old_m = 0;
	while (true) {
		//STEP 1
		int m = -1;
		//troviamo il primo uomo diverso tra m_i e bottom_matching
		for (int j = old_m; j < n; j++) {
			if (m_i[j] != bottom_matching[j]) {
				m = j; 
				old_m = j;
				break;
			}
		}

		if (m < 0) { //m_i == bottom_matching
			break;
		}
		
		//STEP 2
		int w = m_i[m];
		marking[w] = n;
		breakmarriage(m_i, m, n, men_preferences, men_preferences_indexes, women_preferences, marking, rotations_list, &rotation_index, w);
	}
	free(m_i);
	free(marking);
	free(men_preferences_indexes);
	return rotations_list;
}


void breakmarriage(int* M, int m, int n, int* men_preferences, int* men_preferences_indexes, int* women_preferences, int* marking,
				   struct RotationsList* rotations_list, int* rotation_index, int first_woman) {
	int former_wife = M[m]; //il w dell'articolo, donna con cui l'uomo è accoppiato e si deve separare
	int i;
	int* reversed_M = (int*)malloc(sizeof (int) * n);
	int* old_reversed_M = (int*)malloc(sizeof (int) * n);
	for (i = 0; i < n; i++) {
		reversed_M[M[i]] = i;
		old_reversed_M[M[i]] = i;
	}
	int w, m1, breakmarriage_fail, k, old_marking;
	int previous_woman = first_woman;


	while(true) {
		men_preferences_indexes[m] += 1;//tolgo dalle preferenze la donna con cui m era fidanzato
		breakmarriage_fail = true;
		//itero sulle preferenze di m
		//m diventa m' dell'articolo all'interno del ciclo
		for (i = men_preferences_indexes[m]; i < n; i++) {
			w = men_preferences[m * n + i]; //il w dell'articolo
			//prendo m1 (attuale compagno di w) e lo confronto con m
			m1 = reversed_M[w];
			//se w preferisce m a m1, sciolgo (w, m1) e creo (w, m)
			if (marking[w] < 0 && accept_proposal(women_preferences, n, w, m, m1)) { //step 2a
				k = men_preferences_indexes[m]; //aggiorniamo indice
				while (men_preferences[m * n + k] != w) {
					k++;
				}
				men_preferences_indexes[m]=k;
				reversed_M[w] = m;
				marking[w] = previous_woman;
				previous_woman = w;
				m = m1; //nuovo uomo non accoppiato
				breakmarriage_fail = false;
				break;
			} else if (marking[w] >= 0 && accept_proposal(women_preferences, n, w, m, old_reversed_M[w])) { //step 2b
				breakmarriage_fail = false;
				old_marking = marking[w];
				reversed_M[w] = m;
				pause_breakmarriage(marking, M, reversed_M, old_reversed_M, rotations_list, w, previous_woman, rotation_index);
				reversed_M[w] = m1;
				
				k = men_preferences_indexes[m]; //aggiorniamo indice
				while (men_preferences[m * n + k] != w) {
					k++;
				}
				men_preferences_indexes[m]=k;

				if (former_wife == w) { //3c: w = w'
					reversed_M[w] = m;

					free(reversed_M);
					free(old_reversed_M);
					return; //al passo 1
				} else {//3d
					if (!accept_proposal(women_preferences, n, w, m, m1)) {
						marking[w] = old_marking;
						previous_woman = w;
						continue;//al passo 2
					} else {
						reversed_M[w] = m;
						m = m1;
						previous_woman = M[m];
						break;
					}
				}
			}
		}
		if (breakmarriage_fail) { //non abbiamo trovato un accoppiamento stabile per m (non deve mai accadere)
			printf("ERRORE! FALLIMENTO DI BREAKMARRIAGE!");
			free(reversed_M);
			free(old_reversed_M);
			return;
		}
	}
}


void pause_breakmarriage(int* marking, int* M, int* reversed_M, int* old_reversed_M, struct RotationsList* rotations_list, int w, 
					     int previous_woman, int* rotation_index) {
	struct RotationList* prev_list_el = NULL;
	int w2 = w;
	int go_on = true;
	struct RotationNode* rotation_node = (struct RotationNode*)malloc(sizeof (struct RotationNode));
	rotation_node->missing_predecessors = 0;
	rotation_node->index = *rotation_index;
	rotation_node->successors = NULL;
	*rotation_index += 1;
	struct RotationList* list_el = NULL;
	
	while(w2 != w || go_on) {
		go_on = false;
		//costruire lista della rotazione dalla coda alla testa
		list_el = (struct RotationList*)malloc(sizeof (struct RotationList));
		list_el->man = old_reversed_M[w2];
		list_el->woman = w2;
		list_el->next = prev_list_el;
		prev_list_el = list_el;

		//aggiorniamo old_reversed_M (i = i+1)
		old_reversed_M[w2] = reversed_M[w2];
		//aggiorna M
		M[old_reversed_M[w2]] = w2;
		//resettare marking
		marking[w2] = -1;
		//update previous_woman
		w2 = previous_woman;
		previous_woman = marking[w2];
	}

	rotation_node->rotation = prev_list_el;
	appendRotationsList(rotations_list, rotation_node);

	return;
}


void build_graph(int n, struct RotationsList* rotations_list, int* top_matching, int* men_preferences, int* women_preferences){
	struct RotationsListElement* rotations_list_element = rotations_list->first;
	struct RotationNode** label_matrix = (struct RotationNode**)malloc(sizeof (struct RotationNode*) * n * n);
	int* is_stable_matrix = (int*)malloc(sizeof (int) * n * n);
	int* label_second_condition = (int*)malloc(sizeof (int) * n * n);
	int* last_labelled_woman_index = (int*)malloc(sizeof (int) * n);
	int* last_labelled_man_index = (int*)malloc(sizeof (int) * n);
	int* applied_rotations = (int*)malloc(sizeof (int) * (rotations_list->last->value->index + 1));
	struct RotationList* pair_list_element;
	int man, woman,index, k, first_woman, next_woman;
	struct RotationNode* p_star;
	struct SuccessorsList* new_successor;

	for (int i=0; i<n; i++){
		for(int j=0; j<n; j++){
			label_matrix[i*n+j]=NULL;
			is_stable_matrix[j*n+i]=false;
			label_second_condition[j*n+i]=false;
		}
		woman = top_matching[i];
		last_labelled_woman_index[woman]=n-1;
		while(women_preferences[woman*n+last_labelled_woman_index[woman]]!=i){
			last_labelled_woman_index[woman]--;
		}
		last_labelled_man_index[i]=0;
		while(men_preferences[i*n+last_labelled_man_index[i]]!=woman){
			last_labelled_man_index[i]++;
		}
		is_stable_matrix[top_matching[i]*n+i]=true;
	}

	for (int i=0; i<rotations_list->last->value->index+1; i++){
		applied_rotations[i]=false;
	}

	while(rotations_list_element!=NULL){
		pair_list_element = rotations_list_element->value->rotation;
		first_woman=pair_list_element->woman;
		while(pair_list_element!=NULL){
			man=pair_list_element->man;
			if(pair_list_element->next==NULL){
				next_woman=first_woman;
			}else{
				next_woman=pair_list_element->next->woman;
			}
			woman=pair_list_element->woman;
			//aggiorna rispetto alla donna
			index=last_labelled_woman_index[next_woman]-1;
			while(women_preferences[next_woman*n+index]!=man){
				label_matrix[next_woman*n+women_preferences[next_woman*n+index]]=rotations_list_element->value;
				index--;
			}
			last_labelled_woman_index[next_woman]=index;
			//aggiorna rispetto all'uomo
			index=last_labelled_man_index[man]+1;
			while(men_preferences[man*n+index]!=next_woman){
				label_second_condition[man*n+men_preferences[man*n+index]]=true;
				index++;
			}
			last_labelled_man_index[man]=index;

			is_stable_matrix[next_woman*n+man]=true;
			label_matrix[woman*n+man]=rotations_list_element->value;
			pair_list_element=pair_list_element->next;
		}
		rotations_list_element=rotations_list_element->next;
	}
	
	for(int m=0; m<n; m++){
		k=0;
		while(top_matching[m]!=men_preferences[m*n+k]){
			k++;
		}
		p_star=NULL;
		for(int j=k;j<n;j++){
			woman = men_preferences[m*n+j];
			if(label_matrix[woman*n+m]==NULL) continue;
			if(is_stable_matrix[woman*n+m]){//label di tipo 1
				if(p_star!=NULL){
					new_successor=(struct SuccessorsList*)malloc(sizeof (struct SuccessorsList));
					new_successor->next=p_star->successors;
					new_successor->value=label_matrix[woman*n+m];
					p_star->successors=new_successor;
					label_matrix[woman*n+m]->missing_predecessors++;
				}
				p_star=label_matrix[woman*n+m];
				applied_rotations[label_matrix[woman*n+m]->index]=true;
			} else if(!applied_rotations[label_matrix[woman*n+m]->index] && label_second_condition[m*n+woman]){//label di tipo 2
				new_successor=(struct SuccessorsList*)malloc(sizeof (struct SuccessorsList));
				new_successor->next=label_matrix[woman*n+m]->successors;
				new_successor->value=p_star;
				label_matrix[woman*n+m]->successors=new_successor;
				applied_rotations[label_matrix[woman*n+m]->index]=true;
				p_star->missing_predecessors++;
			}
		}
		//resettare applied_rotations
		for(int j=k;j<n;j++){
			woman=men_preferences[m*n+j];
			if(label_matrix[woman*n+m]!=NULL){
				applied_rotations[label_matrix[woman*n+m]->index]=false;
			}
		}
	}

	free(label_matrix);
	free(label_second_condition);
	free(is_stable_matrix);
	free(last_labelled_woman_index);
	free(last_labelled_man_index);
	free(applied_rotations);
}


void recursive_search(int* matching, int n, struct RotationsListElement* free_rotations_list, struct ResultsList* results_list){
	struct RotationNode* successor;
	struct RotationsListElement* new_list_el;
	struct RotationsListElement* temp;
	struct RotationsListElement* new_free_rotations_list;
	struct SuccessorsList* successors_list;
	struct RotationList* rotation;
	struct RotationsListElement* first_new_rotation;
	
	while(free_rotations_list != NULL){
		//applica la rotazione
		rotation = free_rotations_list->value->rotation;
		
		int first_woman = rotation->woman;
		while(rotation->next != NULL){
			matching[rotation->man] = rotation->next->woman;
			rotation = rotation->next;
		}
		matching[rotation->man] = first_woman;
		
		//aggiungo il matching ai risultati
		int* new_matching = (int*)malloc(sizeof (int) * n);
		for(int i = 0; i < n; i++){
			new_matching[i] = matching[i];
		}
		appendResultsList(results_list,new_matching);
		
		//creo la nuova lista delle rotazioni per la chiamata ricorsiva
		//tolgo la rotazione applicata e aggiungo le nuove rotazioni
		first_new_rotation = NULL;
		new_free_rotations_list = free_rotations_list->next;
		successors_list = free_rotations_list->value->successors;
		while(successors_list != NULL){
			successor = successors_list->value;
			successor->missing_predecessors -= 1;
			if (successor->missing_predecessors == 0){
				//aggiungi questa rotazione in cima alla lista
				new_list_el = (struct RotationsListElement*)malloc(sizeof (struct RotationsListElement));
				new_list_el->value = successor;
				new_list_el->next = new_free_rotations_list;
				new_free_rotations_list = new_list_el;
				if (first_new_rotation == NULL){ //per liberare successivamente la memoria
					first_new_rotation = new_list_el;
				}
			}
			successors_list = successors_list->next;
		}
		
		//chiamata ricorsiva
		recursive_search(matching, n, new_free_rotations_list, results_list);
		
		//ripristino le rotazioni
		//ripristino i missing_predecessors dei successori
		successors_list = free_rotations_list->value->successors;
		while(successors_list != NULL){
			successor = successors_list->value;
			successor->missing_predecessors += 1;
			successors_list = successors_list->next;
		}
		
		//libero la memoria allocata per le nuove rotazioni
		if (first_new_rotation != NULL){
			while(new_list_el != first_new_rotation) {
				temp = new_list_el->next;
				free(new_list_el);
				new_list_el = temp;
			}
			free(new_list_el);
		}
		
		//ripristina rotazione
		rotation = free_rotations_list->value->rotation;
		while(rotation != NULL){
			matching[rotation->man] = rotation->woman;
			rotation = rotation->next;
		}
		
		//ad ogni iterazione, togliamo una rotazione dalla lista
		free_rotations_list = free_rotations_list->next;
	}
}