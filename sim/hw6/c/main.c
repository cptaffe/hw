
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "mt19937ar.c"

struct {
	int threads;
} Config;

struct simThread {
	int t, m0, n0;
};

void *simThread(void *v) {
	MT19937State mts;
	const struct simThread s = *(struct simThread *) v;
	int m, *p = calloc(sizeof(int), s.n0);
	/* Initlialize mersenne twister. */
	mts.i = MT19937_N+1;
	memset(&mts.a, 0, sizeof(mts.a));
	MT19937InitGenRand(&mts, s.t);
	/* Free struct simThread */
	free(v);
	for (m = 0; m < s.m0; m++) {
		int i, n, *chairs = calloc(sizeof(int), s.n0);
		for (i = 0; i < s.n0; i++) {
			chairs[i] = i+1;
		}
		for (n = s.n0; n > 1; n--) {
			int i, *nchairs = calloc(sizeof(int), n-1);
			for (i = 0; i < n; i++) {
				int j;
				if (chairs[i] == 0) continue;
				j = (i == 0) ? 0 : (i == n-1) ? n-2 : i-(MT19937GenRandRes53(&mts) <= 0.5);
				if (nchairs[j] == 0 || MT19937GenRandRes53(&mts) <= 0.5) {
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

int *simulate(int n0, int m0) {
	int i, *p = calloc(sizeof(int), n0), *pt;
	struct simThread *s = calloc(1, sizeof(struct simThread));
	pthread_t *ts = calloc(Config.threads, sizeof(pthread_t));
	for (i = 0; i < Config.threads; i++) {
		struct simThread *s = calloc(1, sizeof(struct simThread));
		s->m0 = m0/Config.threads;
		s->n0 = n0;
		s->t = i;
		pthread_create(&ts[i], NULL, simThread, (void *) s);
	}

	/* Run inline */
 	s->m0 = m0%Config.threads;
 	s->n0 = n0;
	pt = simThread((void *) s);
	for (i = 0; i < n0; i++) {
 		p[i] += pt[i];
 	}

	for (i = 0; i < Config.threads; i++) {
		int j, *pt;
		pthread_join(ts[i], (void **) &pt);
		for (j = 0; j < n0; j++) {
			p[j] += pt[j];
		}
		free(pt);
	}
	return p;
}

int main(int argc, char *argv[]) {
	/*
	 * Monte Carlo Simulation: Homework 6
	 * Connor Taffe
	 */
	int n, j, n0, n1, m, *p;
	if (argc != 4) {
		printf("Usage: %s n0 n1 m\n", argv[0]);
		exit(1);
	} else {
		n0 = strtol(argv[1], NULL, 10);
		n1 = strtol(argv[2], NULL, 10);
		m = strtol(argv[3], NULL, 10);
	}
	Config.threads = sysconf(_SC_NPROCESSORS_ONLN);
	printf("Running %d trials of %d to %d chairs. Utilizing %d %s.\n", m, n0, n1, Config.threads, Config.threads==1 ? "thread" : "threads");

	for (n = n0; n <= n1; n++) {
		p = simulate(n, m);
		printf(" === %d ===\n", n);
		for (j = 0; j < n; j++) {
			printf("%d: %Lf\n", j+1, ((long double) p[j])/m);
		}
		free(p);
	}
}
