#include "../data_structures/data_structures.h"

void free_results(struct ResultsList* results){
	struct ResultsListElement* el = results->first;
	struct ResultsListElement* next_el;
	while(el!=null){
		free(el->value);
		next_el=el->next;
		free(el);
		el=next_el;
	}
	free(results);
}