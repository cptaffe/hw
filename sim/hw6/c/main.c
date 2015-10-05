
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/*
 * Mersenne Twister PRNG extern
 */
extern double genrand_res53(void);

int *simulate(int n0, int m0) {
	int i, m, *p = calloc(sizeof(int), n0);
	for (m = 0; m < m0; m++) {
		int n, *chairs = calloc(sizeof(int), n0);
		for (i = 0; i < n0; i++) {
			chairs[i] = i+1;
		}
		for (n = n0; n > 1; n--) {
			int i, *nchairs = calloc(sizeof(int), n-1);
			for (i = 0; i < n; i++) {
				int j;
				if (chairs[i] == 0) continue;
				j = (i == 0) ? 0 : (i == n-1) ? n-2 : i-(genrand_res53() <= 0.5);
				if (nchairs[j] == 0 || genrand_res53() <= 0.5) {
					nchairs[j] = chairs[i];
				}
			}
			free(chairs);
			chairs = nchairs;
		}
		p[chairs[0]-1]++;
		free(chairs);
	}
	return p;
}

int main(int argc, char *argv[]) {
	/*
	 * Monte Carlo Simulation: Homework 6
	 * Connor Taffe
	 */
	int i, n, m, *p;
	if (argc != 3) {
		printf("Usage: %s n m\n", argv[0]);
		exit(1);
	} else {
		n = strtol(argv[1], NULL, 10);
		m = strtol(argv[2], NULL, 10);
	}
	printf("Running %d trials of %d columns.\n", m, n);

	p = simulate(n, m);
	for (i = 0; i < n; i++) {
		printf("%i: %Lf%%\n", i+1, ((long double) p[i])/m);
	}
	free(p);
}
