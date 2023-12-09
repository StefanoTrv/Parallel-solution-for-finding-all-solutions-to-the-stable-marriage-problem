#include <stdlib.h>
#include "..\data_structures\data_structures.h"

#define false 0
#define true 1

char* gale_shapley(int, int*, int*);
int accept_proposal(int*, int, int, int, int);
struct RotationsList* find_all_rotations(int*, int*, int, char*);
void breakmarriage(char*, int, int, int*, int*, int*, int*, struct RotationsList*, struct RotationNode**, int*, int*, int);
void pause_breakmarriage(int*, char*, char*, char*, struct RotationsList*, int, int, int, struct RotationNode**, int*, int*);
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
	printf("accept_proposal: w=%i m=%i m1=%i\n",w,m,m1);
    for (int i = 0; i < n; i++) {
        if (women_preferences[w * n + i] == m) {
            return true;
        }
        if (women_preferences[w * n + i] == m1) {
			//printf("FALSE!!!!!");
            return false;
        }
    }
	//printf("ERROR!");
	//printf("\n");
    return 0;
}


struct RotationsList* find_all_rotations(int* men_preferences, int* women_preferences, int n, char* top_matching) {
	printf("men_preferences:\n");
	for (int i=0;i<n;i++){
		for (int j=0;j<n;j++){
			printf("%i, ",men_preferences[i*n+j]);
		}
		printf("\n");
	}
	printf("women_preferences:\n");
	for (int i=0;i<n;i++){
		for (int j=0;j<n;j++){
			printf("%i, ",women_preferences[i*n+j]);
		}
		printf("\n");
	}
	
	printf("Top matching:\n");
	for (int i=0;i<n;i++){
		printf("%i, ",top_matching[i]);
	}
	printf("\n");
	struct RotationsList* free_rotations_list = (struct RotationsList*) malloc(sizeof (struct RotationsList));
	free_rotations_list->first=NULL;
	free_rotations_list->last=NULL;
	struct RotationNode** last_to_have_modified = (struct RotationNode**)malloc(sizeof (struct RotationNode) * n); //vettore di puntatori all'ultimo nodo che ha modificato l'uomo
	char* m_i = (char*) malloc(sizeof (char) * n);
	char* inverted_bottom_matching = gale_shapley(n, women_preferences, men_preferences);
	char* bottom_matching = (char*)malloc(sizeof (char) * n);
	for(int i=0;i<n;i++){
		bottom_matching[inverted_bottom_matching[i]]=i;
	}
	printf("Bottom matching:\n");
	for(int i=0;i<n;i++){
		printf("%i, ", bottom_matching[i]);
	}
	printf("\n");
	
	int* marking = (int*) malloc(sizeof (int) * n); //-1 unmarked, n marked ma non associata, altri sono la donna precedente
	int rotation_index = 0; //per indicizzare le rotationi su already_added_predecessors
	int* already_added_predecessors = (int*) malloc(sizeof (int) * (n*n)); //per ridurre il numero di archi nel grafo delle rotazioni
	printf("A\n");

	for (int j = 0; j < n; j++) {
		m_i[j] = top_matching[j];
		marking[j] = -1;
		last_to_have_modified[j] = NULL;
	}
	int* men_preferences_indexes = (int*) malloc(sizeof (int) * n);
	for (int j = 0; j < n*n; j++) {
		already_added_predecessors[j] = false;
	}
	printf("B\n");
	//escludiamo le preferenze sicuramente non presenti in un matrimonio stabile
	printf("men_preferences_indexes[j]\n");
	for (int j = 0; j < n; j++) {
		int k = 0; 
		while (men_preferences[j * n + k] != m_i[j]) {
			k++;
		}
		men_preferences_indexes[j] = k+1;//scartiamo quelle attuali, teniamo solo quelle diverse
		printf("%i  ",men_preferences_indexes[j]);
	}
	printf("\nC\n");

