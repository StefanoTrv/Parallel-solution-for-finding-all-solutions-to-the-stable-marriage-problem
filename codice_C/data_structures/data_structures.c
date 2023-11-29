struct ResultsList {
	ResultsListElement* first;
	ResultsListElement* last;
}

struct ResultsListElement {
	char* value;	//il matching
	ResultsListElement* next;
}

struct RotationList { //list_el
	char man;
	char woman;
	RotationList* next;
}

struct RotationNode {
	RotationList* rotation;
	int index;
	int missing_predecessors;
	SuccessorsList* successors;
}

struct SuccessorsList {
	RotationNode* value;
	SuccessorsList* next;
}

struct RotationsList { //free_rotations_list
	RotationsListElement* first;
	RotationsListElement* last;
}

struct RotationsListElement { //free_rotations_list
	RotationNode* value;
	ResultsListElement* next;
}

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