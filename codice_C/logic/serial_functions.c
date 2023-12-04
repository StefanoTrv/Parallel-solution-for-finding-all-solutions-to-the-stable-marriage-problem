#include <stdlib.h>
#include "..\data_structures\data_structures.h"

#define false 0
#define true 1

char* gale_shapley(int, int*, int*);
int accept_proposal(int*, int, int, int, int);
struct RotationsList* find_all_rotations(int*, int*, int, char*);
void breakmarriage(char*, int, int, int*, int*, int*, int*, struct RotationsList*, struct RotationNode**, int*, int*);
void pause_breakmarriage(int*, char*, char*, char*, struct RotationsList*, int, int, struct RotationNode**, int*, int*);
void recursive_search(char*, int, struct RotationsListElement*, struct ResultsList*);


char* gale_shapley(int n, int* men_preferences, int* women_preferences) {
    int women_partners[n], men_free[n];
    for (int i = 0; i < n; i++) {
        women_partners[i] = -1;
        men_free[i] = true;
    }

    while (true) {
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

        for (int i = 0; i < n && men_free[m]; i++) {
            int w = men_preferences[m * n + i];
            if (women_partners[w] == -1) {
                women_partners[w] = m;
                men_free[m] = false;
            } else {
                int m1 = women_partners[w];
                if (accept_proposal(women_preferences, n, w, m, m1)) {
                    women_partners[w] = m;
                    men_free[m] = false;
                    men_free[m1] = true;
                }
            }
        }
    }
    char* matching = (char*)malloc(n * sizeof(char));
    for (char i = 0; i < n; i++) {
        matching[women_partners[i]] = i;
    }
    return matching;
}


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


struct RotationsList* find_all_rotations(int* men_preferences, int* women_preferences, int n, char* top_matching) {
	struct RotationsList* free_rotations_list = (struct RotationsList*) malloc(sizeof (struct RotationsList));
	struct RotationNode** last_to_have_modified = (struct RotationNode**)malloc(sizeof (struct RotationNode) * n); //vettore di puntatori all'ultimo nodo che ha modificato l'uomo
	char* m_i = (char*)malloc(sizeof (char) * n);
	char* bottom_matching = gale_shapley(n, women_preferences, men_preferences);
	int* marking = (int*)malloc(sizeof (int) * n); //-1 unmarked, n marked ma non associata, altri sono la donna precedente
	int rotation_index = 0; //per indicizzare le rotationi su already_added_predecessors
	int* already_added_predecessors = (int*)malloc(sizeof (int) * (n*n)); //per ridurre il numero di archi nel grafo delle rotazioni

	for (int j = 0; j < n; j++) {
		m_i[j] = top_matching[j];
		marking[j] = -1;
		last_to_have_modified[j] = NULL;
	}
	int* men_preferences_indexes = (int*)malloc(sizeof (int) * n);
	for (int j = 0; j < n*n; j++) {
		already_added_predecessors[j] = false;
	}
	//escludiamo le preferenze sicuramente non presenti in un matrimonio stabile
	for (int j = 0; j < n; j++) {
		int k = 0; 
		while (men_preferences[k] != m_i[j]) {
			k++;
		}
		men_preferences_indexes[j] = k;
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
		char w = m_i[m];
		marking[w] = n;
		breakmarriage(m_i, m, n, men_preferences, men_preferences_indexes, women_preferences, marking, free_rotations_list, 
					  last_to_have_modified, &rotation_index, already_added_predecessors);
	}
	free(last_to_have_modified);
	free(m_i);
	free(marking);
	free(already_added_predecessors);
	free(men_preferences_indexes);
	return free_rotations_list;
}


void breakmarriage(char* M, int m, int n, int* men_preferences, int* men_preferences_indexes, int* women_preferences, int* marking,
				   struct RotationsList* free_rotations_list, struct RotationNode** last_to_have_modified, int* rotation_index, 
				   int* already_added_predecessors) {
	int i = 0;
	while (men_preferences[m * n + i] != M[m]) {
		i++;
	}
	men_preferences_indexes[m] = i + 1;
	int former_wife = M[m]; //il w dell'articolo, donna con cui l'uomo è accoppiato e si deve separare
	char* reversed_M = (char*)malloc(sizeof (char) * n);
	char* old_reversed_M = (char*)malloc(sizeof (char) * n);
	for (i = 0; i < n; i++) {
		reversed_M[M[i]] = i;
		old_reversed_M[M[i]] = i;
	}
	int previous_woman = n;

	while(true) {
		int breakmarriage_fail = true;
		//itero sulle preferenze di m
		//m diventa m' dell'articolo all'interno del ciclo
		for (i = men_preferences_indexes[m]; i < n; i++) {
			int w = men_preferences[m * n + i]; //il w dell'articolo
			//prendo m1 (attuale compagno di w) e lo confronto con m
			int m1 = reversed_M[w];
			//se w preferisce m a m1, sciolgo (w, m1) e creo (w, m)
			if (marking[w] < 0 && accept_proposal(women_preferences, n, w, m, m1)) { //step 2a
				int k = men_preferences_indexes[m]; //aggiorniamo indice per efficienza
				while (men_preferences[k] != w) {
					k++;
				}
				men_preferences_indexes[m] = k;
				reversed_M[w] = m;
				marking[w] = previous_woman;
				previous_woman = w;
				m = m1; //nuovo uomo non accoppiato
				break;
			} else if (accept_proposal(women_preferences, n, w, m, old_reversed_M[w])) { //step 2b
				int old_marking = marking[w];
				pause_breakmarriage(marking, M, reversed_M, old_reversed_M, free_rotations_list, w, previous_woman, 
									last_to_have_modified, rotation_index, already_added_predecessors);
				if (former_wife == w) { //3c: w = w'
					reversed_M[w] = m;
					return; //al passo 1
				} else {
					if (accept_proposal(women_preferences, n, w, m, m1)) {
						m = m1;
						breakmarriage_fail = false;
						break;
					} else {
						marking[w] = old_marking;
						continue;
					}
					
				}
			}
		}
		if (breakmarriage_fail) { //non abbiamo trovato un accoppiamento stabile per m
			free(reversed_M);
			free(old_reversed_M);
			return;
		}
	}
}


