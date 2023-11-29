#include <stdlib.h>


int test(int**, int, int, int, int);


int* gale_shapley(int n, int** men_preferences, int** women_preferences) {
    int women_partners[n], men_free[n];
    for (int i = 0; i < n; i++) {
        women_partners[i] = -1;
        men_free[i] = 1;
    }

    while (1) {
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

        for (int i = 0; i < n && men_free[m]; i++) {
            int w = men_preferences[m][i];
            if (women_partners[w] == -1) {
                women_partners[w] = m;
                men_free[m] = 0;
            } else {
                int m1 = women_partners[w];
                if (test(women_preferences, n, w, m, m1)) {
                    women_partners[w] = m;
                    men_free[m] = 0;
                    men_free[m1] = 1;
                }
            }
        }
    }
    int* matching = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        matching[women_partners[i]] = i;
    }
    return matching;
}


int test(int** women_preferences, int n, int w, int m, int m1) {
    for (int i = 0; i < n; i++) {
        if (women_preferences[w][i] == m) {
            return 1;
        }
        if (women_preferences[w][i] == m1) {
            return 0;
        }
    }
    return 0;
}
