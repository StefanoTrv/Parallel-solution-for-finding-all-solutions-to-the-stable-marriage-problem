#include <stdlib.h>

//crea una singola tabella n*n di preferenze
//si può impostare il seme chiamando prima srand()
int* make_random_preferences(int n){
	int* preferences = malloc(sizeof (int) * n * n);
	int* numbers = malloc(sizeof (int) * n);
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
	return preferences;
}