void pause_breakmarriage(int* marking, char* M, char* reversed_M, char* old_reversed_M, struct RotationsList* free_rotations_list, int w, 
					     int previous_woman, struct RotationNode** last_to_have_modified, int* rotation_index, int* already_added_predecessors) {
	int no_predecessors = true;
	struct RotationList* prev_list_el = NULL;
	int w2 = w;
	int go_on = true;
	struct RotationNode* rotation_node = (struct RotationNode*)malloc(sizeof (struct RotationNode));
	rotation_node->missing_predecessors = 0;
	rotation_node->index = *rotation_index;
	*rotation_index += 1;
	struct RotationsList* predecessors_list = (struct RotationsList*)malloc(sizeof (struct RotationsList)); //i predecessori del nodo che stiamo creando, per poter resettare already_added_predecessors
	struct RotationList* list_el = NULL;

	while(w2 != w && go_on) {
		go_on = w2 == w;
		//costruire lista della rotazione dalla coda alla testa
		list_el = (struct RotationList*)malloc(sizeof (struct RotationList));
		list_el->man = old_reversed_M[w2];
		list_el->woman = w2;
		list_el->next = prev_list_el;
		prev_list_el = list_el;
		//aggiorniamo predecessori e last_to_have_modified
		struct SuccessorsList* new_successor = (struct SuccessorsList*)malloc(sizeof (struct SuccessorsList));
		new_successor->value = rotation_node;
		if (last_to_have_modified[old_reversed_M[w2]] != NULL && !already_added_predecessors[last_to_have_modified[old_reversed_M[w2]]->index]) { 
			//aggiungiamo la rotazione solo una volta ad ogni predecessore
			new_successor->next = last_to_have_modified[old_reversed_M[w2]]->successors;
			last_to_have_modified[old_reversed_M[w2]]->successors = new_successor;
			no_predecessors = false;
			rotation_node->missing_predecessors += 1;
			already_added_predecessors[last_to_have_modified[old_reversed_M[w2]]->index] = true;
			appendRotationsList(predecessors_list, last_to_have_modified[old_reversed_M[w2]]);
		} else {
			new_successor->next = NULL;
		}
		last_to_have_modified[old_reversed_M[w2]] = rotation_node;
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

	if (no_predecessors) {
		appendRotationsList(free_rotations_list, rotation_node);
	}

	//ripristiniamo already_added_predecessors, deallocando lo spazio
	struct RotationsListElement* rot_list_el = predecessors_list->first;
	struct RotationsListElement * temp;
	while (rot_list_el != NULL) {
		already_added_predecessors[rot_list_el->value->index] = false;
		temp = rot_list_el;
		rot_list_el = rot_list_el->next;
		free(temp);
	}

	free(predecessors_list);
	return;
}


void recursive_search(char* matching, int n, struct RotationsListElement* free_rotations_list, struct ResultsList* results_list){
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
		
		char first_woman = rotation->woman;
		while(rotation->next != NULL){
			matching[rotation->man] = rotation->next->woman;
			rotation = rotation->next;
		}
		matching[rotation->man] = first_woman;
		
		//aggiungo il matching ai risultati
		char* new_matching = (char*)malloc(sizeof (char) * n);
		for(int i=0;i<n;i++){
			new_matching[i]=matching[i];
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
				if (first_new_rotation==NULL){ //per liberare successivamente la memoria
					first_new_rotation = new_list_el;
				}
			}
		}
		
		//chiamata ricorsiva
		recursive_search(matching, n, new_free_rotations_list, results_list);
		
		//ripristino le rotazioni
		//ripristino i missing_predecessors dei successori
		successors_list = free_rotations_list->value->successors;
		while(successors_list != NULL){
			successor = successors_list->value;
			successor->missing_predecessors += 1;
		}
		
		//libero la memoria allocata per le nuove rotazioni
		if (first_new_rotation!=NULL){
			while(new_list_el!=first_new_rotation) {
				temp = new_list_el->next;
				free(new_list_el);
				new_list_el=temp;
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