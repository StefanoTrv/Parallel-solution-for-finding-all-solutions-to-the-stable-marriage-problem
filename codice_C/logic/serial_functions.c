#include <stdlib.h>
#include "..\data_structures\data_structures.h"

#define false 0
#define true 1

int* gale_shapley(int, int*, int*);
int accept_proposal(int*, int, int, int, int);
struct RotationsList* find_all_rotations(int*, int*, int, int*);
void breakmarriage(int*, int, int, int*, int*, int*, int*, struct RotationsList*, int*, int);
void pause_breakmarriage(int*, int*, int*, int*, struct RotationsList*, int, int, int*);
void build_graph(int, struct RotationsList*, int*, int*, int*);
void recursive_search(int*, int, struct RotationsListElement*, struct ResultsList*);


int* gale_shapley(int n, int* men_preferences, int* women_preferences) {
    int women_partners[n], men_free[n];
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
    return matching;
}


//True se w accetta la proposta di m, False se preferisce rimanere con m1 
int accept_proposal(int* women_preferences, int n, int w, int m, int m1) {
	printf("accept_proposal: w=%i m=%i m1=%i\n",w,m,m1);
    for (int i = 0; i < n; i++) {
        if (women_preferences[w * n + i] == m) {
            return true;
        }
        if (women_preferences[w * n + i] == m1) {
			printf("\nProposta rifiutata\n");
            return false;
        }
    }
    return 0;
}


struct RotationsList* find_all_rotations(int* men_preferences, int* women_preferences, int n, int* top_matching) {
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
	struct RotationsList* rotations_list = (struct RotationsList*) malloc(sizeof (struct RotationsList));
	rotations_list->first = NULL;
	rotations_list->last = NULL;
	int* m_i = (int*) malloc(sizeof (int) * n);
	int* inverted_bottom_matching = gale_shapley(n, women_preferences, men_preferences);
	int* bottom_matching = (int*)malloc(sizeof (int) * n); //per avere il bottom matching dal pov degli uomini
	for(int i = 0; i < n; i++){
		bottom_matching[inverted_bottom_matching[i]] = i;
	}
	printf("\nBottom matching:\n");
	for(int i=0;i<n;i++){
		printf("%i, ", bottom_matching[i]);
	}
	printf("\n");
	
	int* marking = (int*) malloc(sizeof (int) * n); //-1 unmarked, n marked ma non associata, altri sono la donna precedente
	int rotation_index = 0; //per indicizzare le rotationi (DA TOGLIERE QUESTA VARIABILE?)

	for (int j = 0; j < n; j++) {
		m_i[j] = top_matching[j];
		marking[j] = -1;
	}
	int* men_preferences_indexes = (int*) malloc(sizeof (int) * n);

	//escludiamo le preferenze sicuramente non presenti in un matrimonio stabile
	printf("men_preferences_indexes[j]\n");
	for (int j = 0; j < n; j++) {
		int k = 0; 
		while (men_preferences[j * n + k] != m_i[j]) {
			k++;
		}
		men_preferences_indexes[j] = k;//quelle attuali verranno scartate successivamente
		printf("%i  ",men_preferences_indexes[j]);
	}

