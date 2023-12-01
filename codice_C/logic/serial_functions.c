#include <stdlib.h>
#include "..\data_structures\data_structures.h"

int test(int*, int, int, int, int);
char* gale_shapley(int, int*, int*);
struct RotationsList* find_all_rotations(int*, int*, int, char*);
void recursive_search(char*, int, struct RotationsListElement*, struct ResultsList*);


char* gale_shapley(int n, int* men_preferences, int* women_preferences) {
    int women_partners[n], men_free[n];
    for (int i = 0; i < n; i++) {
        women_partners[i] = -1;
        men_free[i] = 1;
    }

    while (1) {
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
                men_free[m] = 0;
            } else {
                int m1 = women_partners[w];
                if (accept_proposal(women_preferences, n, w, m, m1)) {
                    women_partners[w] = m;
                    men_free[m] = 0;
                    men_free[m1] = 1;
                }
            }
        }
    }
    char* matching = malloc(n * sizeof(char));
    for (char i = 0; i < n; i++) {
        matching[women_partners[i]] = i;
    }
    return matching;
}


int accept_proposal(int* women_preferences, int n, int w, int m, int m1) {
    for (int i = 0; i < n; i++) {
        if (women_preferences[w * n + i] == m) {
            return 1;
        }
        if (women_preferences[w * n + i] == m1) {
            return 0;
        }
    }
    return 0;
}


struct RotationsList* find_all_rotations(int* men_preferences, int* women_preferences, int n, char* top_matching) {
	struct RotationsList* free_rotations_list = malloc(sizeof (struct RotationsList));
	struct RotationNode* last_to_have_modified[n]; //vettore di puntatori all'ultimo nodo che ha modificato l'uomo
	char* m_i = malloc(sizeof (char) * n);
	char* bottom_matching = gale_shapley(n, women_preferences, men_preferences);
	int marking[n]; //-1 unmarked, n marked ma non associata, altri sono la donna precedente
	int rotation_index = 0; //per indicizzare le rotationi su already_added_predecessors
	int already_added_predecessors[n*n]; //per ridurre il numero di archi nel grafo delle rotazioni

	for (int j = 0; j < n; j++) {
		m_i[j] = top_matching[j];
		marking[j] = -1;
		last_to_have_modified[j] = NULL;
	}
	int men_preferences_indexes[n];
	for (int j = 0; j < n*n; j++) {
		already_added_predecessors[j] = 0;
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
	while (1) {
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
	free(m_i);
	return free_rotations_list->first;
}


void recursive_search(char* matching, int n, struct RotationsListElement* free_rotations_list, struct ResultsList* results_list){
	struct RotationNode* successor;
	struct RotationsListElement* new_list_el;
	struct RotationList* temp;
	struct RotationsListElement* new_free_rotations_list;
	struct SuccessorsList* successors_list;
	struct RotationList* rotation;
	struct RotationList* first_new_rotation;
	
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
		char* new_matching = malloc(sizeof (char) * n);
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
				new_list_el = malloc(sizeof (struct RotationsListElement));
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