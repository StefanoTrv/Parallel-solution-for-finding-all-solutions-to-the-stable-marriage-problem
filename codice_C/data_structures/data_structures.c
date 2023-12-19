#include <stdlib.h>

struct ResultsList {
	struct ResultsListElement* first;
	struct ResultsListElement* last;
};

struct ResultsListElement {
	int* value;	//il matching
	struct ResultsListElement* next;
};

struct RotationList { //list_el
	int man;
	int woman;
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

void appendResultsList(struct ResultsList* list, int* result){
	struct ResultsListElement *new_el = (struct ResultsListElement*)malloc(sizeof (struct ResultsListElement));
	new_el->value = result;
	new_el->next = NULL;
	if (list->first==NULL){
		list->first = new_el;
		list->last=new_el;
	}else{
		list->last->next = new_el;
		list->last = new_el;
	}
}

void appendRotationsList(struct RotationsList* list, struct RotationNode* rotation_node){
	struct RotationsListElement *new_el = (struct RotationsListElement*)malloc(sizeof (struct RotationsListElement));
	new_el->value = rotation_node;
	new_el->next = NULL;
	if (list->first==NULL){
		list->first = new_el;
		list->last=new_el;
	}else{
		list->last->next = new_el;
		list->last = new_el;
	}
}