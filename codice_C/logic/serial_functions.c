#include <stdlib.h>


int test(int*, int, int, int, int);
int* gale_shapley(int, int*, int*);
void recursive_search(char*, int, struct RotationsListElement*, struct ResultsList*);


int* gale_shapley(int n, int* men_preferences, int* women_preferences) {
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
            int w = men_preferences[m][i];
            if (women_partners[w] == -1) {
                women_partners[w] = m;
                men_free[m] = 0;
            } else {
                int m1 = women_partners[w];
                if (test(women_preferences, n, w, m, m1)) {
                    women_partners[w] = m;
                    men_free[m] = 0;
                    men_free[m1] = 1;
                }
            }
        }
    }
    int* matching = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        matching[women_partners[i]] = i;
    }
    return matching;
}

void recursive_search(char* matching, int n, struct RotationsListElement* free_rotations_list, struct ResultsList* results_list){
	struct RotationNode* successor;
	struct RotationList* new_list_el;
	struct RotationList* temp;
	struct RotationsListElement* new_free_rotations_list;
	struct SuccessorsList* successors_list;
	struct RotationsList* rotation;
	struct RotationList* first_new_rotation;
	
	while(free_rotations_list != NULL){
		//applica la rotazione
		rotation = free_rotations_list->value->value;
		
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
				new_list_el = malloc(sizeof (struct RotationList));
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
		rotation = free_rotations_list->value->value;
		while(rotation != NULL){
			matching[rotation->man] = rotation->woman;
			rotation = rotation->next;
		}
		
		//ad ogni iterazione, togliamo una rotazione dalla lista
		free_rotations_list = free_rotations_list->next;
	}
}

int test(int* women_preferences, int n, int w, int m, int m1) {
    for (int i = 0; i < n; i++) {
        if (women_preferences[w][i] == m) {
            return 1;
        }
        if (women_preferences[w][i] == m1) {
            return 0;
        }
    }
    return 0;
}
