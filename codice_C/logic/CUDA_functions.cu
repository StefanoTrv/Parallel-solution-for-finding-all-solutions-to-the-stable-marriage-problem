#include <stdlib.h>
#include "..\data_structures\data_structures.h"

#define false 0
#define true 1


__global__ void build_graph_CUDA(int, int, int*, int*, int*, int*, int*, int*);

__device__ int* label_matrix;
__device__ int* is_stable_matrix;
__device__ int* label_second_condition;
__device__ int* applied_rotations;

__global__ void build_graph_CUDA(int n, int number_of_rotations, int* rotations_vector, int* end_displacement_vector, int* top_matching, int* women_preferences, int* men_preferences, int* triangular_matrix){
	int woman, i, j, first_woman, man, next_woman, k, p_star, iterations;

	if(threadIdx.x==0){
		label_matrix = (int*)malloc(sizeof (int) * n * n);
		is_stable_matrix = (int*)malloc(sizeof (int) * n * n);
		label_second_condition = (int*)malloc(sizeof (int) * n * n);
		applied_rotations = (int*)malloc(sizeof (int) * number_of_rotations);
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
	}
	__syncwarp();

	for (i=threadIdx.x;i<number_of_rotations;i+=blockDim.x){
		applied_rotations[i]=false;
	}
	__syncwarp();

	iterations = number_of_rotations/blockDim.x + ((number_of_rotations%blockDim.x < 1) ? 0 : 1);
	for (i=threadIdx.x;i<iterations;i+=blockDim.x){
		if(i<number_of_rotations){
			if(i==0){
				j=0;
			}else{
				j=end_displacement_vector[i-1]+1;
			}
		}
		__syncwarp();
		if(i<number_of_rotations){
			first_woman=rotations_vector[number_of_rotations+j];
			for(;j<=end_displacement_vector[i];j++){
				man=rotations_vector[j];
				if(j==end_displacement_vector[i]){
					next_woman=first_woman;
				}else{
					next_woman=rotations_vector[number_of_rotations+j+1];
				}
				woman=rotations_vector[number_of_rotations+j];
				//aggiorna rispetto alla donna
				k=n-1;
				while(women_preferences[next_woman*n+k]!=man){
					atomicMin(label_matrix + (next_woman*n+women_preferences[next_woman*n+k]),i);// => label_matrix[next_woman*n+women_preferences[next_woman*n+k]]=i;
					k--;
				}
				//aggiorna rispetto all'uomo
				k=0;
				while(men_preferences[man*n+k]!=next_woman){
					label_second_condition[man*n+men_preferences[man*n+k]]=true;
					k++;
				}

				is_stable_matrix[next_woman*n+man]=true;
				atomicMin(label_matrix + (woman*n+man),i);// => label_matrix[woman*n+man]=i;
			}
		}
		__syncwarp();
	}
	__syncthreads();

	iterations = n/blockDim.x + ((n%blockDim.x < 1) ? 0 : 1);
	for(man=threadIdx.x;man<iterations;man+=blockDim.x){
		if(man<n){
			k=0;
			while(top_matching[man]!=men_preferences[man*n+k]){
				k++;
			}
		}
		__syncwarp();
		if(man<n){
			p_star=-1;
			for(j=k;j<n;j++){
				woman = men_preferences[man*n+j];
				if(label_matrix[woman*n+man]==n) continue;
				if(is_stable_matrix[woman*n+man]){//label di tipo 1
					if(p_star!=-1){
						triangular_matrix[((p_star-1)*p_star)/2+label_matrix[woman*n+man]]=true;
					}
					p_star=label_matrix[woman*n+man];
					applied_rotations[label_matrix[woman*n+man]]=true;
				} else if(!applied_rotations[label_matrix[woman*n+man]] && label_second_condition[man*n+woman]){//label di tipo 2
					triangular_matrix[((label_matrix[woman*n+man]-1)*label_matrix[woman*n+man])/2+p_star]=true;
					applied_rotations[label_matrix[woman*n+man]]=true;
				}
			}
		}
		__syncwarp();
		//resettare applied_rotations
		if(man<n){
			for(j=k;j<n;j++){
				woman=men_preferences[man*n+j];
				applied_rotations[label_matrix[woman*n+man]]=false;
			}
		}
		__syncwarp();
	}
	__syncthreads();
	
	if(threadIdx.x==0){
		free(label_matrix);
		free(is_stable_matrix);
		free(label_second_condition);
		free(applied_rotations);
	}
}
