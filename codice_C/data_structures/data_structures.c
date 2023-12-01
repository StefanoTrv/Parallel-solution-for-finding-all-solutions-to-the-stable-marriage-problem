#include <stdlib.h>

struct ResultsList {
	struct ResultsListElement* first;
	struct ResultsListElement* last;
};

struct ResultsListElement {
	char* value;	//il matching
	struct ResultsListElement* next;
};

struct RotationList { //list_el
	char man;
	char woman;
	struct RotationList* next;
};

struct RotationNode {
	struct RotationList* rotation;
	int index;
	int missing_predecessors;
	struct SuccessorsList* successors;
};

struct SuccessorsList {
	struct RotationNode* value;
	struct SuccessorsList* next;
};

struct RotationsList { //free_rotations_list
	struct RotationsListElement* first;
	struct RotationsListElement* last;
};

struct RotationsListElement { //free_rotations_list
	struct RotationNode* value;
	struct RotationsListElement* next;
};

void appendResultsList(struct ResultsList* list, char* result){
	struct ResultsListElement *new = malloc(sizeof (struct ResultsListElement));
	new->value = result;
	new->next = NULL;
	list->last->next = new;
	list->last = new;
}

void appendRotationsList(struct RotationsList* list, struct RotationNode* rotation_node){
	struct RotationsListElement *new = malloc(sizeof (struct RotationsListElement));
	new->value = rotation_node;
	new->next = NULL;
	list->last->next = new;
	list->last = new;
}