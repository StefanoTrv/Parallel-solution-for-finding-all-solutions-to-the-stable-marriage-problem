#include <stdlib.h>
#include "..\data_structures\data_structures.h"

#define false 0
#define true 1


__global__ void build_graph_CUDA(int, int, int, int*, int*, int*, int*, int*, int*);

__device__ int* label_matrix;
__device__ int* is_stable_matrix;
__device__ int* label_second_condition;
__device__ int* applied_rotations;
__shared__ int* first_men_preferences_index;
__shared__ int* first_women_preferences_index;

__global__ void build_graph_CUDA(int n, int number_of_rotations, int total_number_of_pairs, int* rotations_vector, int* end_displacement_vector, int* top_matching, int* women_preferences, int* men_preferences, int* triangular_matrix){
	int woman, i, j, first_woman, man, next_woman, k, p_star, iterations;
	if(threadIdx.x==0){
		label_matrix = (int*)malloc(sizeof (int) * n * n);
		is_stable_matrix = (int*)malloc(sizeof (int) * n * n);
		label_second_condition = (int*)malloc(sizeof (int) * n * n);
		applied_rotations = (int*)malloc(sizeof (int) * number_of_rotations * n);
		first_men_preferences_index = (int*)malloc(sizeof (int) * n);
		first_women_preferences_index = (int*)malloc(sizeof (int) * n);
	}
	__syncthreads();

	for(i=threadIdx.x;i<n;i+=blockDim.x){
		for(j=0; j<n; j++){
			label_matrix[i*n+j]=n;
			is_stable_matrix[j*n+i]=false;
			label_second_condition[j*n+i]=false;
		}
		woman = top_matching[i];
	}
	__syncthreads();

	for(i=threadIdx.x;i<n;i+=blockDim.x){
		is_stable_matrix[top_matching[i]*n+i]=true;
		for(j=0;j<n;j++){
			if(top_matching[j]==men_preferences[j*n+i]) first_men_preferences_index[j]=i;
			if(j==women_preferences[top_matching[j]*n+i]) first_women_preferences_index[top_matching[j]]=i;
		}
	}
	__syncwarp();

	for (i=threadIdx.x;i<number_of_rotations;i+=blockDim.x){
		for(j=0;j<n;j++){
			applied_rotations[j*n+i]=false;
		}
	}
	__syncwarp();

	for (i = threadIdx.x; i < ((number_of_rotations-1)*number_of_rotations)/2; i+=blockDim.x) {
		triangular_matrix[i] = false;
		//printf("%i ",triangular_matrix[i]);
	}
	__syncwarp();

	iterations = number_of_rotations/blockDim.x + ((number_of_rotations%blockDim.x < 1) ? 0 : 1);
	for (i=threadIdx.x;i<iterations*blockDim.x;i+=blockDim.x){
		if(i<number_of_rotations){
			if(i==0){
				j=0;
			}else{
				j=end_displacement_vector[i-1]+1;
			}
			printf("\nThread: %i\ti: %i\tj: %i",threadIdx.x,i,j);
		} else {
			printf("\nThread: %i\ti: %i\tNON ESEGUITO",threadIdx.x,i);
		}
		__syncwarp();
		if(i<number_of_rotations){
			first_woman=rotations_vector[total_number_of_pairs+j];
			for(;j<=end_displacement_vector[i];j++){
				man=rotations_vector[j];
				if(j==end_displacement_vector[i]){
					next_woman=first_woman;
				}else{
					next_woman=rotations_vector[total_number_of_pairs+j+1];
				}
				woman=rotations_vector[total_number_of_pairs+j];
				printf("\nCoppia della rotazione %i: (%i,%i).\t(i=%i, j=%i, w_index=%i)",i,man,woman,i,j,total_number_of_pairs+j);
				//aggiorna rispetto alla donna
				k=first_women_preferences_index[next_woman]-1;
				printf("\nkw[%i] = %i",next_woman,k);
				while(women_preferences[next_woman*n+k]!=man){
					atomicMin(label_matrix + (next_woman*n+women_preferences[next_woman*n+k]),i);// => label_matrix[next_woman*n+women_preferences[next_woman*n+k]]=i;
					printf("\nlabel_matrix[%i = (%i, %i)] = %i\t(type 2)",next_woman*n+women_preferences[next_woman*n+k],next_woman,women_preferences[next_woman*n+k],label_matrix[next_woman*n+women_preferences[next_woman*n+k]]);
					k--;
				}
				//aggiorna rispetto all'uomo
				k=first_men_preferences_index[man]+1;
				printf("\nkm[%i] = %i",man,k);
				while(men_preferences[man*n+k]!=next_woman){
					label_second_condition[man*n+men_preferences[man*n+k]]=true;
					printf("\n\tlabel_second_condition[%i = (%i,%i)] = %i\t(type 2)",man*n+men_preferences[man*n+k],man,men_preferences[man*n+k],label_second_condition[man*n+men_preferences[man*n+k]]);
					k++;
				}

				is_stable_matrix[next_woman*n+man]=true;
				printf("\nLa coppia uomo %i e donna %i e' stabile.",man, woman);
				atomicMin(label_matrix + (woman*n+man),i);// => label_matrix[woman*n+man]=i;
				printf("\nlabel_matrix[%i = (%i,%i)] = %i\t(type 1)",(woman*n+man),woman,man,label_matrix[(woman*n+man)]);
			}
		}
		__syncwarp();
	}
	__syncthreads();
	if(threadIdx.x==0){
		printf("\nLABEL_MATRIX:\n");
		for(i=0;i<n;i++){
			for(j=0; j<n; j++){
				printf("%i\t",label_matrix[i*n+j]);
			}
			printf("\n");
		}
	}
	if(threadIdx.x==0){
		printf("\nIS STABLE MATRIX:\n");
		for(i=0;i<n;i++){
			for(j=0; j<n; j++){
				printf("%i\t",is_stable_matrix[i*n+j]);
			}
			printf("\n");
		}
	}
	if(threadIdx.x==0){
		printf("\nLABEL SECOND CONDITION:\n");
		for(i=0;i<n;i++){
			for(j=0; j<n; j++){
				printf("%i\t",label_second_condition[i*n+j]);
			}
			printf("\n");
		}
	}

	iterations = n/blockDim.x + ((n%blockDim.x < 1) ? 0 : 1);
	for(man=threadIdx.x;man<iterations*blockDim.x;man+=blockDim.x){
		if(man<n){
			k=0;
			while(top_matching[man]!=men_preferences[man*n+k]){
				k++;
			}
			//printf("\n2. Thread: %i\tman: %i\tk: %i\tm_p[]: %i",threadIdx.x,man,k,men_preferences[man*n+k]);
		} else {
			//printf("\n2. Thread: %i\tman: %i\tNON ESEGUITO",threadIdx.x,man);
		}
		__syncwarp();
		if(man<n){
			p_star=-1;
			for(j=k;j<n;j++){
				printf("Thread %i, man = %i, j = %i, p_star = %i, label_matrix[%i] = %i\n",threadIdx.x,man,j,p_star,woman*n+man,label_matrix[woman*n+man]);
				woman = men_preferences[man*n+j];
				if(label_matrix[woman*n+man]==n) continue;
				if(is_stable_matrix[woman*n+man]){//label di tipo 1
					if(p_star!=-1){
						triangular_matrix[(label_matrix[woman*n+man]-1)*number_of_rotations+p_star]=true;
						printf("triangular_matrix[%i] = %i (type 1, thread: %i)\tda %i a %i\n",(label_matrix[woman*n+man]-1)*number_of_rotations+p_star,triangular_matrix[(label_matrix[woman*n+man]-1)*number_of_rotations+p_star],threadIdx.x,p_star,label_matrix[woman*n+man]);
					}
					p_star=label_matrix[woman*n+man];
					applied_rotations[man*n+label_matrix[woman*n+man]]=true;
				} else if(!applied_rotations[man*n+label_matrix[woman*n+man]] && label_second_condition[man*n+woman]){//label di tipo 2
					triangular_matrix[(p_star-1)*number_of_rotations+label_matrix[woman*n+man]]=true;
					printf("triangular_matrix[%i] = %i  (type 2, thread: %i)\tda %i a %i\n",(p_star-1)*number_of_rotations+label_matrix[woman*n+man],triangular_matrix[(p_star-1)*number_of_rotations+label_matrix[woman*n+man]],threadIdx.x,label_matrix[woman*n+man],p_star);
					applied_rotations[man*n+label_matrix[woman*n+man]]=true;
				}
			}
		}
		__syncwarp();
	}
	__syncthreads();
	
	if(threadIdx.x==0){
		printf("\nLABEL_MATRIX:\n");
		for(i=0;i<n;i++){
			for(j=0; j<n; j++){
				printf("%i\t",label_matrix[i*n+j]);
			}
			printf("\n");
		}
		printf("\nTriangular matrix:\n");
		for(i=1;i<number_of_rotations;i++){
			for(j=0; j<i; j++){
				printf("%i (%i, %i)\t",triangular_matrix[(i-1)*number_of_rotations+j],i,j);
			}
			printf("\n");
		}
		printf("\nTriangular matrix:\n");
		for (i = 0; i < ((number_of_rotations-1)*number_of_rotations)/2; i++) {
			printf("%i ",triangular_matrix[i]);
		}
		printf("\n");
		free(label_matrix);
		free(is_stable_matrix);
		free(label_second_condition);
		free(applied_rotations);
	}
}
