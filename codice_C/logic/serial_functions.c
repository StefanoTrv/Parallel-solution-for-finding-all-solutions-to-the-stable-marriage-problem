#include <stdlib.h>
#include "..\data_structures\data_structures.h"

#define false 0
#define true 1

char* gale_shapley(int, int*, int*);
int accept_proposal(int*, int, int, int, int);
struct RotationsList* find_all_rotations(int*, int*, int, char*);
void breakmarriage(char*, int, int, int*, int*, int*, int*, struct RotationsList*, struct RotationNode**, int*, int*, int);
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
	free_rotations_list->first=NULL;
	free_rotations_list->last=NULL;
	struct RotationNode** last_to_have_modified = (struct RotationNode**)malloc(sizeof (struct RotationNode) * n); //vettore di puntatori all'ultimo nodo che ha modificato l'uomo
	char* m_i = (char*)malloc(sizeof (char) * n);
	char* bottom_matching = gale_shapley(n, women_preferences, men_preferences);
	int* marking = (int*)malloc(sizeof (int) * n); //-1 unmarked, n marked ma non associata, altri sono la donna precedente
	int rotation_index = 0; //per indicizzare le rotationi su already_added_predecessors
	int* already_added_predecessors = (int*)malloc(sizeof (int) * (n*n)); //per ridurre il numero di archi nel grafo delle rotazioni
	printf("A\n");

	for (int j = 0; j < n; j++) {
		m_i[j] = top_matching[j];
		marking[j] = -1;
		last_to_have_modified[j] = NULL;
	}
	int* men_preferences_indexes = (int*)malloc(sizeof (int) * n);
	for (int j = 0; j < n*n; j++) {
		already_added_predecessors[j] = false;
	}
	printf("B\n");
	//escludiamo le preferenze sicuramente non presenti in un matrimonio stabile
	for (int j = 0; j < n; j++) {
		int k = 0; 
		while (men_preferences[k] != m_i[j]) {
			k++;
		}
		men_preferences_indexes[j] = k;
	}
	printf("C\n");

	int old_m = 0;
	while (true) {
		printf("C1\n");
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
		printf("C2\n");

		if (m < 0) { //m_i == bottom_matching
			break;
		}
		printf("C3\n");

		//STEP 2
		char w = m_i[m];
		marking[w] = n;
		printf("\t\t%i\n",w);
		breakmarriage(m_i, m, n, men_preferences, men_preferences_indexes, women_preferences, marking, free_rotations_list, 
					  last_to_have_modified, &rotation_index, already_added_predecessors, w);
		printf("C4\n");
	}
	printf("D\n");
	free(last_to_have_modified);
	free(m_i);
	free(marking);
	free(already_added_predecessors);
	free(men_preferences_indexes);
	printf("E\n");
	return free_rotations_list;
}


