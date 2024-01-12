#include "../data_structures/data_structures.h"

void free_results(struct ResultsList* results){
	struct ResultsListElement* el = results->first;
	struct ResultsListElement* next_el;
	while(el!=NULL){
		free(el->value);
		next_el=el->next;
		free(el);
		el=next_el;
	}
	free(results);
}


void free_rotations_list_struct(struct RotationsList *rotations_list){
	struct RotationsListElement *rotations_list_element, *temp_rle;
	rotations_list_element = rotations_list->first;
	struct RotationNode *rotation_node;
	struct RotationList *rotation, *temp_r;
	struct SuccessorsList *successors_list, *temp_sl;
	while (rotations_list_element!=NULL){
		rotation_node=rotations_list_element->value;
		rotation=rotation_node->rotation;
		while(rotation!=NULL){
			temp_r=rotation;
			rotation=rotation->next;
			free(temp_r);
		}
		successors_list=rotation_node->successors;
		while(successors_list!=NULL){
			temp_sl=successors_list;
			successors_list=successors_list->next;
			free(temp_sl);
		}
		temp_rle=rotations_list_element;
		rotations_list_element=rotations_list_element->next;
		free(temp_rle);
	}
	free(rotations_list);
}