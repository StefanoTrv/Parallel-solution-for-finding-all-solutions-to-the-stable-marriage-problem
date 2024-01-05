#include <stdlib.h>

//Create a n*n preference matrix
//Seed can be set by calling srand() first
int* make_random_preferences(int n){
	int* preferences = (int*)malloc(sizeof (int) * n * n);
	int* numbers = (int*)malloc(sizeof (int) * n);
	int x;
	for(int i=0;i<n;i++){
		for(int j=0;j<n;j++){
			numbers[j]=j;
		}
		for(int j=0;j<n-1;j++){
			x = rand() % (n-j);
			preferences[i*n+j]=numbers[x];
			numbers[x]=numbers[n-j-1];
		}
		preferences[i*n+n-1]=numbers[0];
	}
	free(numbers);
	return preferences;
}