void breakmarriage(char* M, int m, int n, int* men_preferences, int* men_preferences_indexes, int* women_preferences, int* marking,
				   struct RotationsList* free_rotations_list, struct RotationNode** last_to_have_modified, int* rotation_index, 
				   int* already_added_predecessors, int first_woman) {
	printf("\n");
	for (int i=0;i<5;i++){
		printf("%i, ",marking[i]);
	}
	printf("\n");
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
	int w, m1, breakmarriage_fail, k, old_marking;
	int previous_woman = first_woman;
	printf("I\n");

	while(true) {
		printf("II\n");
		breakmarriage_fail = true;
		//itero sulle preferenze di m
		//m diventa m' dell'articolo all'interno del ciclo
		for (i = men_preferences_indexes[m]; i < n; i++) {
			printf("III\n");
			w = men_preferences[m * n + i]; //il w dell'articolo
			//prendo m1 (attuale compagno di w) e lo confronto con m
			m1 = reversed_M[w];
			//se w preferisce m a m1, sciolgo (w, m1) e creo (w, m)
			printf("IV\n");
			if (marking[w] < 0 && accept_proposal(women_preferences, n, w, m, m1)) { //step 2a
				printf("V\n");
				k = men_preferences_indexes[m]; //aggiorniamo indice per efficienza
				while (men_preferences[k] != w) {
					k++;
				}
				men_preferences_indexes[m] = k;
				reversed_M[w] = m;
				marking[w] = previous_woman;
				printf("\t\t 183: %i -> %i\n",w,previous_woman);
				previous_woman = w;
				m = m1; //nuovo uomo non accoppiato
				printf("VI\n");
				break;
			} else if (accept_proposal(women_preferences, n, w, m, old_reversed_M[w])) { //step 2b
				printf("VII\n");
				old_marking = marking[w];
				pause_breakmarriage(marking, M, reversed_M, old_reversed_M, free_rotations_list, first_woman, previous_woman, 
									last_to_have_modified, rotation_index, already_added_predecessors);
				printf("VIII\n");
				if (former_wife == w) { //3c: w = w'
					printf("IX\n");
					reversed_M[w] = m;
					return; //al passo 1
				} else {
					printf("X\n");
					if (accept_proposal(women_preferences, n, w, m, m1)) {
						printf("XI\n");
						m = m1;
						breakmarriage_fail = false;
						break;
					} else {
						printf("XII\n");
						marking[w] = old_marking;
						continue;
					}
					
				}
			}
			printf("XIII\n");
		}
		printf("XIV\n");
		if (breakmarriage_fail) { //non abbiamo trovato un accoppiamento stabile per m
			printf("XV\n");
			free(reversed_M);
			free(old_reversed_M);
			return;
		}
		printf("XVI\n");
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
	printf("\n\t\tasd asd -> %i",rotation_node->index);
	*rotation_index += 1;
	struct RotationsList* predecessors_list = (struct RotationsList*)malloc(sizeof (struct RotationsList)); //i predecessori del nodo che stiamo creando, per poter resettare already_added_predecessors
	predecessors_list->first = NULL;
	struct RotationList* list_el = NULL;
	struct SuccessorsList* new_successor;
	printf("\n.");

	while(w2 != w || go_on) {
		go_on = false;
		printf("\n..");
		//costruire lista della rotazione dalla coda alla testa
		list_el = (struct RotationList*)malloc(sizeof (struct RotationList));
		list_el->man = old_reversed_M[w2];
		list_el->woman = w2;
		printf("\n..a");
		list_el->next = prev_list_el;
		printf("\n..b");
		prev_list_el = list_el;
		printf("\n..c");
		//aggiorniamo predecessori e last_to_have_modified
		new_successor = (struct SuccessorsList*)malloc(sizeof (struct SuccessorsList));
		printf("\n..d");
		new_successor->value = rotation_node;
		printf("\n...");
		if (last_to_have_modified[old_reversed_M[w2]] != NULL && !already_added_predecessors[last_to_have_modified[old_reversed_M[w2]]->index]) {
			printf("\n....");
			//aggiungiamo la rotazione solo una volta ad ogni predecessore
			new_successor->next = last_to_have_modified[old_reversed_M[w2]]->successors;
			last_to_have_modified[old_reversed_M[w2]]->successors = new_successor;
			no_predecessors = false;
			rotation_node->missing_predecessors += 1;
			printf("\n....a");
			already_added_predecessors[last_to_have_modified[old_reversed_M[w2]]->index] = true;
			printf("\n....b");
			appendRotationsList(predecessors_list, last_to_have_modified[old_reversed_M[w2]]);
			printf("\nX");
		} else {
			printf("\nX.");
			new_successor->next = NULL;
			printf("\nX..");
		}
		printf("\nX...");
		last_to_have_modified[old_reversed_M[w2]] = rotation_node;
		printf("\n\t1");
		//aggiorniamo old_reversed_M (i = i+1)
		old_reversed_M[w2] = reversed_M[w2];
		printf("\n\t2");
		//aggiorna M
		M[old_reversed_M[w2]] = w2;
		printf("\n\t3");
		//resettare marking
		marking[w2] = -1;
		printf("\n\t4");
		//update previous_woman
		printf("\n\t\t%i _ %i",w2,w);
		w2 = previous_woman;
		printf("\n\tw2/previous_woman = %i",w2);
		previous_woman = marking[w2];
		printf("\n\t5");
		printf("\nX....");
	}
	printf("\nXX");

	rotation_node->rotation = prev_list_el;

	if (no_predecessors) {
		printf("\nXX.");
		appendRotationsList(free_rotations_list, rotation_node);
	}
	printf("\nXX..");

	//ripristiniamo already_added_predecessors, deallocando lo spazio
	struct RotationsListElement* rot_list_el = predecessors_list->first;
	struct RotationsListElement * temp;
	
	printf("\nXX..a");
	while (rot_list_el != NULL) {
		printf("\nXX..b");
		printf("\n\t\t index = %i",rot_list_el->value->index);
		already_added_predecessors[rot_list_el->value->index] = false;
		printf("\nXX..c");
		temp = rot_list_el;
		printf("\nXX..d");
		rot_list_el = rot_list_el->next;
		printf("\nXX..e");
		free(temp);
		printf("\nXX..f");
	}
	printf("\nXX...");

	free(predecessors_list);
	printf("\nXX....");
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