	int m = 0;
	while (true) {
		printf("C1\n");
		//STEP 1
		for (int i = 0; i < n; i++) {
			if (m_i[i] != bottom_matching[i] && i < n) {
				break;
			} else {
				printf("D\n");
				free(last_to_have_modified);
				free(m_i);
				free(marking);
				free(already_added_predecessors);
				free(men_preferences_indexes);
				free(inverted_bottom_matching);
				free(bottom_matching);
				printf("E\n");
				return free_rotations_list;
			}
		}

		for (int i = 0; i < n; i++) {
			if (m_i[i] != bottom_matching[i]) {
				m = i;
			}
			marking[i] = -1;
		}
		
		printf("while di find_all_rotations::\tm = %i, w = %i",m,m_i[m]);
		
		//STEP 2
		char w = m_i[m];
		marking[w] = n;
		printf("\t\t%i\n",w);
		breakmarriage(m_i, m, n, men_preferences, men_preferences_indexes, women_preferences, marking, free_rotations_list, 
					  last_to_have_modified, &rotation_index, already_added_predecessors, w);
		printf("\n");
		printf("C3\n");
	}
}


void breakmarriage(char* M, int m, int n, int* men_preferences, int* men_preferences_indexes, int* women_preferences, int* marking,
				   struct RotationsList* free_rotations_list, struct RotationNode** last_to_have_modified, int* rotation_index, 
				   int* already_added_predecessors, int first_woman) {
	printf("breakmarriage:\n");
	for (int i=0;i<n;i++){
		printf("%i, ",marking[i]);
	}
	printf("\n");
	printf("\nM[i] = ");
	for (int i=0;i<n;i++){
		printf("%i, ",M[i]);
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

	while(true) {
		printf("\nwhile di breakmarriage: +++++++++++++++++++++++++\n");
		printf("marking\n");
		for (int i=0;i<n;i++){
			printf("%i, ",marking[i]);
		}
		printf("\nreversed_M\n");
		for (int i=0;i<n;i++){
			printf("%i, ",reversed_M[i]);
		}
		printf("\n+++++++++++++++++++++++++\n");
		breakmarriage_fail = true;
		//itero sulle preferenze di m
		//m diventa m' dell'articolo all'interno del ciclo
		printf("m_p_i[%i] = %i\n",m,men_preferences_indexes[m]);
		for (i = men_preferences_indexes[m]; i < n; i++) {
			printf("\nFOR di breakmarriage: +++++++++++++++++++++++++\n");
			printf("marking\n");
			for (int i=0;i<n;i++){
				printf("%i, ",marking[i]);
			}
			printf("\nreversed_M\n");
			for (int i=0;i<n;i++){
				printf("%i, ",reversed_M[i]);
			}
			printf("\n+++++++++++++++++++++++++\n");
			printf("III\t\ti: %i, m: %i, mpi[m * n + i]: %i, ",i,m,men_preferences[m * n + i]);
			w = men_preferences[m * n + i]; //il w dell'articolo
			//prendo m1 (attuale compagno di w) e lo confronto con m
			m1 = reversed_M[w];
			printf("m1: %i\n",m1);
			//se w preferisce m a m1, sciolgo (w, m1) e creo (w, m)
			printf("La donna %i che ha il marking %i ha ricevuto dall'uomo %i una proposta.\n",w,marking[w],m);
			if (marking[w] < 0 && accept_proposal(women_preferences, n, w, m, m1)) { //step 2a
				printf("Proposta accettata senza marking.\n");
				printf("V\n");
				k = men_preferences_indexes[m]; //aggiorniamo indice
				while (men_preferences[k] != w) {
					k++;
				}
				men_preferences_indexes[m] = k+1;
				reversed_M[w] = m;
				marking[w] = previous_woman;
				printf("\t\t 183: %i -> %i\n",w,previous_woman);
				previous_woman = w;
				printf("\t\t m: %i \tm1: %i\n",m,m1);
				m = m1; //nuovo uomo non accoppiato
				printf("VI\n");
				breakmarriage_fail = false;
				printf("VIb %i\n",breakmarriage_fail);
				break;
			} else if (accept_proposal(women_preferences, n, w, m, old_reversed_M[w])) { //step 2b
				breakmarriage_fail = false;
				printf("Proposta che vuole accettare, ma e' marcata!\n");
				printf("VII\n");
				old_marking = marking[w];
				//test
				//k = men_preferences_indexes[m]; //aggiorniamo indice
				//while (men_preferences[k] != w) {
				//	k++;
				//}
				//men_preferences_indexes[m] = k+1;
				reversed_M[w] = m;
				//marking[w] = previous_woman;
				//previous_woman = w;
				//m = m1; //nuovo uomo non accoppiato
				//test
				pause_breakmarriage(marking, M, reversed_M, old_reversed_M, free_rotations_list, w, m, previous_woman, 
									last_to_have_modified, rotation_index, already_added_predecessors);
				reversed_M[w] = m1;
				printf("VIII\n");
				printf("\t\tformer_wife = %i\tw = %i\n",former_wife,w);
				if (former_wife == w) { //3c: w = w'
					printf("IX\n");
					reversed_M[w] = m;
					free(reversed_M);
					free(old_reversed_M);
					return; //al passo 1
				} else {
					printf("X\n");
					if (!accept_proposal(women_preferences, n, w, m, m1)) {
						printf("XII\n");
						marking[w] = old_marking;
						previous_woman = w;
						continue;
					} else {
						printf("\nTERMINA\n");
						printf("XI\n");
						printf("m1 = %i _ w = %i\n", m1, w);
						reversed_M[w] = m;
						m = m1;
						break;
					}
					
				}
			}
			printf("XIII\t\t%i\n",i);
		}
		printf("XIV %i\n",breakmarriage_fail);
		if (breakmarriage_fail) { //non abbiamo trovato un accoppiamento stabile per m
			printf("XV\n");
			printf("NON DOVREBBE MAI ESSERE QUI!\n");
			free(reversed_M);
			free(old_reversed_M);
			return;
		}
		printf("XVI\n");
	}
}


void pause_breakmarriage(int* marking, char* M, char* reversed_M, char* old_reversed_M, struct RotationsList* free_rotations_list, int w, int m, 
					     int previous_woman, struct RotationNode** last_to_have_modified, int* rotation_index, int* already_added_predecessors) {
	printf("pause_breakmarriage:\n");
	for (int i=0;i<3;i++){
		printf("%i, ",marking[i]);
	}
	printf("\n");
	for (int i=0;i<3;i++){
		printf("%i, ",reversed_M[i]);
	}
	printf("\n");
	int no_predecessors = true;
	struct RotationList* prev_list_el = NULL;
	int w2 = w;
	int go_on = true;
	struct RotationNode* rotation_node = (struct RotationNode*)malloc(sizeof (struct RotationNode));
	rotation_node->missing_predecessors = 0;
	rotation_node->index = *rotation_index;
	rotation_node->successors=NULL;
	printf("\n\t\trotation node index -> %i",rotation_node->index);
	*rotation_index += 1;
	struct RotationsList* predecessors_list = (struct RotationsList*)malloc(sizeof (struct RotationsList)); //i predecessori del nodo che stiamo creando, per poter resettare already_added_predecessors
	predecessors_list->first = NULL;
	struct RotationList* list_el = NULL;
	struct SuccessorsList* new_successor;
	printf("\nprevious_woman: %i\n", previous_woman);
	printf("\n.");
	printf("\nw = %i\n",w);
	printf("\nquesta e' w2 %i\n", w2);
	while(w2 != w || go_on) {
		printf("\nWHILE di pause: ----------------------\n");
		for (int i=0;i<3;i++){
			printf("%i, ",marking[i]);
		}
		printf("\n");
		go_on = false;
		printf("\n..");
		//costruire lista della rotazione dalla coda alla testa
		list_el = (struct RotationList*)malloc(sizeof (struct RotationList));
		list_el->man = old_reversed_M[w2];
		list_el->woman = w2;
		printf("\ncoppia rotazione: uomo->%i donna->%i",list_el->man,list_el->woman);
		printf("\n..a");
		list_el->next = prev_list_el;
		printf("\n..b");
		prev_list_el = list_el;
		printf("\n..c");
		//aggiorniamo predecessori e last_to_have_modified
		if (last_to_have_modified[old_reversed_M[w2]] != NULL && !already_added_predecessors[last_to_have_modified[old_reversed_M[w2]]->index]) {
			new_successor = (struct SuccessorsList*)malloc(sizeof (struct SuccessorsList));
			printf("\n..d");
			new_successor->value = rotation_node;
			printf("\n...");
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
		printf("\n\t\tmarking[w2]: %i, w2: %i, ora diventa -1",marking[w2],w2);
		marking[w2] = -1;
		printf("\n\t3");
		//update previous_woman
		printf("\n\t\tw2: %i _ previous_woman:%i",w2,previous_woman);
		w2 = previous_woman;
		previous_woman = marking[w2];
		printf("\n\t5");
		printf("\nX....");
		printf("\nfine del while di pause:\nM[i] = ");
		for (int i=0;i<3;i++){
			printf("%i, ",M[i]);
		}
		printf("\nreversed_M\n");
		for (int i=0;i<3;i++){
			printf("%i, ",reversed_M[i]);
		}
		printf("\nold_reversed_M\n");
		for (int i=0;i<3;i++){
			printf("%i, ",old_reversed_M[i]);
		}
		printf("\nmarking\n");
		for (int i=0;i<3;i++){
			printf("%i, ",marking[i]);
		}
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
	printf("\nXX....\n");
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
		printf("\n!");
		//applica la rotazione
		rotation = free_rotations_list->value->rotation;
		
		char first_woman = rotation->woman;
		while(rotation->next != NULL){
			matching[rotation->man] = rotation->next->woman;
			rotation = rotation->next;
		}
		matching[rotation->man] = first_woman;
		printf("\n!!");
		
		//aggiungo il matching ai risultati
		char* new_matching = (char*)malloc(sizeof (char) * n);
		for(int i=0;i<n;i++){
			new_matching[i]=matching[i];
		}
		appendResultsList(results_list,new_matching);
		printf("\n!!!");
		
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
			successors_list = successors_list->next;
		}
		printf("\n!!!!");
		
		//chiamata ricorsiva
		recursive_search(matching, n, new_free_rotations_list, results_list);
		printf("\nFine chiamata ricorsiva");
		
		//ripristino le rotazioni
		//ripristino i missing_predecessors dei successori
		successors_list = free_rotations_list->value->successors;
		while(successors_list != NULL){
			successor = successors_list->value;
			successor->missing_predecessors += 1;
			successors_list=successors_list->next;
		}
		printf("\n!!!!!");
		
		//libero la memoria allocata per le nuove rotazioni
		if (first_new_rotation!=NULL){
			while(new_list_el!=first_new_rotation) {
				temp = new_list_el->next;
				free(new_list_el);
				new_list_el=temp;
			}
			free(new_list_el);
		}
		printf("\n!!!!!!");
		
		//ripristina rotazione
		rotation = free_rotations_list->value->rotation;
		while(rotation != NULL){
			matching[rotation->man] = rotation->woman;
			rotation = rotation->next;
		}
		printf("\n!!!!!!!");
		
		//ad ogni iterazione, togliamo una rotazione dalla lista
		free_rotations_list = free_rotations_list->next;
		printf("\n!!!!!!!!");
	}
}