	int old_m = 0;
	while (true) {
		printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\nINIZIO ITERAZIONE CICLO DI FIND_ALL_ROTATIONS\n");
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
		
		printf("while di find_all_rotations::\tm = %i, w = %i",m,m_i[m]);
		
		//STEP 2
		int w = m_i[m];
		marking[w] = n;
		printf("\t\t%i\n",w);
		breakmarriage(m_i, m, n, men_preferences, men_preferences_indexes, women_preferences, marking, rotations_list, &rotation_index, w);
		
		printf("\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	}
	free(m_i);
	free(marking);
	free(men_preferences_indexes);
	free(inverted_bottom_matching);
	free(bottom_matching);
	return rotations_list;
}


void breakmarriage(int* M, int m, int n, int* men_preferences, int* men_preferences_indexes, int* women_preferences, int* marking,
				   struct RotationsList* rotations_list, int* rotation_index, int first_woman) {
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
		printf("L'indice meno uno punta a %i, mentre la donna precedente era %i.\n",men_preferences[m*n+men_preferences_indexes[m]-1],M[m]);
		for (i = men_preferences_indexes[m]; i < n; i++) {
			printf("\nFOR di breakmarriage con m=%i e w=%i: +++++++++++++++++++++++++\n",m,men_preferences[m * n + i]);
			printf("marking\n");
			for (int i=0;i<n;i++){
				printf("%i, ",marking[i]);
			}
			printf("\nreversed_M\n");
			for (int i=0;i<n;i++){
				printf("%i, ",reversed_M[i]);
			}
			printf("\n+++++++++++++++++++++++++\n");
			printf("i: %i, m: %i, mpi[m * n + i]: %i, ",i,m,men_preferences[m * n + i]);
			w = men_preferences[m * n + i]; //il w dell'articolo
			//prendo m1 (attuale compagno di w) e lo confronto con m
			m1 = reversed_M[w];
			printf("m1: %i\n",m1);
			//se w preferisce m a m1, sciolgo (w, m1) e creo (w, m)
			printf("L'uomo %i propone alla donna %i, che ha il marking %i.\n",m,w,marking[w]);
			if (marking[w] < 0 && accept_proposal(women_preferences, n, w, m, m1)) { //step 2a
				printf("Proposta accettata senza marking.\n");
				k = men_preferences_indexes[m]; //aggiorniamo indice
				while (men_preferences[m * n + k] != w) {
					k++;
				}
				men_preferences_indexes[m]=k;
				printf("Updated men_preferences_index[%i]=%i",m,k+1);
				reversed_M[w] = m;
				marking[w] = previous_woman;
				printf("\t\t w passa da a : %i -> %i\n",previous_woman,w);
				previous_woman = w;
				printf("\t\t m: %i \tm1: %i\n",m,m1);
				m = m1; //nuovo uomo non accoppiato
				breakmarriage_fail = false;
				break;
			} else if (marking[w] >= 0 && accept_proposal(women_preferences, n, w, m, old_reversed_M[w])) { //step 2b
				breakmarriage_fail = false;
				printf("Proposta che vuole accettare, ma e' marcata!\n");
				old_marking = marking[w];
				reversed_M[w] = m;
				pause_breakmarriage(marking, M, reversed_M, old_reversed_M, rotations_list, w, previous_woman, rotation_index);
				reversed_M[w] = m1;
				printf("\t\tformer_wife = %i\tw = %i\n",former_wife,w);
				
				k = men_preferences_indexes[m]; //aggiorniamo indice
				while (men_preferences[m * n + k] != w) {
					k++;
				}
				men_preferences_indexes[m]=k;

				if (former_wife == w) { //3c: w = w'
					printf("3c\n");
					reversed_M[w] = m;

					free(reversed_M);
					free(old_reversed_M);
					return; //al passo 1
				} else {//3d
					if (!accept_proposal(women_preferences, n, w, m, m1)) {
						printf("qwerty\n");
						marking[w] = old_marking;
						previous_woman = w;
						continue;//al passo 2
					} else {
						printf("\nAZKABAN! %i %i\n",marking[w],m1);
						reversed_M[w] = m;
						m = m1;
						previous_woman = M[m];
						break;
					}
				}
			}
		}
		if (breakmarriage_fail) { //non abbiamo trovato un accoppiamento stabile per m
			printf("ERRORE! NON DOVREBBE MAI ESSERE QUI!");
			free(reversed_M);
			free(old_reversed_M);
			return;
		}
	}
}


void pause_breakmarriage(int* marking, int* M, int* reversed_M, int* old_reversed_M, struct RotationsList* rotations_list, int w, 
					     int previous_woman, int* rotation_index) {
	printf("pause_breakmarriage:\n");
	for (int i=0;i<8;i++){
		printf("%i, ",marking[i]);
	}
	printf("\n");
	for (int i=0;i<8;i++){
		printf("%i, ",reversed_M[i]);
	}
	printf("\n");
	struct RotationList* prev_list_el = NULL;
	int w2 = w;
	int go_on = true;
	struct RotationNode* rotation_node = (struct RotationNode*)malloc(sizeof (struct RotationNode));
	rotation_node->missing_predecessors = 0;
	rotation_node->index = *rotation_index;
	rotation_node->successors = NULL;
	printf("\n\t\trotation node index -> %i",rotation_node->index);
	*rotation_index += 1;
	struct RotationList* list_el = NULL;
	
	while(w2 != w || go_on) {
		printf("\nWHILE di pause: ----------------------\n");
		for (int i=0;i<8;i++){
			printf("%i, ",marking[i]);
		}
		printf("\n");
		printf("w2: %i\t-\tprevious_woman:%i\n",w2,previous_woman);
		go_on = false;
		//costruire lista della rotazione dalla coda alla testa
		list_el = (struct RotationList*)malloc(sizeof (struct RotationList));
		list_el->man = old_reversed_M[w2];
		list_el->woman = w2;
		printf("\ncoppia rotazione: uomo->%i donna->%i",list_el->man,list_el->woman);
		list_el->next = prev_list_el;
		prev_list_el = list_el;

		//aggiorniamo old_reversed_M (i = i+1)
		old_reversed_M[w2] = reversed_M[w2];
		//aggiorna M
		M[old_reversed_M[w2]] = w2;
		//resettare marking
		printf("\n\t\tmarking[w2]: %i, w2: %i, ora diventa -1",marking[w2],w2);
		marking[w2] = -1;
		//update previous_woman
		w2 = previous_woman;
		previous_woman = marking[w2];
		printf("\nfine del while di pause:\nM[i] = ");
		for (int i=0;i<8;i++){
			printf("%i, ",M[i]);
		}
		printf("\nreversed_M\n");
		for (int i=0;i<8;i++){
			printf("%i, ",reversed_M[i]);
		}
		printf("\nold_reversed_M\n");
		for (int i=0;i<8;i++){
			printf("%i, ",old_reversed_M[i]);
		}
		printf("\nmarking\n");
		for (int i=0;i<8;i++){
			printf("%i, ",marking[i]);
		}
		printf("\nNuovo w2 per il prossimo ciclo: %i",w2);
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
	printf("%p != %p (NULL)\n",rotations_list->first->value, NULL);

	for (int i=0; i<n; i++){
		for(int j=0; j<n; j++){
			label_matrix[i*n+j]=NULL;
			is_stable_matrix[j*n+i]=false;
			label_second_condition[j*n+i]=false;
		}
		woman = top_matching[i];
		last_labelled_woman_index[woman]=n-1;
		while(women_preferences[woman*n+last_labelled_woman_index[woman]]!=i){
			printf("woman: %i, llw: %i\t",woman,last_labelled_woman_index[woman]);
			last_labelled_woman_index[woman]--;
		}
		last_labelled_man_index[i]=0;
		while(men_preferences[i*n+last_labelled_man_index[i]]!=woman){
			printf("man: %i, llm: %i\t",i,last_labelled_man_index[i]);
			last_labelled_man_index[i]++;
		}
		printf("\n");
		is_stable_matrix[top_matching[i]*n+i]=true;
	}
	printf("\n");printf("\n");

	for(int i=0; i<n; i++){
		for(int j=0; j<n; j++){
			printf("%i\t", is_stable_matrix[i*n+j]);
		}
		printf("\n");
	}
	printf("\n");printf("\n");
	printf("\n");printf("\n");

	for(int i=0; i<n; i++){
		for(int j=0; j<n; j++){
			printf("%i\t", !(label_matrix[i*n+j]==NULL));
		}
		printf("\n");
	}
	printf("\n");printf("\n");

	for (int i=0; i<rotations_list->last->value->index+1; i++){
		applied_rotations[i]=false;
	}

	printf("Inizio scrittura matrice etichette.\n");

	while(rotations_list_element!=NULL){
		printf("$");
		pair_list_element = rotations_list_element->value->rotation;
		first_woman=pair_list_element->woman;
		while(pair_list_element!=NULL){
			printf(".");
			man=pair_list_element->man;
			if(pair_list_element->next==NULL){
				next_woman=first_woman;
			}else{
				next_woman=pair_list_element->next->woman;
			}
			woman=pair_list_element->woman;
			printf("\n\tO - man: %i, woman: %i, next_woman: %i, last_labelled_woman_index: %i\n\t",man,woman,next_woman,last_labelled_woman_index[next_woman]-1);
			//aggiorna rispetto alla donna
			index=last_labelled_woman_index[next_woman]-1;
			while(women_preferences[next_woman*n+index]!=man){
				printf("'(wp: %i, man: %i, index: %i)",women_preferences[next_woman*n+index],man,index);
				label_matrix[next_woman*n+women_preferences[next_woman*n+index]]=rotations_list_element->value;
				index--;
			}
			last_labelled_woman_index[next_woman]=index;
			//aggiorna rispetto all'uomo
			index=last_labelled_man_index[man]+1;
			while(men_preferences[man*n+index]!=next_woman){
				printf("'(mp: %i, woman: %i, index: %i)",men_preferences[man*n+index],next_woman,index);
				label_second_condition[man*n+men_preferences[man*n+index]]=true;
				index++;
			}
			last_labelled_man_index[man]=index;

			is_stable_matrix[next_woman*n+man]=true;
			label_matrix[woman*n+man]=rotations_list_element->value;
			pair_list_element=pair_list_element->next;
		}
		printf("\n");
		rotations_list_element=rotations_list_element->next;
	}
	printf("\n");printf("\n");

	for(int i=0; i<n; i++){
		for(int j=0; j<n; j++){
			printf("%i\t", is_stable_matrix[i*n+j]);
		}
		printf("\n");
	}
	printf("\n");printf("\n");
	printf("\n");printf("\n");

	for(int i=0; i<n; i++){
		for(int j=0; j<n; j++){
			if(label_matrix[i*n+j]==NULL){
				printf("_\t");
			}else{
				printf("%i\t", label_matrix[i*n+j]->index);
			}
		}
		printf("\n");
	}
	printf("\n");printf("\n");
	
	for(int m=0; m<n; m++){
		k=0;
		while(top_matching[m]!=men_preferences[m*n+k]){
			k++;
		}
		printf("k = %i\n",k);
		p_star=NULL;
		for(int j=k;j<n;j++){
			woman = men_preferences[m*n+j];
			printf("\tm: %i, j: %i, woman: %i\t",m,j,woman);
			if(label_matrix[woman*n+m]==NULL){
				printf("null\n");
				continue;
			}
			printf("\n");
			if(is_stable_matrix[woman*n+m]){//label di tipo 1
				printf("Label di tipo 1 (man: %i, woman: %i)\n",m,woman);
				if(p_star!=NULL){
					printf("Ci siamo!\n");
					new_successor=(struct SuccessorsList*)malloc(sizeof (struct SuccessorsList));
					new_successor->next=p_star->successors;
					new_successor->value=label_matrix[woman*n+m];
					p_star->successors=new_successor;//add
					label_matrix[woman*n+m]->missing_predecessors++;
					printf("label_matrix[woman*n+m]->index: %i\tp->successors: %p\tNULL: %p\tlabel_matrix[woman*n+m]->missing_predecessors: %i\n",label_matrix[woman*n+m]->index,p_star->successors,NULL,label_matrix[woman*n+m]->missing_predecessors);
				}
				p_star=label_matrix[woman*n+m];//aggiunto questo
				applied_rotations[label_matrix[woman*n+m]->index]=true;
			} else if(!applied_rotations[label_matrix[woman*n+m]->index] && label_second_condition[m*n+woman]){//label di tipo 2
				if(p_star==NULL) {
					printf("WHAT???\n");
					continue;
				}
				printf("Label di tipo 2\n");
				printf("label_matrix[woman*n+m]->index: %i\tp->successors: %p\tNULL: %p\tlabel_matrix[woman*n+m]->missing_predecessors: %i\n",label_matrix[woman*n+m]->index,p_star->successors,NULL,label_matrix[woman*n+m]->missing_predecessors);
				new_successor=(struct SuccessorsList*)malloc(sizeof (struct SuccessorsList));
				printf(".");
				new_successor->next=label_matrix[woman*n+m]->successors;
				new_successor->value=p_star;
				printf(",");
				label_matrix[woman*n+m]->successors=new_successor;
				applied_rotations[label_matrix[woman*n+m]->index]=true;
				printf(".%p\n",p_star);
				p_star->missing_predecessors++;//add
				printf("Nuovo missing_predecessors per %i: %i\n",p_star->index,p_star->missing_predecessors);
				printf(",");
			}else{
				printf("No label\n");
			}
		}
		//resettare applied_rotations
		for(int j=k;j<n;j++){
			woman=men_preferences[m*n+j];
			if(label_matrix[woman*n+m]==NULL){
				continue;
			}
			applied_rotations[label_matrix[woman*n+m]->index]=false;		
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
		printf("\nFine chiamata ricorsiva");
		
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